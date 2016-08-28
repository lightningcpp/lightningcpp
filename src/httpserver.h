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
#ifndef HTTPSERVER2_H
#define HTTPSERVER2_H

#include <asio.hpp>

#include <string>
#include <thread>

#include "httpconfig.h"
#include "httpserverconnection.h"
#include "httpsession.h"

#include <gtest/gtest_prod.h>

namespace http {
/**
 * @brief The HttpServer class
 *
 * \msc
 *  HttpServer, HttpSession,Request,Response,HttpConnection;
 *
 *  HttpServer->HttpSession [label="handle_accept", URL="\ref HttpSession::start()"];
 *  HttpSession=>HttpConnection [label="bind", URL="\ref HttpSession::parse_request ( buffer_t&, const size_t)", ID="1"];
 *  HttpSession->HttpConnection [label="start", URL="\ref HttpSession::start()", ID="1"];
 *  HttpConnection->HttpConnection [label="write", URL="\ref start", ID="2"];
 *  HttpConnection->HttpSession [label="parse: HttpSession::parse_request ( buffer_t&, const size_t)", URL="\ref HttpSession::parse_request ( buffer_t&, const size_t)", ID="2"];
 *  HttpSession=>HttpConnection [label="return: HttpConnection"
 *\endmsc

 *
 */
class HttpServer {
public:
	HttpServer ( const std::string & address, const int & port, http_delegate_t && request_handler ) :
        acceptor_ ( io_service_ ), request_handler_ ( std::move ( request_handler ) ) {

		// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
		asio::ip::tcp::resolver resolver ( io_service_ );
		asio::ip::tcp::resolver::query query ( address, std::to_string ( port ) );
		asio::ip::tcp::endpoint endpoint = *resolver.resolve ( query );
		acceptor_.open ( endpoint.protocol() );
		acceptor_.set_option ( asio::ip::tcp::acceptor::reuse_address ( true ) );
		acceptor_.bind ( endpoint );
		acceptor_.listen();
		start_accept();

		// Create a pool of threads to run all of the io_services.
		for ( std::size_t i = 0; i < HTTP_SERVER_THREAD_POOL_SIZE; ++i ) {
			std::shared_ptr<std::thread> thread ( new std::thread (
					std::bind ( static_cast<size_t ( asio::io_service::* ) () > ( &asio::io_service::run ), &io_service_ ) ) );
			threads_[i] = thread;
		}
	}
	HttpServer ( const HttpServer& ) = delete;
	HttpServer ( HttpServer&& ) = delete;
	HttpServer& operator= ( const HttpServer& ) = delete;
	HttpServer& operator= ( HttpServer&& ) = delete;
	~HttpServer() {
		io_service_.stop();

		//Wait for all threads in the pool to exit.
		for ( std::size_t i = 0; i < threads_.size(); ++i )
		{ threads_[i]->join(); }
	}

private:
	FRIEND_TEST ( HttpServerTest, TestCreateServer );

	/* Initiate an asynchronous accept operation. */
	void start_accept() {
        server_connection_.reset ( new HttpServerConnection( io_service_ ) );
        acceptor_.async_accept ( server_connection_->socket(),
								 std::bind ( &HttpServer::handle_accept, this, std::placeholders::_1 ) );
	}

	/* Handle completion of an asynchronous accept operation. */
	void handle_accept ( const asio::error_code& e ) {
		if ( !e ) {
            std::shared_ptr< HttpSession > _session = std::shared_ptr< HttpSession >( new HttpSession( std::move( server_connection_ ), request_handler_ ) );
            _session->start();
		}

		start_accept();
	}

	/* The io_service used to perform asynchronous operations. */
	asio::io_service io_service_;
	/* Acceptor used to listen for incoming connections. */
	asio::ip::tcp::acceptor acceptor_;
	/* The Thread pool. */
	std::array<std::shared_ptr<std::thread>, HTTP_SERVER_THREAD_POOL_SIZE > threads_;

    server_connection_ptr server_connection_;

	/** http request handler delegate */
	http_delegate_t request_handler_;
};
}//namespace http
#endif // HTTPSERVER2_H
