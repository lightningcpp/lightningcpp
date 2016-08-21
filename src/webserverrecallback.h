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
#ifndef WEBSERVERRECALLBACK_H
#define WEBSERVERRECALLBACK_H

#include "re2/re2.h"

namespace http {

template< class HeaderParameter, class ... Args >
class WebServerReCallBack : public std::enable_shared_from_this< WebServerReCallBack< HeaderParameter, Args... > > {
public:
	WebServerReCallBack () {}
	WebServerReCallBack ( const WebServerReCallBack& ) = delete;
	WebServerReCallBack ( WebServerReCallBack&& ) = delete;
	WebServerReCallBack& operator= ( const WebServerReCallBack& ) = delete;
	WebServerReCallBack& operator= ( WebServerReCallBack&& ) = delete;
	~WebServerReCallBack() {}

	std::function< bool ( HttpRequest & request, HttpResponse & response ) > bind (
		const std::string & uri, std::function< void ( HttpRequest&, HttpResponse&, Args... ) > && delegate ) {
		_uri = uri;
		_delegate = std::move ( delegate );
		return std::bind ( &WebServerReCallBack< HeaderParameter, Args ...>::execute, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2 );
	};
private:
	bool execute ( HttpRequest & request, HttpResponse & response ) {
		std::tuple< Args... > args;

		if ( bind_with_tuple ( request, args, std::index_sequence_for<Args...>() ) ) {
			callback_with_tuple ( request, response, args, std::index_sequence_for<Args...>() );
			HeaderParameter::execute ( request, response );
			return true;

		} else { return false; }
	}
	template<std::size_t... Is>
	inline bool bind_with_tuple ( HttpRequest & request, std::tuple<Args...>& tuple, std::index_sequence<Is...> ) {
		return ( RE2::FullMatch ( request.uri(), _uri, &std::get<Is> ( tuple )... ) );
	}
	template<std::size_t... Is>
	inline void callback_with_tuple ( HttpRequest & request, HttpResponse & response, std::tuple<Args...>& tuple, std::index_sequence<Is...> ) {
		_delegate ( request, response, std::get<Is> ( tuple )... );
	}

	std::string _uri;
	std::function< void ( HttpRequest&, HttpResponse&, Args... ) > _delegate;
};
}//namespace http
#endif // WEBSERVERRECALLBACK_H
