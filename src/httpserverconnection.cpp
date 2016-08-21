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

#include "httpserverconnection.h"

namespace http {
HttpServerConnection::HttpServerConnection ( std::shared_ptr< HttpSession > session, asio::io_service& io_service ) :
	session_ ( session ),
	strand_ ( io_service ),
	socket_ ( io_service ) {}

asio::ip::tcp::socket& HttpServerConnection::socket() {
	return socket_;
}

void HttpServerConnection::start () {
	socket_.async_read_some ( asio::buffer ( buffer_ ),
							  strand_.wrap (
								  std::bind ( &HttpServerConnection::handle_read, shared_from_this(),
										  std::placeholders::_1,
										  std::placeholders::_2 ) ) );
}

void HttpServerConnection::handle_read ( const asio::error_code& e, std::size_t bytes_transferred ) {
	if ( !e ) {
		response_ = session_->parse_request ( buffer_, bytes_transferred );

		if ( response_ ) {
			size_t _buffer_size = response_->header ( buffer_ );
			asio::async_write ( socket_, asio::buffer ( buffer_, _buffer_size ),
								strand_.wrap (
									std::bind ( &HttpServerConnection::handle_write, shared_from_this(),
												std::placeholders::_1 ) ) );

		} else {
			socket_.async_read_some ( asio::buffer ( buffer_ ),
									  strand_.wrap (
										  std::bind ( &HttpServerConnection::handle_read, shared_from_this(),
												  std::placeholders::_1,
												  std::placeholders::_2 ) ) );
		}

	} else if ( e != asio::error::eof ) {
		std::cout << "handle read error: " << e.category().name() << ':' << e.message() << std::endl;
	}
}

void HttpServerConnection::handle_write ( const asio::error_code& e ) {
	if ( !e ) {
		int next_size = response_->read ( buffer_ );

		if ( next_size > 0 ) {

			//write next chunk
			asio::async_write ( socket_, asio::buffer ( buffer_, next_size ), strand_.wrap (
									std::bind ( &HttpServerConnection::handle_write, shared_from_this(), std::placeholders::_1 ) ) );

		} else if ( session_->persistent() ) {

			session_->reset();
			response_->reset();
			start();

		} else {
			// Initiate graceful connection closure.
			asio::error_code ignored_ec;
			socket_.shutdown ( asio::ip::tcp::socket::shutdown_both, ignored_ec );
		}

	} else if ( e != asio::error::eof ) {
		std::cout << "handle write error: " << e.category().name() << ':' << e.value() << std::endl;
	}
}
}//namespace http
