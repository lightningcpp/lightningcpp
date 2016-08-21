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

#include <cstring>
#include <iostream>
#include <string>

#include "httpresponse.h"
#include "httputils.h"

namespace http {

void HttpResponse::istream ( std::unique_ptr< std::istream > && is ) {
	if ( body_istream_.get() ) {
		throw std::logic_error ( "body input stream can only be set once." );

	} else {
		body_istream_ = std::move ( is );
	}
}

size_t HttpResponse::header ( buffer_t & buffer ) {
	// create status line: Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
	int _position = snprintf ( buffer.data(), BUFFER_SIZE, "%s/%d.%d %d %s\r\n",
							   protocol_.c_str(), version_major_, version_minor_, static_cast< int > ( status_ ), status_reason_phrase ( status_ ).c_str() );

	if ( _position < 0 && static_cast< size_t > ( _position ) > BUFFER_SIZE ) { throw http_status::INTERNAL_SERVER_ERROR; }

	for ( auto & _header : parameters_ ) {
		_position += snprintf ( buffer.data() +_position, BUFFER_SIZE-_position, "%s: %s\r\n", _header.first.c_str(), _header.second.c_str() );

		if ( _position < 0 && static_cast< size_t > ( _position ) > BUFFER_SIZE ) { throw http_status::INTERNAL_SERVER_ERROR; }
	}

	_position += snprintf ( buffer.data()+_position, BUFFER_SIZE-_position, "\r\n" );
	return _position;
}

void HttpResponse::reset() {
	body_istream_.release();
	body_ostream_.str ( std::string() );
	parameters_.clear();
	size_ = 0;
}


size_t HttpResponse::read ( buffer_t & buffer ) {
	if ( body_istream_ ) {
		//read body from input stream
		return body_istream_->readsome ( buffer.data(), BUFFER_SIZE );

	} else {
		// read body from string stream
		body_ostream_.read ( buffer.data(), BUFFER_SIZE );
		size_t read_ = body_ostream_.gcount();
		return read_;
	}
}
} //namespace http
