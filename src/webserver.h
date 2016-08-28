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
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <functional>

#include "httpconfig.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "errordelegate.h"
#include "webservercallback.h"
#include "webserverrecallback.h"

#include "re2/re2.h"

namespace http {

template< typename T >
class WebServer : T {
public:
	WebServer ( const std::string & ip, const int & port ) :
		T ( ip, port, http_delegate_t ( std::bind ( &WebServer::execute, this, std::placeholders::_1, std::placeholders::_2 ) ) ) {}
	WebServer ( const WebServer& ) = delete;
	WebServer ( WebServer&& ) = delete;
	WebServer& operator= ( const WebServer& ) = delete;
	WebServer& operator= ( WebServer&& ) = delete;
	virtual ~WebServer() {}


    template< class ... Args >
    void bind ( const std::string & uri, std::function< void ( HttpRequest&, HttpResponse&, Args... ) > && delegate ) {
        std::shared_ptr< WebServerReCallBack< DefaultParameter, Args... > > cb = std::make_shared< WebServerReCallBack< DefaultParameter, Args ... > >();
		_delegates.push_back ( cb->bind ( uri, std::move ( delegate ) ) );
	}

    void bind ( const std::string & uri, http_delegate_t && delegate ) {
        std::shared_ptr< WebServerCallBack< DefaultParameter > > cb = std::make_shared< WebServerCallBack< DefaultParameter > >();
		_delegates.push_back ( cb->bind ( uri, std::move ( delegate ) ) );
	}
	/**
	 * @brief bind error delegate
	 * @param status the http status for this delegate
	 * @param delegate the delegate
	 */
	void bind_error_delegate ( http_status status, http_delegate_t && delegate ) {
		_error_delegates[ status ] = std::move ( delegate );
	}

    void execute ( HttpRequest & request, HttpResponse & response ) {
		try {
			for ( auto delegate : _delegates ) {
				if ( delegate ( request, response ) ) {
					return;
				}
			}

			response.status ( http_status::NOT_FOUND );
			handle_error ( request, response );

		} catch ( http_status & status ) {
			response.status ( status );

			if ( request.method() == method::HEAD || //error without body
					status == http_status::NO_CONTENT ||
					status == http_status::NOT_MODIFIED ||
					( static_cast< int > ( status ) >= 100 && static_cast< int > ( status ) <= 199 ) ) {

				//TODO clean buffer response.
				return;

			} else { //error with body
				handle_error ( request, response );
			}

		} catch ( ... ) {
			response.status ( http_status::INTERNAL_SERVER_ERROR );
			handle_error ( request, response );
		}
	}

private:
    std::list< webserver_delegate_t > _delegates;
	std::map<http_status, http_delegate_t > _error_delegates {
		{ http_status::BAD_GATEWAY, ErrorDelegate<>::bind ( http::response::BAD_GATEWAY ) },
		{ http_status::BAD_REQUEST, ErrorDelegate<>::bind ( http::response::BAD_REQUEST ) },
		{ http_status::CREATED, ErrorDelegate<>::bind ( http::response::CREATED ) },
		{ http_status::FORBIDDEN, ErrorDelegate<>::bind ( http::response::FORBIDDEN ) },
		{ http_status::INTERNAL_SERVER_ERROR, ErrorDelegate<>::bind ( http::response::INTERNAL_SERVER_ERROR ) },
		{ http_status::MOVED_PERMANENTLY, ErrorDelegate<>::bind ( http::response::MOVED_PERMANENTLY ) },
		{ http_status::MULTIPLE_CHOICES, ErrorDelegate<>::bind ( http::response::MULTIPLE_CHOICES ) },
		{ http_status::NOT_FOUND, ErrorDelegate<>::bind ( http::response::NOT_FOUND ) },
		{ http_status::NOT_IMPLEMENTED, ErrorDelegate<>::bind ( http::response::NOT_IMPLEMENTED ) },
		{ http_status::NOT_MODIFIED, ErrorDelegate<>::bind ( http::response::NOT_MODIFIED ) },
		{ http_status::NO_CONTENT, ErrorDelegate<>::bind ( http::response::NO_CONTENT ) },
		{ http_status::SERVICE_UNAVAILABLE, ErrorDelegate<>::bind ( http::response::SERVICE_UNAVAILABLE ) },
		{ http_status::UNAUTHORIZED, ErrorDelegate<>::bind ( http::response::UNAUTHORIZED ) }
	};
	/**
	 * @brief error handler
	 * @param status
	 */
    template< class Request, class Response >
    void handle_error ( Request & request, Response & response ) {
		http_status status = response.status();

		try {
			if ( _error_delegates.find ( status ) != _error_delegates.end() ) {
				request.uri ( make_error_uri ( status ) );
				_error_delegates[ status ] ( request, response );
				response.status ( status );

			} else {
				request.uri ( make_error_uri ( http_status::NOT_FOUND ) );
				_error_delegates[ status ] ( request, response );
				response.status ( http_status::NOT_FOUND );
			}

		} catch ( ... ) {
            //TODO log
			request.uri ( make_error_uri ( http_status::INTERNAL_SERVER_ERROR ) );
			_error_delegates[ status ] ( request, response );
			response.status ( http_status::INTERNAL_SERVER_ERROR );
		}
	}
	inline std::string make_error_uri ( http_status status ) {
        std::stringstream _buf;
        _buf << "/" << static_cast< int > ( status ) << ".html";
        return _buf.str();
	}
};

}//namespace http
#endif // WEBSERVER_H
