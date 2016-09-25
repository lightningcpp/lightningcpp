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

#include "../include/http/server.h"
#include "../include/http/httpserver.h"
#include "../include/http/mod/match.h"
#include "../include/http/mod/http.h"

#include "testutils.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace http {

TEST ( WebserverTest, DelegateTestWithoutArgs ) {
    Server< HttpServer > _server( "localhost", "9000" );
    _server.bind( mod::Match<>( "/foo" ), mod::Http() );

    Request _request( "/foo" );
    Response _response;
    _server.request( _request, _response );
    EXPECT_EQ( http::http_status::OK, _response.status() );
    EXPECT_EQ( 2U, _response.parameter_size() );

    Request _request2 ( "/bar" );
    _server.request( _request2, _response );
    EXPECT_EQ( http::http_status::NOT_FOUND, _response.status() );
    EXPECT_EQ( 2U, _response.parameter_size() );
}
TEST ( WebserverTest, DelegateTestArgs ) {
    Server< HttpServer > _server( "localhost", "9000" );
    _server.bind( mod::Match< std::string, int >( "/(\\w+)/(\\d+)", "name", "user-id" ), mod::Http() );

    Request _request( "/alice/42" );
    Response _response;
    _server.request( _request, _response );
    EXPECT_EQ( http::http_status::OK, _response.status() );
    EXPECT_EQ( 2U, _response.parameter_size() );
    EXPECT_EQ( "alice", _request.attribute( "name" ) );
    EXPECT_EQ( "42", _request.attribute( "user-id" ) );

    Request _request2 ( "/bar" );
    _server.request( _request2, _response );
    EXPECT_EQ( http::http_status::NOT_FOUND, _response.status() );
    EXPECT_EQ( 2U, _response.parameter_size() );
}
TEST ( WebserverTest, DelegateTestMixed ) {
    Server< HttpServer > _server( "localhost", "9000" );
    _server.bind( mod::Match<>( "/foo" ), mod::Http() );
    _server.bind( mod::Match< std::string, int >( "/(\\w+)/(\\d+)", "name", "user-id" ), mod::Http() );

    Request _request( "/foo" );
    Response _response;
    _server.request( _request, _response );
    EXPECT_EQ( http::http_status::OK, _response.status() );
    EXPECT_EQ( 2U, _response.parameter_size() );

    Request _request2 ( "/alice/42" );
    _server.request( _request2, _response );
    EXPECT_EQ( http::http_status::OK, _response.status() );
    EXPECT_EQ( 2U, _response.parameter_size() );
    EXPECT_EQ( "alice", _request2.attribute( "name" ) );
    EXPECT_EQ( "42", _request2.attribute( "user-id" ) );

    Request _request3 ( "/bar" );
    _server.request( _request3, _response );
    EXPECT_EQ( http::http_status::NOT_FOUND, _response.status() );
    EXPECT_EQ( 2U, _response.parameter_size() );
}
}//namespace http
