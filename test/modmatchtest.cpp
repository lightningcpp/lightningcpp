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

#include "../include/http/mod/match.h"

#include <gtest/gtest.h>

namespace http {
namespace mod {

TEST ( ModMatchTest, TestExecute ) {
    Match<> match( "/foo" );
    Request _request( "/foo" );
    Response _response;
    EXPECT_EQ( http::http_status::OK, match.execute ( _request, _response ) );
}
TEST ( ModMatchTest, TestNotFound ) {
    Match<> match( "/foo" );
    Request _request( "/bar" );
    Response _response;
    EXPECT_EQ( http::http_status::NOT_FOUND, match.execute( _request, _response ) );
}
TEST ( ModMatchTest, TestExecuteWithArgs ) {
    Match< std::string, int > match( "/foo/(\\w+)/(\\d+)", "name", "user-id" );
    Request _request( "/foo/alice/42" );
    Response _response;
    EXPECT_EQ( http::http_status::OK, match.execute( _request, _response ) );
    EXPECT_EQ( 2U, _request.attribute_names().size() );
    EXPECT_EQ( "alice", _request.attribute( "name" ) );
    EXPECT_EQ( "42", _request.attribute( "user-id" ) );
}
TEST ( ModMatchTest, TestExecuteWithArgsMissing ) {
    Match< std::string, int > match( "/foo/(\\w+)/(\\d+)", "name", "user-id" );
    Request _request( "/foo/" );
    Response _response;
    EXPECT_EQ( http::http_status::NOT_FOUND, match.execute( _request, _response ) );
    EXPECT_EQ( 0U, _request.attribute_names().size() );
}
TEST ( ModMatchTest, TestExecuteWithArgsOptional ) {
    Match< std::string, int > match( "/foo/(\\w+)/?(\\d+)", "name", "user-id" );
    Request _request( "/foo/bar/" );
    Response _response;
    EXPECT_EQ( http::http_status::NOT_FOUND, match.execute( _request, _response ) );
    EXPECT_EQ( 0U, _request.attribute_names().size() );
}
}//namespace mod
}//namespace http
