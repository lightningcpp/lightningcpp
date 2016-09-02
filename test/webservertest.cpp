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

void delegate_args_function ( HttpRequest & request, HttpResponse&, std::string name, int id ) {
    test_callback->callback ( request.uri(), name, id );
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

    EXPECT_CALL ( *test_callback, callback ( "/static" ) ).Times ( 1 );
    EXPECT_CALL ( *test_callback, callback ( "/static/bob/123", "bob", 123 ) ).Times ( 1 );
    EXPECT_CALL ( *test_callback, callback ( "/cls" ) ).Times ( 1 );
    EXPECT_CALL ( *test_callback, callback ( "/cls/john/123", "john", 123 ) ).Times ( 1 );
    EXPECT_CALL ( *test_callback, callback ( "/lambda" ) ).Times ( 1 );
    EXPECT_CALL ( *test_callback, callback ( "/lambda/alice/234", "alice", 234 ) ).Times ( 1 );

	WebServer< HttpServer > server ( "127.0.0.1", 8080 );
    MethodClass cls;
    server.bind ( "/static",  delegate_function );
    server.bind ( "/cls", &MethodClass::delegate_function, &cls );

    server.bind< DefaultParameter, std::string, int > ( "/static/(\\w+)/(\\d+)", delegate_args_function, _1, _2, _3, _4 );
    server.bind ( "/lambda", [] ( HttpRequest & request, HttpResponse& ) {
        test_callback->callback ( request.uri() );
    } );
    server.bind< DefaultParameter, std::string, int >( "/lambda/(\\w+)/(\\d+)", [] ( HttpRequest & request, HttpResponse&, std::string name, int id ) {
        test_callback->callback ( request.uri(), name, id );
    } );

    server.bind< DefaultParameter, std::string, int > ( "/static/(\\w+)/(\\d+)", delegate_args_function, _1, _2, _3, _4 );
    server.bind< DefaultParameter, std::string, int > ( "/cls/(\\w+)/(\\d+)", &MethodClass::delegate_function_user, &cls, _1, _2, _3, _4 );

	HttpResponse response;
    HttpRequest request0 ( "/static" );
    server.execute ( request0, response );

    HttpRequest request1 ( "/static/bob/123" );
    server.execute ( request1, response );

    HttpRequest request2 ( "/cls" );
    server.execute ( request2, response );

    HttpRequest request3 ( "/cls/john/123" );
    server.execute ( request3, response );

    HttpRequest request4 ( "/lambda" );
    server.execute ( request4, response );

    HttpRequest request5 ( "/lambda/alice/234" );
    server.execute ( request5, response );

	delete test_callback;
}
TEST ( WebServerTest, EqualsMatchAnyTest ) {
	test_callback = new MockTestCallback();
	EXPECT_CALL ( *test_callback, callback ( "/foo/bar" ) ).Times ( 1 );

	WebServer<DummyServer> server ( "127.0.0.1", 8080 );
    server.bind< DefaultParameter > ( "*",  delegate_function );

	HttpRequest request ( "/foo/bar" );
	HttpResponse response;
	server.execute ( request, response );

	delete test_callback;
}
TEST ( WebServerTest, HttpServerTest ) {

    WebServer< HttpServer > server ( "127.0.0.1", 8080 );
    server.bind< EmptyParameter, std::string, int >( "/foo/lambda/(\\w+)/(\\d+)", [] ( HttpRequest&, HttpResponse & response, std::string, int ) {
        response.status ( http::http_status::OK );
        response << "abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL MNO PQRS TUV";

        response.status ( http::http_status::OK );
        response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime::TEXT ) );
        response.parameter ( header::CONTENT_LENGTH, std::to_string ( 0 ) );
    } );

    HttpRequest request ( "/foo/lambda/alice/123" );
    HttpResponse response;
    server.execute ( request, response );

    EXPECT_EQ( 2U, response.parameter_size() );
}
}//namespace http
