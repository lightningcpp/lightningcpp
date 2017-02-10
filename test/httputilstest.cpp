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

// #include "../src/httputils.h"

#include <gtest/gtest.h>

namespace http {


//TEST ( HttpUtilsTest, TestParseIp ) {

//    std::string ip1 = utils::parse_ip ( "http://127.0.0.1/index.html" );
//    EXPECT_EQ ( "127.0.0.1", ip1 );

//    std::string ip2 = utils::parse_ip ( "http://127.0.0.1:8080/index.html" );
//    EXPECT_EQ ( "127.0.0.1", ip2 );
//}
//TEST ( HttpUtilsTest, TestParsePort ) {

//    int port1 = utils::parse_port ( "http://127.0.0.1/index.html" );
//    EXPECT_EQ ( 0, port1 );

//    int port2 = utils::parse_port ( "http://127.0.0.1:8080/index.html" );
//    EXPECT_EQ ( 8080, port2 );
//}
//TEST ( HttpUtilsTest, TestParsePath ) {

//    std::string path1 = utils::parse_path ( "http://127.0.0.1/index.html" );
//    EXPECT_EQ ( "/index.html", path1 );

//    std::string path2 = utils::parse_path ( "http://127.0.0.1:8080/index.html" );
//    EXPECT_EQ ( "/index.html", path2 );

//    std::string path3 = utils::parse_path ( "http://127.0.0.1:8080" );
//    EXPECT_EQ ( "/", path3 );
//}


//TEST ( HttpUtilsTest, TestUrlEncode ) {
//    EXPECT_EQ ( "%22%2FAardvarks%20lurk%2C%20OK%3F%3D%22", http::utils::UrlEscape::urlEncode ( "\"/Aardvarks lurk, OK?=\"" ) );
//}
//TEST ( HttpUtilsTest, TestSha256Encode ) {
//    EXPECT_EQ ( "j7bZM0LXZ9eXeZruTqWm2DIvDYVUU3wxPPpp%2BiXxzQc%3D", http::utils::UrlEscape::urlEncode ( "j7bZM0LXZ9eXeZruTqWm2DIvDYVUU3wxPPpp+iXxzQc=" ) );
//}
}//namespace http
