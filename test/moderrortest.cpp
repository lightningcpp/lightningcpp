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

#include "../src/mod/error.h"

#include <gtest/gtest.h>

namespace http {
namespace mod {

TEST ( ModErrorTest, TestExecute ) {
    http::mod::Error error;
    Request _request( "/foo" );
    Response _response;
    _response.status( http::http_status::NOT_FOUND );
    http::http_status _status = error.execute ( _request, _response );
    EXPECT_EQ( http::http_status::NOT_FOUND, _status );

    std::stringstream _sbuf;
    buffer_t _buf;
    while( _response.tellp() > _response.tellg() ) {
        size_t _size = _response.read( _buf );
        _sbuf << std::string( _buf.data(), 0, _size );
    }
    EXPECT_EQ( "<html><head><title>Not Found</title></head><body><h1>404 Not Found</h1></body></html>", _sbuf.str() );
}
}//namespace mod
}//namespace http
