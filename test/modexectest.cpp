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

#include "../include/http/mod/exec.h"

#include <gtest/gtest.h>

namespace http {
namespace mod {

TEST ( ModExecTest, TestExecute ) {
    bool _executed = false;
    Exec x( [&_executed] ( Request&, Response& ) {
        _executed = true;
        return http::http_status::BAD_GATEWAY;
    } );

    Request _request( "/foo" );
    Response _response;
    EXPECT_EQ( http::http_status::BAD_GATEWAY, x.execute ( _request, _response ) );
    EXPECT_TRUE( _executed );
}
}//namespace mod
}//namespace http
