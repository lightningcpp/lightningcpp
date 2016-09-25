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

#include "../include/http/mod/mstch.h"

#include <gtest/gtest.h>

namespace http {
namespace mod {

TEST ( ModMstchTest, TestCreateMstchMap ) {

    Mstch mstch( "{{mustache}} templating", bustache::object{{"mustache", "bustache"}} );
    Request _request( "/foo/bar" );
    Response _response;

    EXPECT_EQ( http::http_status::OK, mstch.execute( _request, _response ) );
    EXPECT_EQ( "bustache templating",  _response.str() );
}
TEST ( ModMstchTest, TestCreateMstchList ) {

    Mstch mstch( "{{header}} {{#items}}{{name}} {{/items}}", bustache::object{
        {"header", "Colors"},
        {"items",
        bustache::array {
                bustache::object { {"name", "red"} },
                bustache::object { {"name", "green"} },
                bustache::object { {"name", "blue"} }
        } } }
    );

    Request _request( "/foo/bar" );
    Response _response;

    EXPECT_EQ( http::http_status::OK, mstch.execute( _request, _response ) );
    EXPECT_EQ( "Colors red green blue ",  _response.str() );
}
TEST ( ModMstchTest, TestCreateMstchLiteralXml ) {

    const std::string s1 = R"xml(<?xml version="1.0"?>
</rainbow>
<title>{{header}}</title>
<colors>
{{#items}}
    <color>{{name}}</color>
{{/items}}
</colors>
</rainbow>)xml";

    const std::string r1 = R"xml(<?xml version="1.0"?>
</rainbow>
<title>Colors</title>
<colors>
    <color>red</color>
    <color>green</color>
    <color>blue</color>
</colors>
</rainbow>)xml";

Mstch mstch( s1, bustache::object{
        {"header", "Colors"},
        {"items",
        bustache::array {
                bustache::object { {"name", "red"} },
                bustache::object { {"name", "green"} },
                bustache::object { {"name", "blue"} }
        } } }
    );
    Request _request( "/foo/bar" );
    Response _response;

    EXPECT_EQ( http::http_status::OK, mstch.execute( _request, _response ) );
    EXPECT_EQ( r1,  _response.str() );
}
}//namespace mod
}//namespace http
