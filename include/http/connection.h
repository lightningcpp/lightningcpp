/*
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include "constant.h"
#include "socket.h"

#include "utils/httpparser.h"

namespace http {

/** \brief type definition for the server delegate pointer. */
typedef std::function< void ( Request&, Response & ) > callback_ptr;

/**
 * @brief class representing a server connection.

a connection class is created for each connection. After connection close or error the
object will be destoryed.

 * the request execution call flow:
 *\msc
 *  Server, Connection, Socket, Response, Server;
 *
 *  Server rbox Connection [label="wait for connection", textbgcolour="#ffb000"];
 *  Server->Server [label="establish", URL="\ref Server::start ()"];
 *  Server->Connection [label="start", URL="\ref Connection::start ()"];
 *  Connection->Socket [label="read", URL="\ref Socket::read ( buffer_t &, delegate_t )"];
 *  Connection rbox Socket [label="connect, read request for connection", textbgcolour="#00ff00"];
 *  Socket->Connection [label="connect", URL="\ref Connection::connect"];
 *  Connection->Socket [label="read", URL="\ref Socket::read ( buffer_t&, http_delegate_t )"];
 *  Socket->Connection [label="read", URL="\ref void Connection::read (const std::error_code &e, std::size_t size)"];
 *  Server rbox Connection [label="process request", textbgcolour="#ffb000"];
 *  Connection=>Server [label="execute", URL="\ref void Connection::read (const std::error_code &e, std::size_t size)"];
 *  Server=>Response [label="write attribute", URL="\ref void Connection::read (const std::error_code &e, std::size_t size)"];
 *  Response>>Server [label=""];
 *  Server>>Connection [label="Response"];
 *  Connection rbox Response [label="send response", textbgcolour="#ff0000"];
 *  Connection=>Response [label="read header", URL="\ref void Response::header (const std::error_code &e, std::size_t size)"];
 *  Connection<<Response [label="buffer"];
 *  Connection->Socket [label="write header", URL="\ref void Socket::write (const std::error_code &e, std::size_t size)"];
 *  Socket->Connection [label="write", URL="\ref void Connection::write ( const std::error_code& )"];
 *  Connection=>Response [label="read attribute", URL="\ref void Response::header (const std::error_code &e, std::size_t size)"];
 *  Connection<<Response [label="buffer"];
 *  Connection->Socket [label="write attr", URL="\ref void Socket::write (const std::error_code &e, std::size_t size)"];
 *  Connection rbox Socket [label="close or reset connection", textbgcolour="#ffb000"];
 *  Connection->Connection [label="close|start", URL="\ref Connection::start ()"];
 *\endmsc
 */
class Connection : public std::enable_shared_from_this< Connection > {
public:
    /**
     * @brief Connection CTOR.
     * @param socket the socket for this connection.
     * @param callback the server callback method for requests.
     */
    Connection ( socket_ptr && socket, callback_ptr callback ) :
        socket_ ( std::move( socket ) ), callback_ ( callback ) {}

    /** @brief start the connection */
    void start() {
        request_.reset();
        response_.reset();
        socket_->read ( buffer_,
            std::bind( &Connection::connect, shared_from_this(), _1, _2 ) );
    }

    /**
     * @brief Connect with new request.
     * @param e the error code.
     * @param size Size of buffer.
     * @return
     */
    void connect ( const std::error_code& e, std::streamsize size ) {
        if( !e ) {
            std::cout << "CONNECT: " << size << std::endl;
            size_t body_start_ = http_parser_.parse_request ( request_, buffer_, 0, size );
            std::cout << ">> CONNECT: body_start: " << body_start_ << std::endl;

            if ( body_start_ == 0 ) { //continue to read the header.
                std::cout << "CONNECT: header incomplete" << std::endl;
                socket_->read( buffer_, std::bind( &Connection::connect, shared_from_this(), _1, _2 ) );

            } else {

                size_t _body_length = body_length();
                if ( _body_length == 0 ) {
                    std::cout << "CONNECT: without body" << std::endl;
                    callback_ ( request_, response_ );
                    size_t _buffer_size = response_.header ( buffer_.data(), BUFFER_SIZE );
                    socket_->write( buffer_, _buffer_size, std::bind( &Connection::write, shared_from_this(), _1 ) );
                } else {

                    if ( size - body_start_ <= _body_length ) {
                        std::cout << "CONNECT: body complete:" << std::string ( buffer_.data(), _body_length ) << std::endl;
                        request_.write ( buffer_.data()+body_start_, _body_length );
                        callback_ ( request_, response_ );

                        size_t _buffer_size = response_.header ( buffer_.data(), BUFFER_SIZE );
                        socket_->write( buffer_, _buffer_size, std::bind( &Connection::write, shared_from_this(), _1 ) );

                    } else if ( size - body_start_ > 0 ) {
                        std::cout << "CONNECT: read more:" << std::string ( buffer_.data(), body_start_, size - body_start_ ) << std::endl;
                        request_.write ( buffer_.data()+body_start_, size - body_start_ );
                        socket_->read( buffer_, std::bind( &Connection::read, shared_from_this(), _1, _2 ) );

                    } else {
                        socket_->read( buffer_, std::bind( &Connection::read, shared_from_this(), _1, _2 ) );
                    }
                }
            }
        } else std::cout << "ERROR(connect): " << e.message() << std::endl;
    }

    /**
     * @brief parse request input content
     * @return
     */
    void read ( const std::error_code& e, std::size_t size ) {
        if( !e ) {
            std::cout << "READ: " << size << std::endl;
            size_t _body_length = body_length();
            request_.write ( buffer_.data(), size );
            if( static_cast< size_t >( request_.tellp() ) == _body_length ) { //execute request

                std::cout << "READ: complete: " << std::endl;
                callback_ ( request_, response_ );
                size_t _buffer_size = response_.header ( buffer_.data(), BUFFER_SIZE );
                socket_->write( buffer_, _buffer_size, std::bind( &Connection::write, shared_from_this(), _1 ) );

            } else if( static_cast< size_t >( request_.tellp() ) < _body_length ) {
                std::cout << "READ: more: " << std::endl;
                socket_->read( buffer_, std::bind( &Connection::read, shared_from_this(), _1, _2 ) );

            } else {
                std::cout << "READ: parse_content: Wrong size: " << request_.tellp() << ", " << _body_length << std::endl;
                response_.status( http_status::BAD_REQUEST ); //TODO
                size_t _buffer_size = response_.header ( buffer_.data(), BUFFER_SIZE );
                socket_->write( buffer_, _buffer_size, std::bind( &Connection::write, shared_from_this(), _1 ) );
            }
        } else std::cout << "ERROR(read): " << e.message() << std::endl;
    }

    /**
     * @brief parse request output content.
     * @param e
     */
    void write ( const std::error_code& e ) {
        if( !e ) {
            std::cout << "WRITE: no size" << std::endl;
            size_t _body_length = response_length();
            if( static_cast< size_t >( response_.tellg() ) == _body_length ) { //finish request
                if( request_.persistent() ) {
                    std::cout << "WRITE: persistent" << std::endl;
                    start(); //restart this connection

                } else socket_->close();

            } else if( static_cast< size_t >( response_.tellg() ) < _body_length ) {
                size_t _write_position = response_.read ( buffer_.data(), BUFFER_SIZE );
                socket_->write( buffer_, _write_position, std::bind( &Connection::write, shared_from_this(), _1 ) );
            } else {
                std::cout << "send entity: Wrong size." << std::endl;
                response_.status( http_status::BAD_REQUEST ); //TODO
                socket_->close();
            }
        } else std::cout << "ERROR (WRITE(e)): " << e.message() << std::endl;
    }

private:
    socket_ptr socket_ = nullptr;
    callback_ptr callback_;
    Request request_;
    Response response_;
    utils::HttpParser http_parser_;
    buffer_t buffer_;

    size_t body_length() {
        if( request_.contains_parameter( http::header::CONTENT_LENGTH ) )
            return std::stoul ( request_.parameter ( http::header::CONTENT_LENGTH ) );
        else return 0U;
    }
    size_t response_length() {
        if( response_.contains_parameter( http::header::CONTENT_LENGTH ) )
            return std::stoul ( response_.parameter ( http::header::CONTENT_LENGTH ) );
        else return 0U;
    }

};
/** \brief socket pointer */
typedef std::unique_ptr< Socket > server_socket_ptr;
}//namespace http
#endif //namespace HTTP_CONNECTION_H
