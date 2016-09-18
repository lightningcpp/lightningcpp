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

#include "../src/mod/mstch.h"

#include <gtest/gtest.h>

namespace http {
namespace mod {

TEST ( ModMstchTest, TestCreateMstchMap ) {

//    Mstch mstch( "{{#names}}Hi {{name}}!\n{{/names}}", mstch::map {
//                     {"names", mstch::array{
//                       mstch::map{{"name", std::string{"Chris"}}},
//                       mstch::map{{"name", std::string{"Mark"}}},
//                       mstch::map{{"name", std::string{"Scott"}}},
//                     } } } );

//    Request _request( "/foo/bar" );
//    Response _response;

//    EXPECT_EQ( http::http_status::OK, mstch.execute( _request, _response ) );
//    EXPECT_EQ( "",  _response.str() );
}
}//namespace mod
}//namespace http
