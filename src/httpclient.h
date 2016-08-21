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
#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>

#include <asio.hpp>

#include "httprequest.h"
#include "httpresponse.h"
#include "utils/httpparser.h"

namespace http {

template< class Output, class Enable = void >
struct Writer {
	static void result_write ( Output&, buffer_t&, size_t, size_t ) {}
};
//template<> //TODO
//struct Writer< http::HttpRequest > {
//static void result_write( http::HttpRequest & output, buffer_t & buffer, size_t position, size_t size ) {
//    std::cout << "write request buffer: " << size << std::endl;
//}
//};
template< class Output >
struct Writer< Output, typename std::enable_if< std::is_base_of< std::ostream, Output >::value >::type > {
	static void result_write ( Output & output, buffer_t & buffer, size_t position, size_t size ) {
		output.write ( buffer.data() + position, size );

		if ( ! output.good() ) { //TODO
			std::cout << "write failed" << std::endl;
		}
	}
};

class HttpClient {
public:
	HttpClient ( const std::string & host, const std::string & protocol ) : host_ ( host ), protocol_ ( protocol ), io_service(), socket ( io_service ) {}

	template< class Output >
	HttpResponse get ( HttpRequest & request, Output & output ) {
		if ( ! socket.is_open() ) {
			// std::cout << "connect: " << host_ << ":" << protocol_ << std::endl; //TODO cout
			connect();
		}

		write ( request );

		//read response
		HttpResponse _response;
		read ( _response, output );
		return _response;
	}

private:
	std::string host_;
	std::string protocol_;

	asio::io_service io_service;
	asio::ip::tcp::socket socket;
	buffer_t request_buffer_;
	buffer_t buffer_;
	utils::HttpParser http_parser_;

	void connect();
	void write ( HttpRequest & request );

	template< class Output >
	void read ( HttpResponse & response, Output & output ) {
		int _position = 0;
		asio::error_code error;
		int _len;

		do { //TODO when response is garbage
			_len = socket.read_some ( asio::buffer ( buffer_ ), error );
			_position = http_parser_.parse_response ( response, buffer_, 0, _len );
		} while (  _position == 0 && !error );

		if ( error && error != asio::error::eof ) { throw error; }

		//TODO do requests checks better
		if ( response.protocol() != "HTTP" ) {
			throw http_status::BAD_REQUEST;
		}

		if ( response.status() != http_status::OK ) {
			if ( /* REQUEST NOT AVAILABLE: request.method() == method::HEAD || */ //error without body
				response.status() == http_status::NO_CONTENT ||
				response.status() == http_status::NOT_MODIFIED ||
				( static_cast< int > ( response.status() ) >= 100 && static_cast< int > ( response.status() ) <= 199 ) ) {
				return;
			}
		}

		size_t _read_content = 0; //the recieved content size

		if ( _len - _position > 0 ) { //read the rest of the buffer
			Writer< Output >::result_write ( output, buffer_, _position, _len - _position );
			_read_content = _len - _position;
		}

		//read body from socket
		size_t _content_length = ( response.contains_parameter ( header::CONTENT_LENGTH ) ?
								   std::stoul ( response.parameter ( header::CONTENT_LENGTH ) ) :
								   0 );

		if ( _content_length > 0 && !error ) {
			while ( _read_content < _content_length && !error ) { //get content
				_len = socket.read_some ( asio::buffer ( buffer_ ), error );
				Writer< Output >::result_write ( output, buffer_, 0, _len );
				_read_content += _len;
			};
		}

		if ( error ) { std::cout << "error in reading from socket." << std::endl; } //TODO

		output.flush();
	}
};
}//namespace http
#endif // HTTPCLIENT_H
