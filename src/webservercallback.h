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
#ifndef WEBSERVERCALLBACK_H
#define WEBSERVERCALLBACK_H

#include "re2/re2.h"

#include "httpconfig.h"

namespace http {

template< class HeaderParameter >
class WebServerCallBack : public std::enable_shared_from_this< WebServerCallBack< HeaderParameter > > {
public:
	WebServerCallBack () {}
	WebServerCallBack ( const WebServerCallBack& ) = delete;
	WebServerCallBack ( WebServerCallBack&& ) = delete;
	WebServerCallBack& operator= ( const WebServerCallBack& ) = delete;
	WebServerCallBack& operator= ( WebServerCallBack&& ) = delete;
	~WebServerCallBack() {}

    webserver_delegate_t bind (  const std::string & uri, http_delegate_t && delegate ) {
		_uri = uri;
		_delegate = std::move ( delegate );
		return std::bind ( &WebServerCallBack::execute, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2 );
	};

private:
    bool execute ( HttpRequest & request, HttpResponse & response ) {
        if ( request.uri() == _uri || _uri == "*" || RE2::FullMatch ( request.uri(), _uri ) ) {
            _delegate ( request, response );
            HeaderParameter::execute ( request, response );
            return true;

		} else { return false; }
	}
	std::string _uri;
	http_delegate_t _delegate;
};
}//namespace http
#endif // WEBSERVERCALLBACK_H
