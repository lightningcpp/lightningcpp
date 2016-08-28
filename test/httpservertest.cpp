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

#include <memory>
#include <string>
#include <vector>

#include "../src/httpserver.h"
#include "../src/httpsession.h"
#include "../src/utils/httpparser.h"

#include <gtest/gtest.h>

#include "testutils.h"

namespace http {

class DelegateDummy {
	DelegateDummy() {}
	virtual ~DelegateDummy() {}

	virtual bool handle ( buffer_t buffer, size_t size ) = 0;
};

TEST ( HttpServerTest, TestCreateServer ) {

	//test data
	asio::streambuf response;
	asio::streambuf request;
	std::ostream request_stream ( &request );
	request_stream << "GET /index.html HTTP/1.0\r\n";
	request_stream << "Host: localhost\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";

	std::stringstream _response_expectedstreambuf;
	_response_expectedstreambuf << "HTTP/1.1 200 OK\r\n" <<
								"Content-Length: 0\r\n" <<
								"Content-Type: text/plain\r\n\r\n";

	//setup server
	bool _call_back_called = false;
	HttpServer _server ( "127.0.0.1", 9000, [&_call_back_called] ( HttpRequest & request, HttpResponse & response ) {
		_call_back_called = true;
		EXPECT_EQ ( "GET", request.method() );
		EXPECT_EQ ( "/index.html", request.uri() );
		EXPECT_EQ ( "HTTP", request.protocol() );
		EXPECT_EQ ( 1, request.version_major() );
		EXPECT_EQ ( 0, request.version_minor() );

        EXPECT_EQ ( 3U, request.parameter_map().size() );
		EXPECT_EQ ( "localhost", request.parameter ( http::header::HOST ) );
		EXPECT_EQ ( "*/*", request.parameter ( http::header::ACCEPT ) );
		EXPECT_EQ ( "close", request.parameter ( http::header::CONNECTION ) );

		response.status ( http::http_status::OK );
		response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime::TEXT ) );
		response.parameter ( header::CONTENT_LENGTH, std::to_string ( 0 ) );
	} );

	//send a request
	client_header ( "127.0.0.1", "9000", std::vector< asio::streambuf* > ( { &request } ), response );
	std::istream _response_stream ( &response );
	EXPECT_TRUE ( compare_streams ( _response_expectedstreambuf, _response_stream ) );
	EXPECT_TRUE ( _call_back_called );
}
TEST ( HttpServerTest, TestReadChunkedHeader ) {


	bool _call_back_called = false;
	HttpServer _server ( "127.0.0.1", 9000, [&_call_back_called] ( HttpRequest & request, HttpResponse & response ) {
		_call_back_called = true;
		EXPECT_EQ ( "GET", request.method() );
		EXPECT_EQ ( "/index.html", request.uri() );
		EXPECT_EQ ( "HTTP", request.protocol() );
		EXPECT_EQ ( 1, request.version_major() );
		EXPECT_EQ ( 0, request.version_minor() );

        EXPECT_EQ ( 3U, request.parameter_map().size() );

		EXPECT_EQ ( "localhost", request.parameter ( http::header::HOST ) );
		EXPECT_EQ ( "*/*", request.parameter ( http::header::ACCEPT ) );
		EXPECT_EQ ( "close", request.parameter ( http::header::CONNECTION ) );

		response.status ( http::http_status::OK );
		response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime::TEXT ) );
		response.parameter ( header::CONTENT_LENGTH, std::to_string ( 0 ) );
	} );

	asio::streambuf request1;
	std::ostream request_stream1 ( &request1 );
	request_stream1 << "GET /index.html HTTP/1.0\r\n";
	request_stream1 << "Host: localhost\r\n";

	asio::streambuf request2;
	std::ostream request_stream2 ( &request2 );
	request_stream2 << "Accept: */*\r\n";
	request_stream2 << "Connection: close\r\n\r\n";

	asio::streambuf response;
	client_header ( "127.0.0.1", "9000", std::vector< asio::streambuf* > ( { &request1, &request2 } ), response );
	std::istream _response_stream ( &response );
	std::stringstream _response_expectedstreambuf;
	_response_expectedstreambuf << "HTTP/1.1 200 OK\r\n" <<
								"Content-Length: 0\r\n" <<
								"Content-Type: text/plain\r\n\r\n";
	EXPECT_TRUE ( compare_streams ( _response_expectedstreambuf, _response_stream ) );
	EXPECT_TRUE ( _call_back_called );
}
TEST ( HttpServerTest, TestReadBody ) {

	bool _call_back_called = false;
	HttpServer _server ( "127.0.0.1", 9000, [&_call_back_called] ( HttpRequest & request, HttpResponse & response ) {
		_call_back_called = true;
		EXPECT_EQ ( "GET", request.method() );
		EXPECT_EQ ( "/index.html", request.uri() );
		EXPECT_EQ ( "HTTP", request.protocol() );
		EXPECT_EQ ( 1, request.version_major() );
		EXPECT_EQ ( 0, request.version_minor() );

        EXPECT_EQ ( 4U, request.parameter_map().size() );

		EXPECT_EQ ( "localhost", request.parameter ( http::header::HOST ) );
		EXPECT_EQ ( "*/*", request.parameter ( http::header::ACCEPT ) );
		EXPECT_EQ ( "close", request.parameter ( http::header::CONNECTION ) );
		EXPECT_EQ ( "105", request.parameter ( http::header::CONTENT_LENGTH ) );

        EXPECT_EQ ( "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.", request.str() );

		response.status ( http::http_status::OK );
		response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime::TEXT ) );
		response.parameter ( header::CONTENT_LENGTH, std::to_string ( 0 ) );
	} );

	asio::streambuf request1;
	std::ostream request_stream ( &request1 );
	request_stream << "GET /index.html HTTP/1.0\r\n";
	request_stream << "Host: localhost\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Content-Length: 105\r\n";
	request_stream << "Connection: close\r\n\r\n";
	request_stream << "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa.";

	asio::streambuf response;
	client_header ( "127.0.0.1", "9000", std::vector< asio::streambuf* > ( { &request1 } ), response );
	std::istream _response_stream ( &response );
	std::stringstream _response_expectedstreambuf;
	_response_expectedstreambuf << "HTTP/1.1 200 OK\r\n" <<
								"Content-Length: 0\r\n" <<
								"Content-Type: text/plain\r\n\r\n";
	EXPECT_TRUE ( compare_streams ( _response_expectedstreambuf, _response_stream ) );
	EXPECT_TRUE ( _call_back_called );
}
TEST ( HttpServerTest, DISABLED_TestReadChunkedEncoding ) { //TODO
	bool _call_back_called = false;
	HttpServer _server ( "127.0.0.1", 9000, [&_call_back_called] ( HttpRequest & request, HttpResponse & ) {
		_call_back_called = true;
		std::string _res = "Wikipedia in\r\n\r\nchunks.";
        EXPECT_EQ ( 4U, request.parameter_map().size() );
		EXPECT_EQ ( "localhost", request.parameter ( http::header::HOST ) );
		EXPECT_EQ ( "*/*", request.parameter ( http::header::ACCEPT ) );
		EXPECT_EQ ( "close", request.parameter ( http::header::CONNECTION ) );
		EXPECT_EQ ( "chunked", request.parameter ( http::header::TRANSFER_ENCODING ) );
		EXPECT_FALSE ( request.persistent() );

        std::string _body = request.str();
		EXPECT_EQ ( _res, _body );
	} );
	// _server.run();

	/** send a request **/
	// Get a list of endpoints corresponding to the server name.
	using asio::ip::tcp;

	asio::io_service io_service;
	tcp::resolver resolver ( io_service );
	tcp::resolver::query query ( "127.0.0.1", "9000" );
	tcp::resolver::iterator endpoint_iterator = resolver.resolve ( query );

	// Try each endpoint until we successfully establish a connection.
	tcp::socket socket ( io_service );
	asio::connect ( socket, endpoint_iterator );

	/* send first part of the request */
	{
		asio::streambuf request;
		std::ostream _request_stream ( &request );
		_request_stream << "GET /index.html HTTP/1.0\r\n";
		_request_stream << "Host: localhost\r\n";
		_request_stream << "Accept: */*\r\n";
		_request_stream << "Transfer-Encoding: chunked\r\n";
		_request_stream << "Connection: close\r\n\r\n";
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
		// Send the request.
		asio::write ( socket, request );
	}

	asio::streambuf response;
	asio::read_until ( socket, response, "\r\n" );
	std::istream response_stream ( &response );
	std::string http_version;
	response_stream >> http_version;
	EXPECT_EQ ( "HTTP/1.1", http_version );
	EXPECT_TRUE ( _call_back_called );
}
TEST ( HttpServerTest, TestResponse ) {

	std::string _test_string = "One morning, when Gregor Samsa woke from troubled dreams, he found himself transformed in his bed into a horrible vermin.";
	std::stringstream result_header_ss;
	result_header_ss << "HTTP/1.1 200 OK\r\n";
	result_header_ss << "Content-Length: 121\r\n";
	result_header_ss << "Content-Type: text/plain\r\n\r\n";

	bool _call_back_called = false;
	HttpServer _server ( "127.0.0.1", 9000, [&_call_back_called, &_test_string] ( HttpRequest & request, HttpResponse & response ) {
		_call_back_called = true;
		EXPECT_EQ ( "GET", request.method() );
		EXPECT_EQ ( "/index.html", request.uri() );
		EXPECT_EQ ( "HTTP", request.protocol() );
		EXPECT_EQ ( 1, request.version_major() );
		EXPECT_EQ ( 0, request.version_minor() );

        EXPECT_EQ ( 3U, request.parameter_map().size() );

		EXPECT_EQ ( "localhost", request.parameter ( http::header::HOST ) );
		EXPECT_EQ ( "*/*", request.parameter ( http::header::ACCEPT ) );
		EXPECT_EQ ( "close", request.parameter ( http::header::CONNECTION ) );

		response.status ( http::http_status::OK );
		response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime::TEXT ) );
		response.parameter ( header::CONTENT_LENGTH, std::to_string ( _test_string.size() ) );
		response << _test_string;
	} );

	/** send a request **/

	using asio::ip::tcp;

	asio::io_service io_service;
	tcp::resolver resolver ( io_service );
	tcp::resolver::query query ( "127.0.0.1", "9000" );
	tcp::resolver::iterator endpoint_iterator = resolver.resolve ( query );

	tcp::socket socket ( io_service );
	asio::connect ( socket, endpoint_iterator );

	asio::streambuf request;
	std::ostream request_stream ( &request );
	request_stream << "GET /index.html HTTP/1.0\r\n";
	request_stream << "Host: localhost\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";

	// Send the request.
	asio::write ( socket, request );
	buffer_t _buffer;
	asio::error_code error;

	size_t len = socket.read_some ( asio::buffer ( _buffer ), error );
	EXPECT_FALSE ( error );

	utils::HttpParser _parser;
	HttpResponse _response;
	size_t _position = _parser.parse_response ( _response, _buffer, 0, len );
	EXPECT_EQ ( static_cast< size_t > ( result_header_ss.tellp() ), _position );
	EXPECT_EQ ( _response.parameters_.size(), 2U );
	EXPECT_EQ ( _response.parameter ( header::CONTENT_LENGTH ), "121" );
	EXPECT_EQ ( _response.parameter ( header::CONTENT_TYPE ), "text/plain" );

	std::string _result;

	if ( len > _position ) {
		_result = std::string ( _buffer.data(), _position, len - _position );
	}

	while ( _result.size() < 121 ) {
		size_t len = socket.read_some ( asio::buffer ( _buffer ), error );

		if ( len > 0 ) {
			_result += std::string ( _buffer.data(), 0, len );
		}
	}

	EXPECT_EQ ( _test_string, _result );
}
TEST ( HttpServerTest, TestPersistentConnectionV10 ) {

	std::string _test_string = "One morning, when Gregor Samsa woke from troubled dreams, he found himself transformed in his bed into a horrible vermin.";
	std::stringstream result_header_ss;
	result_header_ss << "HTTP/1.1 200 OK\r\n";
	result_header_ss << "Content-Length: 121\r\n";
	result_header_ss << "Content-Type: text/plain\r\n\r\n";

	bool _call_back_called = false;
	HttpServer _server ( "127.0.0.1", 9000, [&_call_back_called, &_test_string] ( HttpRequest & request, HttpResponse & response ) {
		_call_back_called = true;
		EXPECT_EQ ( "GET", request.method() );
		EXPECT_EQ ( "/index.html", request.uri() );
		EXPECT_EQ ( "HTTP", request.protocol() );
		EXPECT_EQ ( 1, request.version_major() );
		EXPECT_EQ ( 1, request.version_minor() );

        EXPECT_EQ ( 3U, request.parameter_map().size() );

		EXPECT_EQ ( "localhost", request.parameter ( http::header::HOST ) );
		EXPECT_EQ ( "*/*", request.parameter ( http::header::ACCEPT ) );
		EXPECT_EQ ( "keep-alive", request.parameter ( http::header::CONNECTION ) );

		response.status ( http::http_status::OK );
		response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime::TEXT ) );
		response.parameter ( header::CONTENT_LENGTH, std::to_string ( _test_string.size() ) );
		response << _test_string;
	} );

	/** send a request **/

	using asio::ip::tcp;

	asio::io_service io_service;
	tcp::resolver resolver ( io_service );
	tcp::resolver::query query ( "127.0.0.1", "9000" );
	tcp::resolver::iterator endpoint_iterator = resolver.resolve ( query );

	tcp::socket socket ( io_service );
	asio::connect ( socket, endpoint_iterator );


	// Send the first request.
	{
		asio::streambuf request;
		std::ostream request_stream ( &request );
		request_stream << "GET /index.html HTTP/1.1\r\n";
		request_stream << "Host: localhost\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: keep-alive\r\n\r\n";

		asio::write ( socket, request );
		buffer_t _buffer;
		asio::error_code error;

		size_t len = socket.read_some ( asio::buffer ( _buffer ), error );
		EXPECT_FALSE ( error );

		utils::HttpParser _parser;
		HttpResponse _response;
		size_t _position = _parser.parse_response ( _response, _buffer, 0, len );
		EXPECT_EQ ( static_cast< size_t > ( result_header_ss.tellp() ), _position );

		std::string _result;

		if ( len > _position ) {
			_result = std::string ( _buffer.data(), _position, len - _position );
		}

		while ( _result.size() < 121 ) {
			size_t len = socket.read_some ( asio::buffer ( _buffer ), error );

			if ( len > 0 ) {
				_result += std::string ( _buffer.data(), 0, len );
			}
		}

		EXPECT_EQ ( _test_string, _result );
	}
	// Send the second request.
	{
		asio::streambuf request;
		std::ostream request_stream ( &request );
		request_stream << "GET /index.html HTTP/1.1\r\n";
		request_stream << "Host: localhost\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: keep-alive\r\n\r\n";

		asio::write ( socket, request );
		buffer_t _buffer;
		asio::error_code error;
		EXPECT_FALSE ( error );

		size_t len = socket.read_some ( asio::buffer ( _buffer ), error );
        std::cout << error << std::endl;
		EXPECT_FALSE ( error );

		utils::HttpParser _parser;
		HttpResponse _response;
		size_t _position = _parser.parse_response ( _response, _buffer, 0, len );
		EXPECT_EQ ( static_cast< size_t > ( result_header_ss.tellp() ), _position );

		std::string _result;

		if ( len > _position ) {
			_result = std::string ( _buffer.data(), _position, _position + _test_string.size() );
		}

		while ( _result.size() < 121 ) {
			size_t len = socket.read_some ( asio::buffer ( _buffer ), error );

			if ( len > 0 ) {
				_result += std::string ( _buffer.data(), 0, len );
			}
		}

		EXPECT_EQ ( _test_string, _result );
	}
}
TEST ( HttpServerTest, TestPersistentConnectionV11 ) {

	std::string _test_string = "One morning, when Gregor Samsa woke from troubled dreams, he found himself transformed in his bed into a horrible vermin.";
	std::stringstream result_header_ss;
	result_header_ss << "HTTP/1.1 200 OK\r\n";
	result_header_ss << "Content-Length: 121\r\n";
	result_header_ss << "Content-Type: text/plain\r\n\r\n";

	bool _call_back_called = false;
	HttpServer _server ( "127.0.0.1", 9000, [&_call_back_called, &_test_string] ( HttpRequest & request, HttpResponse & response ) {
		_call_back_called = true;
		EXPECT_EQ ( "GET", request.method() );
		EXPECT_EQ ( "/index.html", request.uri() );
		EXPECT_EQ ( "HTTP", request.protocol() );
		EXPECT_EQ ( 1, request.version_major() );
		EXPECT_EQ ( 1, request.version_minor() );

        EXPECT_EQ ( 2U, request.parameter_map().size() );

		EXPECT_EQ ( "localhost", request.parameter ( http::header::HOST ) );
		EXPECT_EQ ( "*/*", request.parameter ( http::header::ACCEPT ) );

		response.status ( http::http_status::OK );
		response.parameter ( header::CONTENT_TYPE, mime::mime_type ( mime::TEXT ) );
		response.parameter ( header::CONTENT_LENGTH, std::to_string ( _test_string.size() ) );
		response << _test_string;
	} );

	/** send a request **/

	using asio::ip::tcp;

	asio::io_service io_service;
	tcp::resolver resolver ( io_service );
	tcp::resolver::query query ( "127.0.0.1", "9000" );
	tcp::resolver::iterator endpoint_iterator = resolver.resolve ( query );

	tcp::socket socket ( io_service );
	asio::connect ( socket, endpoint_iterator );


	// Send the first request.
	{
		asio::streambuf request;
		std::ostream request_stream ( &request );
		request_stream << "GET /index.html HTTP/1.1\r\n";
		request_stream << "Host: localhost\r\n";
		request_stream << "Accept: */*\r\n\r\n";

		asio::write ( socket, request );
		buffer_t _buffer;
		asio::error_code error;

		size_t len = socket.read_some ( asio::buffer ( _buffer ), error );
		EXPECT_FALSE ( error );

		utils::HttpParser _parser;
		HttpResponse _response;
		size_t _position = _parser.parse_response ( _response, _buffer, 0, len );
		EXPECT_EQ ( static_cast< size_t > ( result_header_ss.tellp() ), _position );
		EXPECT_EQ ( http_status::OK, _response.status() );

		std::string _result;

		if ( len > _position ) {
			_result = std::string ( _buffer.data(), _position, len - _position );
		}

		while ( _result.size() < 121 ) {
			size_t len = socket.read_some ( asio::buffer ( _buffer ), error );

			if ( len > 0 ) {
				_result += std::string ( _buffer.data(), 0, len );
			}
		}

		EXPECT_EQ ( _test_string, _result );
	}
	// Send the second request.
	{
		asio::streambuf request;
		std::ostream request_stream ( &request );
		request_stream << "GET /index.html HTTP/1.1\r\n";
		request_stream << "Host: localhost\r\n";
		request_stream << "Accept: */*\r\n\r\n";

		asio::write ( socket, request );
		buffer_t _buffer;
		asio::error_code error;
		EXPECT_FALSE ( error );

		size_t len = socket.read_some ( asio::buffer ( _buffer ), error );
		EXPECT_FALSE ( error );

		utils::HttpParser _parser;
		HttpResponse _response;
		size_t _position = _parser.parse_response ( _response, _buffer, 0, len );
		EXPECT_EQ ( static_cast< size_t > ( result_header_ss.tellp() ), _position );
		EXPECT_EQ ( http_status::OK, _response.status() );

		std::string _result;

		if ( len > _position ) {
			_result = std::string ( _buffer.data(), _position, len - _position );
		}

		while ( _result.size() < 121 ) {
			size_t len = socket.read_some ( asio::buffer ( _buffer ), error );

			if ( len > 0 ) {
				_result += std::string ( _buffer.data(), 0, len );
			}
		}

		EXPECT_EQ ( _test_string, _result );
	}
}
}//namespace http
