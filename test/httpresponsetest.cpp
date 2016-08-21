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

#include <fstream>

#include "../src/httpresponse.h"

#include <gtest/gtest.h>

namespace http {

bool compare ( std::istream & r_stream, std::istream & l_stream ) {
	char *lBuffer = new char[BUFFER_SIZE]();
	char *rBuffer = new char[BUFFER_SIZE]();

	do {
		l_stream.read ( lBuffer, BUFFER_SIZE );
		r_stream.read ( rBuffer, BUFFER_SIZE );

		if ( std::memcmp ( lBuffer, rBuffer, BUFFER_SIZE ) != 0 ) {
			delete[] lBuffer;
			delete[] rBuffer;
			return false;
		}
	} while ( l_stream.good() || r_stream.good() );

	return true;
}

TEST ( HttpResponseTest, StringStreamSize ) {

	HttpResponse response_;
	response_ << "abc";
	EXPECT_EQ ( 3U, response_.size() );
	response_ << "def";
	EXPECT_EQ ( 6U, response_.size() );
	response_ << "ghi";
	EXPECT_EQ ( 9U, response_.size() );
}

TEST ( HttpResponseTest, TestStreamException ) {

	HttpResponse response_;
	response_.istream ( std::make_unique< std::stringstream >() );
	ASSERT_THROW ( {
		response_.istream ( std::make_unique< std::stringstream >() );
	}, std::logic_error );

}

TEST ( HttpResponseTest, HeadersToArray ) {
	std::stringstream _ss;
	_ss << "HTTP/1.1 200 OK\r\n" <<
		"Host: localhost\r\n" <<
		"SOAPACTION: \"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"\r\n" <<
		"\r\n";

	HttpResponse _response;

	_response.status ( http_status::OK );
	_response.parameter ( http::header::HOST, "localhost" );
	_response.parameter ( "SOAPACTION", "\"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"" );

	buffer_t _array;
	size_t _size = _response.header ( _array );
	EXPECT_EQ ( static_cast< size_t > ( _ss.tellp() ), _size );
	std::string _result ( _array.data(), 0, _size );
	EXPECT_EQ ( static_cast< size_t > ( _ss.tellp() ), _result.size() );
	EXPECT_EQ ( _ss.str(), _result );
}

TEST ( HttpResponseTest, BodyToArray ) {

	std::string _test_string = "One morning, when Gregor Samsa woke from troubled dreams, he found himself transformed in his bed into a horrible vermin.";

	HttpResponse _response;

	_response.status ( http_status::OK );
	_response.parameter ( http::header::CONNECTION, "close" );
	_response.parameter ( http::header::HOST, "localhost" );
	_response.parameter ( "SOAPACTION", "\"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"" );

	_response << _test_string;

	buffer_t _array;
	size_t _size = _response.read ( _array );
	EXPECT_EQ ( _test_string.size(), _size );
	std::string _result ( _array.data(), 0, _size );
	EXPECT_EQ ( _test_string.size(), _result.size() );
	EXPECT_EQ ( _test_string, _result );
}

TEST ( HttpResponseTest, LargeBodyToArray ) {

	std::stringstream _filename_ss;
	std::stringstream _result_ios;
	_filename_ss << TESTFILES;
	_filename_ss << "files/The Divine Comedy.txt";
	std::ifstream file ( _filename_ss.str() );

	if ( file ) {
		HttpResponse _response;

		_response.status ( http_status::OK );
		_response.parameter ( http::header::CONNECTION, "close" );
		_response.parameter ( http::header::HOST, "localhost" );
		_response.parameter ( "SOAPACTION", "\"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"" );

		char _str[641414U];
		file.read ( _str, 641414U );
		EXPECT_FALSE ( file.eof() );
		EXPECT_EQ ( 641414U, file.tellg() );

		file.read ( _str, 641414U );
		EXPECT_EQ ( -1, file.tellg() );
		EXPECT_TRUE ( file.eof() );

		_response << std::string ( _str, 0, 641414U );

		size_t _size;
		size_t _size_sum = 0;

		do {
			buffer_t _array;
			_size = _response.read ( _array );
			_size_sum += _size;
			_result_ios.write ( _array.data(), _size );
			//_created_file.write( _array.data(), _size );
			//_result_ss << std::string( _array.data(), 0, _size );
		} while ( _size > 0 );

		EXPECT_EQ ( 641414U, _size_sum );
	}

	std::ifstream _reference_file ( _filename_ss.str() );
	EXPECT_TRUE (  compare ( _reference_file, _result_ios ) );
}

}//namespace http
