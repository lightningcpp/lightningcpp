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

#include "../include/http/request.h"
#include "../include/http/response.h"

#include "../include/http/mod/exec.h"
#include "../include/http/mod/method.h"
#include "../include/http/utils/base64.h"

#include <gtest/gtest.h>

namespace http {
namespace mod {

TEST ( ModMethodTest, TestCreate ) {

    using namespace http::mod;
    using namespace http::mod::method;
    Method method( GET( Exec( [] (Request&, Response&) {
        return http::http_status::OK;
    })));
    Request _request;
    Response _response;
    _request.method( http::method::GET );
    EXPECT_EQ( http::http_status::OK, method.execute ( _request, _response ) );
    _request.method( "POST" );
    EXPECT_EQ( http::http_status::METHOD_NOT_ALLOWED, method.execute ( _request, _response ) );
}

TEST ( ModMethodTest, TestCreateMulti ) {

    using namespace http::mod;
    using namespace http::mod::method;
    Method method(
        GET( Exec( [] (Request&, Response&) {
            return http::http_status::OK;
        })),
        POST( Exec( [] (Request&, Response&) {
            return http::http_status::OK;
        }))
    );
    Request _request;
    Response _response;
    _request.method( "GET" );
    EXPECT_EQ( http::http_status::OK, method.execute ( _request, _response ) );
    _request.method( "POST" );
    EXPECT_EQ( http::http_status::OK, method.execute ( _request, _response ) );
    _request.method( "PUT" );
    http_status _s = method.execute ( _request, _response );
    EXPECT_EQ( http::http_status::METHOD_NOT_ALLOWED, _s );
}
TEST ( ModMethodTest, TestCreateMultiReverse ) {

    using namespace http::mod;
    using namespace http::mod::method;
    Method method(
        POST( Exec( [] (Request&, Response&) {
            return http::http_status::OK;
        })),
        GET( Exec( [] (Request&, Response&) {
            return http::http_status::OK;
        }))
    );
    Request _request;
    Response _response;
    _request.method( "GET" );
    EXPECT_EQ( http::http_status::OK, method.execute ( _request, _response ) );
    _request.method( "POST" );
    EXPECT_EQ( http::http_status::OK, method.execute ( _request, _response ) );
    _request.method( "PUT" );
    http_status _s = method.execute ( _request, _response );
    EXPECT_EQ( http::http_status::METHOD_NOT_ALLOWED, _s );
}
TEST ( ModMethodTest, TestCreateMultiString ) {

    using namespace http::mod;
    using namespace http::mod::method;
    Method method(
        POST( Exec( [] (Request&, Response&) {
            return http::http_status::OK;
        })),
        GET( Exec( [] (Request&, Response&) {
            return http::http_status::OK;
        })),
        S( "SUBSCRIBE", Exec( [] (Request&, Response&) {
            return http::http_status::OK;
        }))

    );
    Request _request;
    Response _response;
    _request.method( "GET" );
    EXPECT_EQ( http::http_status::OK, method.execute ( _request, _response ) );
    _request.method( "POST" );
    EXPECT_EQ( http::http_status::OK, method.execute ( _request, _response ) );
    _request.method( "SUBSCRIBE" );
    EXPECT_EQ( http::http_status::OK, method.execute ( _request, _response ) );
    _request.method( "PUT" );
    http_status _s = method.execute ( _request, _response );
    EXPECT_EQ( http::http_status::METHOD_NOT_ALLOWED, _s );
}
}//namespace mod
}//namespace http
