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

#include "../src/mod/file.h"

#include <gtest/gtest.h>

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
    file::parse_filename ( "/foo.bar", filename_, extension_ );
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
TEST ( ModFileTest, TestRange ) {
    //TODO
}

}//namespace mod
}//namespace http
