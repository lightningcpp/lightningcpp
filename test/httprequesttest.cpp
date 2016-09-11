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

#include <string>

#include "../src/httpconfig.h"
#include "../src/request.h"

#include <gtest/gtest.h>

namespace http {

TEST ( HttpRequestTest, ITestParameter ) {
	Request _request;

	_request.parameter ( http::header::CONNECTION, "close" );
	_request.parameter ( http::header::HOST, "localhost" );
	_request.parameter ( "SOAPACTION", "\"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"" );

	EXPECT_TRUE ( _request.contains_parameter ( http::header::CONNECTION ) );
	EXPECT_TRUE ( _request.contains_parameter ( "HOST" ) );
	EXPECT_TRUE ( _request.contains_parameter ( "SOAPACTION" ) );
	EXPECT_FALSE ( _request.contains_parameter ( "BLA" ) );

	EXPECT_EQ ( _request.parameter ( http::header::CONNECTION ), "close" );
	EXPECT_EQ ( _request.parameter ( http::header::HOST ), "localhost" );
	EXPECT_EQ ( _request.parameter ( "SoapAction" ), "\"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"" );
	EXPECT_FALSE ( _request.contains_parameter ( "BLA" ) );
}

TEST ( HttpRequestTest, HeadersToArray ) {
	std::stringstream _ss;
	_ss << "GET /foo/bar HTTP/1.1\r\n" <<
		"Connection: close\r\n" <<
		"Host: localhost\r\n" <<
		"SOAPACTION: \"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"\r\n" <<
		"\r\n";

	Request _request ( "/foo/bar" );

	_request.parameter ( http::header::CONNECTION, "close" );
	_request.parameter ( http::header::HOST, "localhost" );
	_request.parameter ( "SOAPACTION", "\"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"" );

	buffer_t _array;
	size_t _size = _request.header ( _array );
	EXPECT_EQ ( 131U, _size );
	std::string _result ( _array.data(), 0, _size );
	EXPECT_EQ ( static_cast< size_t > ( _ss.tellp() ), _result.size() );
	EXPECT_EQ ( _ss.str(), _result );
}
}//namespace http
