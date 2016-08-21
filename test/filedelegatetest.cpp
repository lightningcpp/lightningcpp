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

#include "../src/httpserver.h"
#include "../src/filedelegate.h"

#include <gtest/gtest.h>

namespace http {
namespace delegate {

TEST ( FileDelegateTest, ParseFilename ) {

	delegate::FileDelegate fd_ ( TESTFILES );
	std::string filename_, extension_;

	fd_.parse_filename ( "/foo.bar", filename_, extension_ );
	ASSERT_EQ ( "foo.bar", filename_ );
	ASSERT_EQ ( "bar", extension_ );
}
TEST ( FileDelegateTest, RelativeUrlRequest ) {

	delegate::FileDelegate fd_ ( TESTFILES );
	HttpRequest request_ ( "other.txt" );
	HttpResponse response_;

	http_status status_;

	try {
		fd_.execute ( request_, response_ );

	} catch ( http_status & status ) {
		status_ = status;
	}

	ASSERT_EQ ( http_status::BAD_REQUEST, status_ );
}
TEST ( FileDelegateTest, BackUrlRequest ) {

	delegate::FileDelegate fd_ ( TESTFILES );
	HttpRequest request_ ( "/../other.txt" );
	HttpResponse response_;

	http_status status_;

	try {
		fd_.execute ( request_, response_ );

	} catch ( http_status & status ) {
		status_ = status;
	}

	ASSERT_EQ ( http_status::BAD_REQUEST, status_ );
}
TEST ( FileDelegateTest, TestRequest ) {

	delegate::FileDelegate fd_ ( TESTFILES );
	HttpRequest request_ ( "/files/simple.txt" );
	HttpResponse response_;

	fd_.execute ( request_, response_ );
	ASSERT_EQ ( http_status::OK, response_.status() );
}
TEST ( FileDelegateTest, TestRequest404 ) {

	delegate::FileDelegate fd_ ( TESTFILES );
	HttpRequest request_ ( "/other.txt" );
	HttpResponse response_;

	http_status status_;

	try {
		fd_.execute ( request_, response_ );

	} catch ( http_status & status ) {
		status_ = status;
	}

	ASSERT_EQ ( http_status::NOT_FOUND, status_ );
}
TEST ( FileDelegateTest, TestNotModified ) {

	delegate::FileDelegate fd_ ( TESTFILES );
	HttpRequest request_ ( "/files/simple.txt" );
	HttpResponse response_;
	fd_.execute ( request_, response_ );

	HttpRequest request2_ ( "/files/simple.txt" );
	HttpResponse response2_;
	time_t date_ = response_.last_modified();
	request2_.parameter ( http::header::IF_MODIFIED_SINCE, http::utils::time_to_string ( &date_ ) );

	http_status status_;

	try {
		fd_.execute ( request2_, response2_ );

	} catch ( http_status & status ) {
		status_ = status;
	}

	ASSERT_EQ ( http_status::NOT_MODIFIED, status_ );
}
TEST ( FileDelegateTest, TestRange ) {
	//TODO
}
}//namespace delegate
}//namespace http
