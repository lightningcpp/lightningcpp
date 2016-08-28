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
#ifndef HTTPSESSION_H
#define HTTPSESSION_H

#include <array>
#include <functional>
#include <memory>

#include "httpconfig.h"
#include "httpserverconnection.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "utils/httpparser.h"


namespace http {

typedef std::function< void ( HttpRequest&, HttpResponse & ) > callback_ptr;
/**
 * @brief The HttpSession manages different types of connection and creates request, response objects.
 */
class HttpSession : public std::enable_shared_from_this< HttpSession > {
public:
    HttpSession ( server_connection_ptr && connection, callback_ptr callback ) :
        connection_ ( std::move( connection ) ), callback_ ( callback ), request_ ( std::make_shared< HttpRequest >() ), response_ ( std::make_shared< HttpResponse >() ) {
    }

    /**
     * @brief start start the session.
     */
    void start() {
        request_.reset( new HttpRequest() );
        response_.reset( new HttpResponse() );
        connection_->read ( buffer_, std::bind( &HttpSession::parse_request, shared_from_this(), std::placeholders::_1, std::placeholders::_2 ) );
    }

    /**
     * @brief Parse request.
     * @param buffer Reference to the buffer.
     * @param size Size of buffer
     * @return
     */
    void parse_request ( const asio::error_code& e, std::size_t size ) {
        if( !e ) {
            size_t body_start_ = http_parser_.parse_request ( *request_.get(), buffer_, 0, size );

            if ( body_start_ == 0 ) { //continue to read the header.
                connection_->read( buffer_, std::bind( &HttpSession::parse_request, shared_from_this(), std::placeholders::_1, std::placeholders::_2 ) );

            } else {

                size_t _body_length = body_length();
                if ( _body_length == 0 ) {
                    callback_ ( *request_.get(), *response_.get() );
                    size_t _buffer_size = response_->header ( buffer_ );
                    connection_->write( buffer_, _buffer_size, std::bind( &HttpSession::send_entity, shared_from_this(), std::placeholders::_1 ) );
                } else {

                    if ( size - body_start_ <= _body_length ) {
                        request_->write ( buffer_, body_start_, _body_length );
                        response_ = std::make_shared< HttpResponse >();
                        callback_ ( *request_.get(), *response_.get() );

                        size_t _buffer_size = response_->header ( buffer_ );
                        connection_->write( buffer_, _buffer_size, std::bind( &HttpSession::send_entity, shared_from_this(), std::placeholders::_1 ) );

                    } else if ( size - body_start_ > 0 ) {
                        request_->write ( buffer_, body_start_, size - body_start_ );
                        connection_->read( buffer_, std::bind( &HttpSession::parse_content, shared_from_this(), std::placeholders::_1, std::placeholders::_2 ) );

                    } else {
                        connection_->read( buffer_, std::bind( &HttpSession::parse_content, shared_from_this(), std::placeholders::_1, std::placeholders::_2 ) );
                    }
                }
            }
        }
    }

    /**
     * @brief parse request input content
     * @return
     */
    void parse_content ( const asio::error_code& e, std::size_t size ) {
        if( !e ) {
            size_t _body_length = body_length();
            request_->write ( buffer_, 0, size );
            if( static_cast< size_t >( request_->tellp() ) == _body_length ) { //execute request

                response_ = std::make_shared< HttpResponse >();
                callback_ ( *request_.get(), *response_.get() );

                size_t _buffer_size = response_->header ( buffer_ );
                connection_->write( buffer_, _buffer_size, std::bind( &HttpSession::send_entity, shared_from_this(), std::placeholders::_1 ) );

            } else if( static_cast< size_t >( request_->tellp() ) < _body_length ) {
                connection_->read( buffer_, std::bind( &HttpSession::parse_content, shared_from_this(), std::placeholders::_1, std::placeholders::_2 ) );

            } else {
                std::cout << "parse_content: Wrong size." << std::endl;
                response_->status( http_status::BAD_REQUEST ); //TODO
                size_t _buffer_size = response_->header ( buffer_ );
                connection_->write( buffer_, _buffer_size, std::bind( &HttpSession::send_entity, shared_from_this(), std::placeholders::_1 ) );
            }
        }
    }

    void send_entity( const asio::error_code& e ) {
        if( !e ) {
            size_t _body_length = response_length();
            if( static_cast< size_t >( response_->tellg() ) == _body_length ) { //finish request
                if( request_->persistent() ) {
                    start();
                } else connection_->close();

            } else if( static_cast< size_t >( response_->tellg() ) < _body_length ) {
                size_t _write_position = response_->read ( buffer_ );
                connection_->write( buffer_, _write_position, std::bind( &HttpSession::send_entity, shared_from_this(), std::placeholders::_1 ) );
            } else {
                std::cout << "send entity: Wrong size." << std::endl;
                response_->status( http_status::BAD_REQUEST ); //TOOD
                connection_->close();
            }
        }
    }

    /**
	 * @brief reset the state between requests.
	 */
    void reset() {
        //TODO
    }

private:
    size_t body_length() {
        if( request_->contains_parameter( http::header::CONTENT_LENGTH ) )
            return std::stoul ( request_->parameter ( http::header::CONTENT_LENGTH ) );
        else return 0U;
    }
    size_t response_length() {
        if( response_->contains_parameter( http::header::CONTENT_LENGTH ) )
            return std::stoul ( response_->parameter ( http::header::CONTENT_LENGTH ) );
        else return 0U;
    }

    server_connection_ptr connection_ = nullptr;
    callback_ptr callback_;
	utils::HttpParser http_parser_;
    std::shared_ptr< HttpRequest > request_;
	std::shared_ptr< HttpResponse > response_;
    buffer_t buffer_; //TODO
};
}//namespace http
#endif // HTTPSESSION_H
