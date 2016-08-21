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
#ifndef HTTPSERVERCONNECTION_H
#define HTTPSERVERCONNECTION_H

#include <functional>
#include <memory>

#include <asio.hpp>

#include "httprequest.h"
#include "httpresponse.h"
#include "httpsession.h"

namespace http {

class HttpServerConnection  : public std::enable_shared_from_this<HttpServerConnection> {
public:
	HttpServerConnection ( std::shared_ptr< HttpSession > session, asio::io_service& io_service );

	HttpServerConnection ( const HttpServerConnection& ) = delete;
	HttpServerConnection ( HttpServerConnection&& ) = delete;
	HttpServerConnection& operator= ( const HttpServerConnection& ) = delete;
	HttpServerConnection& operator= ( HttpServerConnection&& ) = delete;
	~HttpServerConnection() {}

	/** Get the socket associated with the connection. */
	asio::ip::tcp::socket& socket();
	void start();

private:
	void handle_read ( const asio::error_code& e, std::size_t bytes_transferred );
	void handle_write ( const asio::error_code& e );

	std::shared_ptr< HttpSession > session_;

	/** Strand to ensure the connection's handlers are not called concurrently. */
	asio::io_service::strand strand_;
	/** Socket for the connection. */
	asio::ip::tcp::socket socket_;
	/** Buffer for data. */
	buffer_t buffer_;

	server_delegate_t delegate_;

	std::shared_ptr< HttpResponse > response_;
};
typedef std::shared_ptr< HttpServerConnection > server_connection_ptr;
}//namespace http
#endif // HTTPSERVERCONNECTION_H
