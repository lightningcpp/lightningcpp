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

#include "httpconfig.h"
#include "socket.h"
#include "utils/httpparser.h"

namespace http {

typedef std::function< void ( Request&, Response & ) > callback_ptr;

class Connection : public std::enable_shared_from_this< Connection > {
public:

    Connection ( socket_ptr && socket, callback_ptr callback ) :
        socket_ ( std::move( socket ) ), callback_ ( callback ) {}

    void start() {
        socket_->read ( buffer_,
            std::bind( &Connection::connect, shared_from_this(), _1, _2 ) );
    }

    /**
     * @brief Parse request.
     * @param buffer Reference to the buffer.
     * @param size Size of buffer
     * @return
     */
    void connect ( const asio::error_code& e, std::size_t size ) {
        if( !e ) {
            size_t body_start_ = http_parser_.parse_request ( request_, buffer_, 0, size );

            if ( body_start_ == 0 ) { //continue to read the header.
                socket_->read( buffer_, std::bind( &Connection::read, shared_from_this(), _1, _2 ) );

            } else {

                if ( body_length() == 0 ) {
                    callback_ ( request_, response_ );
                    size_t _buffer_size = response_.header ( buffer_ );
                    socket_->write( buffer_, _buffer_size, std::bind( &Connection::write, shared_from_this(), _1 ) );
                }
            }
        }
    }

    /**
     * @brief parse request input content
     * @return
     */
    void read ( const asio::error_code& e, std::size_t size ) {
        if( !e ) {
            size_t _body_length = body_length();
            request_.write ( buffer_, 0, size );
            if( static_cast< size_t >( request_.tellp() ) == _body_length ) { //execute request

                response_.reset(); // = std::make_shared< Response >();
                callback_ ( request_, response_ );

                size_t _buffer_size = response_.header ( buffer_ );
                socket_->write( buffer_, _buffer_size, std::bind( &Connection::write, shared_from_this(), _1 ) );

            } else if( static_cast< size_t >( request_.tellp() ) < _body_length ) {
                socket_->read( buffer_, std::bind( &Connection::read, shared_from_this(), _1, _2 ) );

            } else {
                std::cout << "parse_content: Wrong size." << std::endl;
                response_.status( http_status::BAD_REQUEST ); //TODO
                size_t _buffer_size = response_.header ( buffer_ );
                socket_->write( buffer_, _buffer_size, std::bind( &Connection::write, shared_from_this(), _1 ) );
            }
        }
    }

    void write ( const asio::error_code& e ) {
        if( !e ) {
            size_t _body_length = response_length();
            if( static_cast< size_t >( response_.tellg() ) == _body_length ) { //finish request
                if( request_.persistent() ) {
                    start(); //restart this connection

                } else socket_->close();

            } else if( static_cast< size_t >( response_.tellg() ) < _body_length ) {
                size_t _write_position = response_.read ( buffer_ );
                socket_->write( buffer_, _write_position, std::bind( &Connection::write, shared_from_this(), _1 ) );
            } else {
                std::cout << "send entity: Wrong size." << std::endl;
                response_.status( http_status::BAD_REQUEST ); //TOOD
                socket_->close();
            }
        }
    }

private:
    socket_ptr socket_ = nullptr;
    callback_ptr callback_;
    Request request_;
    Response response_;
    utils::HttpParser http_parser_;
    buffer_t buffer_; //TODO

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
typedef std::unique_ptr< Socket > server_socket_ptr;
}//namespace http
#endif //namespace HTTP_CONNECTION_H
