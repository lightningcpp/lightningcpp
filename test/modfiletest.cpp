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

#include "../include/http/mod/file.h"

#include <gtest/gtest.h>

#include "testutils.h"

namespace http {
namespace mod {

TEST ( ModFileTest, TestExecute ) {
    File x( TESTFILES );
    Request _request( "/files/simple.txt" );
    Response _response;
    EXPECT_EQ( http::http_status::OK, x.execute ( _request, _response ) );


}
TEST ( ModFileTest, TestExecuteNotFound ) {
    File x( TESTFILES );
    Request _request( "/other.txt" );
    Response _response;
    http_status _status = x.execute ( _request, _response );
    EXPECT_EQ( http::http_status::NOT_FOUND, _status );
}

TEST ( ModFileTest, ParseFilename ) {

    std::string filename_, extension_;
    _file_utils::parse_filename ( "/foo.bar", filename_, extension_ );
    ASSERT_EQ ( "foo.bar", filename_ );
    ASSERT_EQ ( "bar", extension_ );
}
TEST ( ModFileTest, RelativeUrlRequest ) {

    File fd_ ( TESTFILES );
    Request request_ ( "other.txt" );
    Response response_;
    ASSERT_EQ ( http_status::BAD_REQUEST, fd_.execute ( request_, response_ ) );
}
TEST ( ModFileTest, BackUrlRequest ) {

    File fd_ ( TESTFILES );
    Request request_ ( "/../other.txt" );
    Response response_;
    ASSERT_EQ ( http_status::BAD_REQUEST, fd_.execute ( request_, response_ ) );
}
TEST ( ModFileTest, TestRequest ) {

    File fd_ ( TESTFILES );
    Request request_ ( "/files/simple.txt" );
    Response response_;
    ASSERT_EQ ( http_status::OK, fd_.execute ( request_, response_ ) );
}
TEST ( ModFileTest, TestRequestPrefix ) {

    File fd_ ( TESTFILES, "/prefix/" );
    Request request_ ( "/prefix/files/simple.txt" );
    Response response_;
    ASSERT_EQ ( http_status::OK, fd_.execute ( request_, response_ ) );
}
TEST ( ModFileTest, TestRequest404 ) {

    File fd_ ( TESTFILES );
    Request request_ ( "/other.txt" );
    Response response_;
    http_status _stat = fd_.execute ( request_, response_ );
    ASSERT_EQ ( http_status::NOT_FOUND,  _stat );
}
TEST ( ModFileTest, TestNotModified ) {

    File fd_ ( TESTFILES );
    Request request_ ( "/files/simple.txt" );
    Response response_;
    ASSERT_EQ ( http_status::OK, fd_.execute ( request_, response_ ) );

    Request request2_ ( "/files/simple.txt" );
    Response response2_;
    time_t date_ = response_.last_modified();
    request2_.parameter ( http::header::IF_MODIFIED_SINCE, http::utils::time_to_string ( &date_ ) );
    ASSERT_EQ ( http_status::NOT_MODIFIED, fd_.execute ( request2_, response2_ ) );
}
TEST ( ModFileTest, valid_path ) {
    EXPECT_TRUE( File::valid_path( "/index.html" ) );
    EXPECT_TRUE( File::valid_path( "/foo/bar/index.html" ) );
    EXPECT_TRUE( File::valid_path( "/foo/bar/...index.html" ) );
    EXPECT_FALSE( File::valid_path( "/foo/.../...index.html" ) );
    EXPECT_FALSE( File::valid_path( "index.html" ) );
    EXPECT_FALSE( File::valid_path( "" ) );
}
TEST ( ModFileTest, TestRead ) {
    File fd_ ( TESTFILES );
    Request request_ ( "/files/simple.txt" );

    Response response_;
    ASSERT_EQ ( http_status::OK, fd_.execute ( request_, response_ ) );

    std::stringstream _ss;
    std::array< char, BUFFER_SIZE > _buffer;
    auto _size = response_.read( _buffer.data(), BUFFER_SIZE );
    while( _size > 0 ) {
        _ss.write( _buffer.data(), _size );
        _size = response_.read( _buffer.data(), BUFFER_SIZE );
    }
    std::ifstream _file ( std::string ( TESTFILES ) + "files/simple.txt", std::ifstream::binary );
    EXPECT_TRUE( compare_streams( _ss, _file ) );
}
TEST ( ModFileTest, TestChunkedFirstLine ) {
    File fd_ ( TESTFILES );
    Request request_ ( "/files/simple.txt" );
    request_.parameter( header::RANGE, "bytes=0-2" );

    Response response_;
    ASSERT_EQ ( http_status::OK, fd_.execute ( request_, response_ ) );

    EXPECT_EQ( "2", response_.parameter( header::CONTENT_LENGTH ) );
    std::stringstream _ss;
    std::array< char, BUFFER_SIZE > _buffer;

    auto _size = response_.read( _buffer.data(),
        std::stoi( response_.parameter( header::CONTENT_LENGTH ) ) );
    _ss.write( _buffer.data(), _size );
    EXPECT_EQ( 2U, _size );
    EXPECT_EQ( "1\n", _ss.str() );
}
TEST ( ModFileTest, TestChunkedFirstRest ) {
    File fd_ ( TESTFILES );
    Request request_ ( "/files/simple.txt" );
    request_.parameter( header::RANGE, "bytes=9-" );

    Response response_;
    ASSERT_EQ ( http_status::OK, fd_.execute ( request_, response_ ) );

    EXPECT_EQ( "11", response_.parameter( header::CONTENT_LENGTH ) );
    std::stringstream _ss;
    std::array< char, BUFFER_SIZE > _buffer;

    auto _size = response_.read( _buffer.data(),
        std::stoi( response_.parameter( header::CONTENT_LENGTH ) ) );
    _ss.write( _buffer.data(), _size );
    EXPECT_EQ( 11U, _size );
    EXPECT_EQ( "4444\n55555\n", _ss.str() );
}
TEST ( ModFileTest, TestChunkedFirstOutOfRange ) {
    File fd_ ( TESTFILES );
    Request request_ ( "/files/simple.txt" );
    request_.parameter( header::RANGE, "bytes=128-" );

    Response response_;
    ASSERT_EQ ( http_status::REQUEST_RANGE_NOT_SATISFIABLE,
                fd_.execute ( request_, response_ ) );
}
}//namespace mod
}//namespace http
