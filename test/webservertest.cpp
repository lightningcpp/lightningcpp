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

#include <future>
#include <string>
#include <memory>

#include "../src/webserver.h"
#include "../src/httpserver.h"

#include "testutils.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace http {

class TestCallBack {
public:
	virtual ~TestCallBack() {}
	virtual void callback ( std::string uri ) = 0;
	virtual void callback ( std::string uri, std::string name, int id ) = 0;
};

class MockTestCallback : public TestCallBack {
public:
	MOCK_METHOD1 ( callback, void ( std::string uri ) );
	MOCK_METHOD3 ( callback, void ( std::string uri, std::string name, int id ) );
};

MockTestCallback * test_callback;

void delegate_function ( HttpRequest & request, HttpResponse& ) {
	test_callback->callback ( request.uri() );
}

class MethodClass {
public:
	void delegate_function ( HttpRequest & request, HttpResponse& ) {
		test_callback->callback ( request.uri() );
	}
	void delegate_function_user ( HttpRequest & request, HttpResponse&, std::string name, int id ) {
		test_callback->callback ( request.uri(), name, id );
	}
};

class DummyServer {
public:
	DummyServer ( const std::string&, const int&, std::function< void ( HttpRequest&, HttpResponse& ) >&& ) {}
};

TEST ( WebServerTest, DelegateTest ) {
	test_callback = new MockTestCallback();

	EXPECT_CALL ( *test_callback, callback ( "/foo/bar" ) ).Times ( 1 );
	EXPECT_CALL ( *test_callback, callback ( "/foo/bar/cls" ) ).Times ( 1 );
	EXPECT_CALL ( *test_callback, callback ( "/foo/bar/john/123", "john", 123 ) ).Times ( 1 );
	EXPECT_CALL ( *test_callback, callback ( "/foo/lambda/alice/234", "alice", 234 ) ).Times ( 1 );

	WebServer< HttpServer > server ( "127.0.0.1", 8080 );
	MethodClass cls;
	server.bind ( "/foo/bar",  std::function< void ( HttpRequest&, HttpResponse& ) > ( delegate_function ) );
	server.bind ( "/foo/bar/cls", std::function< void ( HttpRequest&, HttpResponse& ) > ( std::bind ( &MethodClass::delegate_function, &cls, std::placeholders::_1, std::placeholders::_2 ) ) );
	server.bind ( "/foo/bar/(\\w+)/(\\d+)", std::function< void ( HttpRequest&, HttpResponse&, std::string, int ) > ( std::bind ( &MethodClass::delegate_function_user, &cls, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 ) ) );
	server.bind ( "/foo/lambda/(\\w+)/(\\d+)", std::function< void ( HttpRequest&, HttpResponse&, std::string, int ) > ( [] ( HttpRequest & request, HttpResponse&, std::string name, int id ) {
		test_callback->callback ( request.uri(), name, id );
	} ) );

	HttpRequest request ( "/foo/bar" );
	HttpResponse response;
	server.execute ( request, response );

	HttpRequest request2 ( "/foo/bar/cls" );
	server.execute ( request2, response );

	HttpRequest request3 ( "/foo/bar/john/123" );
	server.execute ( request3, response );

	HttpRequest request4 ( "/foo/lambda/alice/234" );
	server.execute ( request4, response );

	delete test_callback;
}
TEST ( WebServerTest, EqualsMatchAnyTest ) {
	test_callback = new MockTestCallback();
	EXPECT_CALL ( *test_callback, callback ( "/foo/bar" ) ).Times ( 1 );

	WebServer<DummyServer> server ( "127.0.0.1", 8080 );
	server.bind ( "*",  std::function< void ( HttpRequest&, HttpResponse& ) > ( delegate_function ) );

	HttpRequest request ( "/foo/bar" );
	HttpResponse response;
	server.execute ( request, response );

	delete test_callback;
}
TEST ( WebServerTest, HttpServerTest ) {

	WebServer< HttpServer > server ( "127.0.0.1", 8080 );
	server.bind< EmptyParameter > ( "/foo/lambda/(\\w+)/(\\d+)", std::function< void ( HttpRequest&, HttpResponse&, std::string, int ) > ( [] ( HttpRequest&, HttpResponse & response, std::string, int ) {
		response.status ( http::http_status::OK );
		response << "abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO PQRS TUV";

		response.status ( http::http_status::OK );
		response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime::TEXT ) );
		response.parameter ( header::CONTENT_LENGTH, std::to_string ( 0 ) );
	} ) );


	//test data
	asio::streambuf response;
	asio::streambuf request;
	std::ostream request_stream ( &request );
	request_stream << "GET /foo/lambda/alice/123 HTTP/1.0\r\n";
	request_stream << "Host: localhost\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";

	std::stringstream _response_expectedstreambuf;
	_response_expectedstreambuf << "HTTP/1.1 200 OK\r\n" <<
								"Content-Length: 0\r\n" <<
								"Content-Type: text/plain\r\n\r\n" <<
								"abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO PQRS TUV";

	//send a request
	client_header ( "127.0.0.1", "8080", std::vector< asio::streambuf* > ( { &request } ), response );
	std::istream _response_stream ( &response );
	EXPECT_TRUE ( compare_streams ( _response_expectedstreambuf, _response_stream ) );
}
}//namespace http
