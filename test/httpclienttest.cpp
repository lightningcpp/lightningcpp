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

TEST ( HttpClientTest, SSL_Request ) {

    HttpClient< Https > client_ ( "www.google.com", "https" );
    Request request_ ( "/images/branding/googlelogo/1x/googlelogo_color_150x54dp.png" );
    request_.version_minor ( 0 );
    //TODO std::stringstream _sstream;
    std::ofstream _sstream( "/tmp/google-logo.png" );
    Response _response = client_.get ( request_, _sstream );
    EXPECT_EQ ( http_status::OK, _response.status() );
    EXPECT_EQ ( 3170U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
    //EXPECT_EQ ( _expected, _sstream.str() );
}

TEST ( HttpClientTest, Simple_SSL_Request ) {

    std::ofstream _sstream( "/tmp/google-logo.png" );
    Response _response = http::get ( "https://www.google.com//images/branding/googlelogo/1x/googlelogo_color_150x54dp.png", _sstream );
    EXPECT_EQ ( http_status::OK, _response.status() );
    EXPECT_EQ ( 3170U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
    //EXPECT_EQ ( _expected, _sstream.str() );
}

TEST ( HttpClientTest, Amazon_SSL_Request ) {

    std::ofstream _sstream( "/tmp/cover.jpg" );
    Response _response = http::get ( "https://images-na.ssl-images-amazon.com/images/I/51QRG0GOwYL.jpg", _sstream );
    EXPECT_EQ ( http_status::OK, _response.status() );
    EXPECT_EQ ( 52741U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
    //EXPECT_EQ ( _expected, _sstream.str() );
}

TEST ( HttpClientTest, post ) {

    //create the server
    bool _call_back_called = false;
    HttpServer _server ( "127.0.0.1", "9000", [&_call_back_called] ( Request & request, Response & response ) {
        _call_back_called = true;
        EXPECT_EQ ( "POST", request.method() );
        EXPECT_EQ ( "/index.html", request.uri() );
        EXPECT_EQ ( "HTTP", request.protocol() );
        EXPECT_EQ ( 1, request.version_major() );
        EXPECT_EQ ( 1, request.version_minor() );

        EXPECT_EQ ( 2U, request.parameter_map().size() );
        EXPECT_EQ ( std::to_string( strlen(  "content bla bla..." ) ), request.parameter ( http::header::CONTENT_LENGTH ) );
        EXPECT_EQ ( "content bla bla...", request.str() );

        response << "1\n22\n333\n4444\n55555\n";

        response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime::TEXT ) );
        response.parameter ( header::CONTENT_LENGTH, std::to_string ( 0 ) );
    } );

    std::stringstream _sstream;

    HttpClient< Http > _client ( "127.0.0.1", "9000" );
    Request _request ( "/index.html" );
    _request.method( "POST" );
    _request << "content bla bla...";
    auto _response = _client.get ( _request, _sstream );
}


////TODO
//TEST ( HttpClientTest, SimpleRequestV1_0 ) {

//    bool _call_back_called = false;

//    //create the server
//    Server< HttpServer > server ( "127.0.0.1", "9999" );
//    server.bind ( "/foo/bar", std::function< void ( Request&, Response& ) > ( [] ( Request&, Response & response ) {
//        response.status ( http::http_status::OK );
//        response << "1\n22\n333\n4444\n55555\n";
//    } ) );

//    std::string _expected = "1\n22\n333\n4444\n55555\n";


//    HttpClient client_ ( "localhost", "9999" );
//    HttpRequest request_ ( "/foo/bar" );
//    request_.version_minor ( 0 );
//    std::stringstream _sstream;
//    HttpResponse _response = client_.get ( request_, _sstream );
//    EXPECT_EQ ( http_status::OK, _response.status() );
//    EXPECT_EQ ( 20U, std::stoul ( _response.parameter ( header::CONTENT_LENGTH ) ) );
//    EXPECT_EQ ( _expected, _sstream.str() );
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
