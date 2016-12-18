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

#include <cstdio>
#include <string>
#include <fstream>

#include "../include/http/httpclient.h"
#include "../include/http/httpserver.h"
#include "../include/http/server.h"
#include "../include/http/mod/http.h"
#include "../include/http/mod/match.h"
#include "../include/http/mod/exec.h"
#include "../include/http/mod/file.h"

#include "testutils.h"

#include <gtest/gtest.h>

namespace http {

TEST ( IntegrationTest, GetSimpleException ) {
	HttpClient client_ ( "localhost", "9999" );
	Request request_ ( "/foo/bar" );
	request_.version_minor ( 0 );
	std::stringstream _sstream;
	EXPECT_THROW ( client_.get ( request_, _sstream ), std::system_error );
}

TEST ( IntegrationTest, GetSimpleSmalFile ) {

	//create the server
    Server< HttpServer > server ( "127.0.0.1", "9999" );
    server.bind( http::mod::Match<>( "*" ), http::mod::Exec( [] ( Request&, Response & response ) {
        response << "abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL";
        return http::http_status::OK;
    } ), http::mod::Http() );

	HttpClient client_ ( "localhost", "9999" );
	Request request_ ( "/simple.txt" );
	std::stringstream _sstream;
	client_.get ( request_, _sstream );
	EXPECT_EQ ( "abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL", _sstream.str() );
}

TEST ( IntegrationTest, GetSimpleLargeFile ) {

    //create the server
    Server< HttpServer > server ( "127.0.0.1", "9999" );
    server.bind( http::mod::Match<>( "*" ), http::mod::File( TESTFILES ), http::mod::Http() );

    HttpClient client_ ( "localhost", "9999" );
    Request request_ ( "/files/The%20Divine%20Comedy.txt" );
    std::stringstream _sstream;
    client_.get ( request_, _sstream );
    std::ifstream f1 ( std::string ( TESTFILES ) + "files/The Divine Comedy.txt", std::ifstream::binary );
    EXPECT_TRUE ( compare_streams ( f1, _sstream ) );
}

TEST ( IntegrationTest, GetSimpleLargeFileAsFile ) {

    std::string _target_file = "/tmp/The Divine Comedy.txt";

    //create the server
    Server< HttpServer > server ( "127.0.0.1", "9999" );
    server.bind( http::mod::Match<>( "*" ), http::mod::File( TESTFILES ), http::mod::Http() );

    HttpClient client_ ( "localhost", "9999" );
    Request request_ ( "/files/The%20Divine%20Comedy.txt" );
    std::fstream _sstream ( _target_file, std::fstream::binary | std::fstream::out );
    client_.get ( request_, _sstream );
    std::ifstream f1 ( std::string ( TESTFILES ) + "files/The Divine Comedy.txt", std::ifstream::binary );
    std::ifstream f2 ( _target_file, std::ifstream::binary );
    EXPECT_TRUE ( compare_streams ( f1, f2 ) );
    EXPECT_TRUE ( remove ( _target_file.c_str() ) == 0 );
}

TEST ( IntegrationTest, RepeatGetSimpleLargeFile ) {

    //create the server
    Server< HttpServer > server ( "127.0.0.1", "9999" );
    server.bind( http::mod::Match<>( "*" ), http::mod::File( TESTFILES ), http::mod::Http() );

    for ( int i = 0; i<100; ++i ) {
        HttpClient client_ ( "localhost", "9999" );
        Request request_ ( "/files/The%20Divine%20Comedy.txt" );
        std::stringstream _sstream;
        client_.get ( request_, _sstream );
        std::ifstream f1 ( std::string ( TESTFILES ) + "files/The Divine Comedy.txt", std::ifstream::binary );
        EXPECT_TRUE ( compare_streams ( f1, _sstream ) );
    }
}

TEST ( IntegrationTest, GetReMatchFile ) {

    //create the server
    Server< HttpServer > server ( "127.0.0.1", "9999" );
    server.bind( http::mod::Match<>( "/foo/.*" ), http::mod::Exec( [] ( Request& request, Response & response ) {
        response << request.uri();
        return http::http_status::OK;
    } ), http::mod::Http() );
    server.bind( http::mod::Match<>( "/foo" ), http::mod::Exec( [] ( Request&, Response & response ) {
        response << "abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL";
        return http::http_status::OK;
    } ), http::mod::Http() );

    {
        HttpClient client_ ( "localhost", "9999" );
        Request request_ ( "/foo" );
        std::stringstream _sstream;
        client_.get ( request_, _sstream );
        EXPECT_EQ ( "abc def ghi jkl mno pqrs tuv wxyz ABC DEF GHI JKL", _sstream.str() );
    }
    {
        HttpClient client_ ( "localhost", "9999" );
        Request request_ ( "/foo/bar" );
        std::stringstream _sstream;
        client_.get ( request_, _sstream );
        EXPECT_EQ ( "/foo/bar", _sstream.str() );

    }
}

TEST ( IntegrationTest, GetFileNotFound ) {

    //create the server
    Server< HttpServer > server ( "127.0.0.1", "9999" );
    server.bind( http::mod::Match<>( "*" ), http::mod::File( TESTFILES ), http::mod::Http() );

    HttpClient client_ ( "localhost", "9999" );
    Request request_ ( "/foo.txt" );
    std::stringstream _sstream;
    Response _response = client_.get ( request_, _sstream );
    EXPECT_EQ ( http_status::NOT_FOUND, _response.status() );
    EXPECT_EQ ( "<html><head><title>Not Found</title></head><body><h1>404 Not Found</h1></body></html>", _sstream.str() );
}

TEST ( IntegrationTest, CustomGetFileNotFound ) {

//    //create the server TODO
//    WebServer2< HttpServer > server ( "127.0.0.1", 9999 );
//    server.bind_error_delegate ( http_status::NOT_FOUND, ErrorDelegate<>::bind ( "404" ) );
//    delegate::FileDelegate file_delegate ( TESTFILES );
//    server.bind ( "*", &delegate::FileDelegate::execute, &file_delegate );

//    HttpClient client_ ( "localhost", "9999" );
//    HttpRequest request_ ( "/foo.txt" );
//    std::stringstream _sstream;
//    HttpResponse _response = client_.get ( request_, _sstream );
//    EXPECT_EQ ( http_status::NOT_FOUND, _response.status() );
//    EXPECT_EQ ( "404", _sstream.str() );
}

//TEST ( IntegrationTest, CustomFileDelegateGetFileNotFound ) { TODO

//    //create the server
//    WebServer2< HttpServer > server ( "127.0.0.1", 9999 );
//    delegate::FileDelegate file_delegate ( TESTFILES );
//    server.bind_error_delegate ( http_status::NOT_FOUND, std::bind ( &delegate::FileDelegate::execute, &file_delegate, std::placeholders::_1, std::placeholders::_2 ) );
//    server.bind( "*", &delegate::FileDelegate::execute, &file_delegate );

//    HttpClient client_ ( "localhost", "9999" );
//    HttpRequest request_ ( "/foo.txt" );
//    std::stringstream _sstream;
//    HttpResponse _response = client_.get ( request_, _sstream );
//    EXPECT_EQ ( http_status::NOT_FOUND, _response.status() );
//    EXPECT_EQ ( static_cast< int > ( http_status::NOT_FOUND ), static_cast< int > ( _response.status() ) );
//    EXPECT_EQ ( "<html><body>404: Not found</body></html>\n", _sstream.str() );
//}

TEST ( IntegrationTest, MultiThreadingTest ) {
    //create the server
    Server< HttpServer > server ( "127.0.0.1", "9999" );
    server.bind( http::mod::Match<>( "*" ), http::mod::File( TESTFILES ), http::mod::Http() );

    for ( int j = 0; j<1; ++j ) { //TODO greater loop for "Too many open files" exception

        std::list< std::thread > threads_;
        std::atomic< int > count1 ( 0 );
        std::atomic< int > count2 ( 0 );
        std::atomic< int > count3 ( 0 );

        for ( int i = 0; i<100; ++i ) {
            std::thread th1 ( [&count1] () {
                ++count1;
                HttpClient client_ ( "localhost", "9999" );
                Request request_ ( "/files/The%20Divine%20Comedy.txt" );
                std::stringstream _sstream;
                client_.get ( request_, _sstream );
                std::ifstream f1 ( std::string ( TESTFILES ) + "files/The Divine Comedy.txt", std::ifstream::binary );
                EXPECT_TRUE ( compare_streams ( f1, _sstream ) );
            } );
            threads_.push_back ( std::move ( th1 ) );
        }

        for ( int i = 0; i<100; ++i ) {
            std::thread th1 ( [&count2] () {
                ++count2;
                HttpClient client_ ( "localhost", "9999" );
                Request request_ ( "/files/Metamorphosis.txt" );
                std::stringstream _sstream;
                client_.get ( request_, _sstream );
                std::ifstream f1 ( std::string ( TESTFILES ) + "files/Metamorphosis.txt", std::ifstream::binary );
                EXPECT_TRUE ( compare_streams ( f1, _sstream ) );
            } );
            threads_.push_back ( std::move ( th1 ) );
        }

        for ( int i = 0; i<100; ++i ) {
            std::thread th1 ( [&count3] () {
                ++count3;
                HttpClient client_ ( "localhost", "9999" );
                Request request_ ( "/files/ALICE%27S%20ADVENTURES%20IN%20WONDERLAND.txt" );
                std::stringstream _sstream;
                client_.get ( request_, _sstream );
                std::ifstream f1 ( std::string ( TESTFILES ) + "files/ALICE'S ADVENTURES IN WONDERLAND.txt", std::ifstream::binary );
                EXPECT_TRUE ( compare_streams ( f1, _sstream ) );
            } );
            threads_.push_back ( std::move ( th1 ) );
        }

        for ( auto & t : threads_ ) {
            t.join();
        }

        EXPECT_EQ ( count1, 100 );
        EXPECT_EQ ( count2, 100 );
        EXPECT_EQ ( count3, 100 );
    }
}
}//namespace http
