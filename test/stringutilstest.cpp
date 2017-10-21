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

#include "../include/http/utils/stringutils.h"

#include <gtest/gtest.h>

namespace http {
namespace utils {

TEST ( StringUtilsTest, is_numeric ) {
    EXPECT_TRUE ( is_numeric( "0" ) );
    EXPECT_TRUE ( is_numeric( "123" ) );
    EXPECT_FALSE ( is_numeric( "abc" ) );
    EXPECT_FALSE ( is_numeric( "abc123" ) );
    EXPECT_FALSE ( is_numeric( "123abc" ) );
}


TEST ( StringUtilsTest, TrimString ) {
	std::string v = "  VALUE1 ";
	EXPECT_EQ ( "VALUE1", trim ( v ) );

	v = "VALUE2";
	EXPECT_EQ ( "VALUE2", trim ( v ) );

	v = "   ";
	EXPECT_EQ ( "", trim ( v ) );

	v = " VALUE 3";
	EXPECT_EQ ( "VALUE 3", trim ( v ) );

	v = "VALUE 4   ";
	EXPECT_EQ ( "VALUE 4", trim ( v ) );

	v = "VALUE 4   ";
	std::string & _res = trim ( v );
	EXPECT_EQ ( &v, &_res );
}

TEST ( StringUtilsTest, ParseCSV ) {
	EXPECT_EQ ( std::vector< std::string > ( { "a", "b", "c" } ), parse_csv ( "a,b,c" )  );
	EXPECT_EQ ( std::vector< std::string > ( { "foo protocol", "bar protocol", "extra protocol" } ), parse_csv ( "foo protocol, bar protocol, extra protocol" )  );
}

TEST ( StringUtilsTest, TestParseDateFromStringRFC822 ) {
	EXPECT_EQ ( 784115377U, parse_time_string ( "Sun, 06 Nov 1994 09:49:37 GMT" ) );
}
TEST ( StringUtilsTest, TestParseDateFromStringRFC850 ) {
	EXPECT_EQ ( 784115377U, parse_time_string ( "Sunday, 06-Nov-94 09:49:37 GMT" ) );
}
TEST ( StringUtilsTest, TestParseDateFromStringCString ) {
	EXPECT_EQ ( 784115377U, parse_time_string ( "Sun Nov 06 09:49:37 1994" ) ); //TODO shall also work with "Sun Nov  6 08:49:37 1994"
}

TEST ( StringUtilsTest, TestParseDate1 ) {
	time_t time_ = static_cast< time_t > ( 1469787808U );
	EXPECT_EQ ( "Fri, 29 Jul 2016 10:23:28 GMT", time_to_string ( &time_ ) );
}
TEST ( StringUtilsTest, TestParseDate2 ) {
	time_t time_ = static_cast< time_t > ( 784111777U );
	EXPECT_EQ ( "Sun, 06 Nov 1994 08:49:37 GMT", time_to_string ( &time_ ) );
}

TEST ( StringUtilsTest, ParseRange ) {
	//Range: bytes=0-999
	std::tuple<int, int> range1 = http::utils::parseRange ( "bytes=0-999" );
	EXPECT_EQ ( 0, std::get<0> ( range1 ) );
	EXPECT_EQ ( 999, std::get<1> ( range1 ) );

	//Range: bytes=0-
	std::tuple<int, int> range2 = http::utils::parseRange ( "bytes=0-" );
	EXPECT_EQ ( 0, std::get<0> ( range2 ) );
	EXPECT_EQ ( -1, std::get<1> ( range2 ) );

	//Range: bytes=999-
	std::tuple<int, int> range3 = http::utils::parseRange ( "bytes=999-" );
	EXPECT_EQ ( 999, std::get<0> ( range3 ) );
	EXPECT_EQ ( -1, std::get<1> ( range3 ) );
}

TEST ( StringUtilsTest, UrlParser ) {

    UrlParser p { "http://www.google.com/index.html"};
    EXPECT_EQ ( "www.google.com", p.host() );
    EXPECT_EQ ( "/index.html", p.path() );
    EXPECT_EQ ( "http", p.proto() );
    EXPECT_FALSE ( p.secure() );
}
TEST ( StringUtilsTest, UrlParserSecure ) {

    UrlParser p { "https://www.google.com/index.html"};
    EXPECT_EQ ( "www.google.com", p.host() );
    EXPECT_EQ ( "/index.html", p.path() );
    EXPECT_EQ ( "https", p.proto() );
    EXPECT_TRUE ( p.secure() );
}
TEST ( StringUtilsTest, UrlSecureNoPath ) {

    UrlParser p { "https://www.google.com"};
    EXPECT_EQ ( "www.google.com", p.host() );
    EXPECT_EQ ( "/", p.path() );
    EXPECT_EQ ( "https", p.proto() );
    EXPECT_TRUE ( p.secure() );
}
TEST ( StringUtilsTest, UrlParserWithPort ) {

    UrlParser p { "http://www.google.com:80/index.html"};
    EXPECT_EQ ( "www.google.com", p.host() );
    EXPECT_EQ ( "/index.html", p.path() );
    EXPECT_EQ ( "80", p.proto() );
    EXPECT_FALSE ( p.secure() );
}
TEST ( StringUtilsTest, UrlParserWithPortNoPath ) {

    UrlParser p { "http://www.google.com:80"};
    EXPECT_EQ ( "www.google.com", p.host() );
    EXPECT_EQ ( "/", p.path() );
    EXPECT_EQ ( "80", p.proto() );
    EXPECT_FALSE ( p.secure() );
}
TEST ( StringUtilsTest, UrlParserAmazon ) {

    UrlParser p { "https://images-na.ssl-images-amazon.com/images/I/51QRG0GOwYL.jpg"};
    EXPECT_EQ ( "images-na.ssl-images-amazon.com", p.host() );
    EXPECT_EQ ( "/images/I/51QRG0GOwYL.jpg", p.path() );
    EXPECT_EQ ( "https", p.proto() );
    EXPECT_TRUE ( p.secure() );
}
TEST ( StringUtilsTest, UrlParserDash ) {

    UrlParser p { "https://www.googleapis.com/books/v1/volumes?q=isbn:[Focal Press] The Art of Digital Audio&key=AIzaSy" };
    EXPECT_EQ ( "www.googleapis.com", p.host() );
    EXPECT_EQ ( "/books/v1/volumes?q=isbn:[Focal Press] The Art of Digital Audio&key=AIzaSy", p.path() );
    EXPECT_EQ ( "https", p.proto() );
    EXPECT_TRUE ( p.secure() );
}
}//namespace utils
}//namspace http
