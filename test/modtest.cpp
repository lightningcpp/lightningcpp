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
#include <tuple>
#include <vector>

#include "../include/http/mod/http.h"
#include "../include/http/mod/match.h"
#include "../include/http/mod/mod.h"

#include <gtest/gtest.h>

namespace http {
namespace mod {

TEST ( ModTest, TestExecuteWithoutAttributes ) {

    Mod< Match<>, Http > _mod ( Match<> ( "/foo" ), Http() );

    Request _request ( "/foo" );
    Response _response;
    _mod.execute ( _request, _response );

    EXPECT_EQ ( http::http_status::OK, _response.status() );
    EXPECT_EQ ( 3U, _response.parameter_size() );
}

TEST ( ModTest, TestExecuteWithAttributes ) {

    Mod< Match<std::string, int>, Http > _mod ( Match< std::string, int > ( "/(\\w+)/(\\d+)", "name", "user-id" ), Http() );

    Request _request ( "/Alice/42" );
    Response _response;
    _mod.execute ( _request, _response );

    EXPECT_EQ ( http::http_status::OK, _response.status() );
    EXPECT_EQ ( 3U, _response.parameter_size() );
    EXPECT_EQ ( "Alice", _request.attribute ( "name" ) );
    EXPECT_EQ ( "42", _request.attribute ( "user-id" ) );
}
}//namespace mod
}//namespace http
