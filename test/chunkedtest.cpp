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

#include "../include/http/httpserver.h"
#include "../include/http/utils/chunked.h"

#include "testutils.h"

#include <gtest/gtest.h>

namespace http {
namespace utils {

TEST ( ChunkedTest, WriteChunked ) {
    std::stringstream _attribute_stream;
    Chunked chunked ( [&_attribute_stream] ( char* buffer, std::streamsize size ) {
        _attribute_stream << std::string ( buffer, static_cast< size_t > ( size ) );
    } );
    std::stringstream _request_stream;
    _request_stream << "4\r\n";
    _request_stream << "Wiki\r\n";
    _request_stream << "5\r\n";
    _request_stream << "pedia\r\n";
    _request_stream << "E\r\n";
    _request_stream << " in\r\n";
    _request_stream << "\r\n";
    _request_stream << "chunks.\r\n";
    _request_stream << "0\r\n";
    _request_stream << "\r\n\r\n";
    buffer_t _buffer;
    std::streamsize _request_size = _request_stream.tellp();
    _request_stream.read ( _buffer.data(), _request_size );
    auto _is_complete = chunked.write ( _buffer, 0, static_cast< size_t > ( _request_size ) );
    EXPECT_TRUE ( _is_complete );
    EXPECT_EQ ( "Wikipedia in\r\n\r\nchunks.", _attribute_stream.str() );
}
TEST ( ChunkedTest, WriteChunkedFile ) {
    std::stringstream _attribute_stream;
    Chunked chunked ( [&_attribute_stream] ( char* buffer, std::streamsize size ) {
        _attribute_stream.write ( buffer, size );
    } );
    std::ifstream _is ( TESTFILES + std::string ( "raw/chunked/chunkedimage.raw" ), std::ifstream::binary );
    buffer_t _buffer;
    bool _is_complete = false;

    while ( _is ) {
        _is.read ( _buffer.data(), BUFFER_SIZE );
        _is_complete = chunked.write ( _buffer, 0, static_cast< size_t > ( _is.gcount() ) );
    }

    std::ifstream _is_result ( TESTFILES + std::string ( "raw/chunked/chunkedimage.jpeg" ), std::ifstream::binary );
    EXPECT_EQ ( 33653, _attribute_stream.tellp() );
    EXPECT_TRUE ( _is_complete );
    EXPECT_TRUE (  compare_streams ( _is_result, _attribute_stream ) );
}
TEST ( ChunkedTest, WriteChunkedStatus ) {
    std::stringstream _attribute_stream;
    Chunked chunked ( [&_attribute_stream] ( char* buffer, std::streamsize size ) {
        _attribute_stream.write ( buffer, size );
    } );
    std::ifstream _is ( TESTFILES + std::string ( "raw/chunked/chunkedimage.raw" ), std::ifstream::binary );
    buffer_t _buffer;
    bool _is_complete = false;

    _is.read ( _buffer.data(), BUFFER_SIZE );
    _is_complete = chunked.write ( _buffer, 0, static_cast< size_t > ( _is.gcount() ) );
    EXPECT_FALSE ( _is_complete );
    EXPECT_TRUE ( _is );

    _is.read ( _buffer.data(), BUFFER_SIZE );
    _is_complete = chunked.write ( _buffer, 0, static_cast< size_t > ( _is.gcount() ) );
    EXPECT_FALSE ( _is_complete ); //not the complete file is read.
    EXPECT_TRUE ( _is );
}
TEST ( ChunkedTest, WriteChunkedTrailer ) {
    std::stringstream _attribute_stream;
    Chunked chunked ( [&_attribute_stream] ( char* buffer, std::streamsize size ) {
        _attribute_stream << std::string ( buffer, static_cast< size_t > ( size ) );
    } );
    std::stringstream _request_stream;
    _request_stream << "4\r\n";
    _request_stream << "Wiki\r\n";
    _request_stream << "5\r\n";
    _request_stream << "pedia\r\n";
    _request_stream << "E\r\n";
    _request_stream << " in\r\n";
    _request_stream << "\r\n";
    _request_stream << "chunks.\r\n";
    _request_stream << "0\r\n";
    _request_stream << "SomeAfterHeader: TheData\r\n";
    _request_stream << "\r\n\r\n";
    buffer_t _buffer;
    auto _request_size = _request_stream.tellp();
    _request_stream.read ( _buffer.data(), _request_size );
    chunked.write ( _buffer, 0, static_cast< size_t > ( _request_size ) );
    EXPECT_EQ ( "Wikipedia in\r\n\r\nchunks.", _attribute_stream.str() );
    //TODO Extra Data is not parsed; EXPECT_EQ( "TheData", request.attribute( "SomeAfterHeader" ) );
}
}//namespace utils
}//namespace http
