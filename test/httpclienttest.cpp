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
#include <fstream>

#include "../include/http/httpclient.h"
#include "../include/http/httpserver.h"
#include "../include/http/server.h"

#include <gtest/gtest.h>

namespace http {
//TODO
//TEST ( HttpClientTest, SimpleRequestV1_0 ) {

//	//create the server
//	WebServer< HttpServer > server ( "127.0.0.1", 9999 );
//	server.bind ( "/foo/bar", std::function< void ( HttpRequest&, HttpResponse& ) > ( [] ( HttpRequest&, HttpResponse & response ) {
//		response.status ( http::http_status::OK );
//		response << "1\n22\n333\n4444\n55555\n";
//	} ) );

//	std::string _expected = "1\n22\n333\n4444\n55555\n";

//	HttpClient client_ ( "localhost", "9999" );
//	HttpRequest request_ ( "/foo/bar" );
//	request_.version_minor ( 0 );
//	std::stringstream _sstream;
//	HttpResponse _response = client_.get ( request_, _sstream );
//	EXPECT_EQ ( http_status::OK, _response.status() );
//	EXPECT_EQ ( 20U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
//	EXPECT_EQ ( _expected, _sstream.str() );
//}

//TEST ( HttpClientTest, SimpleRequestV1_1 ) {

//	//create the server
//	WebServer< HttpServer > server ( "127.0.0.1", 9999 );
//	server.bind ( "/foo/bar", std::function< void ( HttpRequest&, HttpResponse& ) > ( [] ( HttpRequest&, HttpResponse & response ) {
//		response.status ( http::http_status::OK );
//		response << "1\n22\n333\n4444\n55555\n";
//	} ) );

//	std::string _expected = "1\n22\n333\n4444\n55555\n";

//	HttpClient client_ ( "localhost", "9999" );
//	HttpRequest request_ ( "/foo/bar" );
//	std::stringstream _sstream;
//	HttpResponse _response = client_.get ( request_, _sstream );
//	EXPECT_EQ ( http_status::OK, _response.status() );
//	EXPECT_EQ ( 20U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
//	EXPECT_EQ ( _expected, _sstream.str() );
//}

//TEST ( HttpClientTest, PersistentRequestV1_1 ) {

//	//create the server
//	WebServer< HttpServer > server ( "127.0.0.1", 9999 );
//	server.bind ( "/foo/bar", std::function< void ( HttpRequest&, HttpResponse& ) > ( [] ( HttpRequest&, HttpResponse & response ) {
//		response.status ( http::http_status::OK );
//		response << "1\n22\n333\n4444\n55555\n";
//	} ) );

//	std::string _expected = "1\n22\n333\n4444\n55555\n";

//	HttpClient client_ ( "localhost", "9999" );
//	{	//request one
//		HttpRequest request_ ( "/foo/bar" );
//		std::stringstream _sstream;
//		HttpResponse _response = client_.get ( request_, _sstream );
//		EXPECT_EQ ( http_status::OK, _response.status() );
//		EXPECT_EQ ( 20U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
//		EXPECT_EQ ( _expected, _sstream.str() );
//	}
//	{	//request two
//		HttpRequest request_ ( "/foo/bar" );
//		std::stringstream _sstream;
//		HttpResponse _response = client_.get ( request_, _sstream );
//		EXPECT_EQ ( http_status::OK, _response.status() );
//		EXPECT_EQ ( 20U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
//		EXPECT_EQ ( _expected, _sstream.str() );
//	}
//}

//TEST ( HttpClientTest, Request404 ) {

//	//create the server
//	WebServer< HttpServer > server ( "127.0.0.1", 9999 );
//	HttpClient client_ ( "localhost", "9999" );
//	HttpRequest request_ ( "/foo/bar" );
//	request_.version_minor ( 0 );
//	std::stringstream _sstream;
//	HttpResponse _response = client_.get ( request_, _sstream );
//	EXPECT_EQ ( _response.status(), http_status::NOT_FOUND );
//}
}//namespace http
