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

#include <iostream>
#include <fstream>
#include <string>

#include "../src/httpconfig.h"
#include "../src/utils/httpparser.h"
#include "../src/httprequest.h"
#include "../src/httpresponse.h"

#include <gtest/gtest.h>

namespace http {
namespace utils {

inline size_t get_file ( const std::string & filename, buffer_t * buffer ) {
	std::ifstream _file ( filename );

	if ( _file.is_open() ) {
		_file.read ( buffer->data(), BUFFER_SIZE );
		_file.close();

    } else { std::cout << "Unable to open file:" << filename << std::endl; }

	return _file.gcount();
}

TEST ( HttpParserTest, RequestParserType ) {
	HttpParser::request_parser_type start_type = HttpParser::request_parser_type::REQUEST_METHOD;
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_URI, ++start_type );
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_PROTOCOL, ++start_type );
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_VERSION_MAJOR, ++start_type );
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_VERSION_MINOR, ++start_type );
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_KEY, ++start_type );
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_VALUE, ++start_type );
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_END, ++start_type );
}

TEST ( HttpParserTest, ResponseParserType ) {
	HttpParser::response_parser_type start_type = HttpParser::response_parser_type::RESPONSE_PROTOCOL;
	EXPECT_EQ ( HttpParser::response_parser_type::RESPONSE_VERSION_MAJOR, ++start_type );
	EXPECT_EQ ( HttpParser::response_parser_type::RESPONSE_VERSION_MINOR, ++start_type );
	EXPECT_EQ ( HttpParser::response_parser_type::RESPONSE_STATUS, ++start_type );
	EXPECT_EQ ( HttpParser::response_parser_type::RESPONSE_STATUS_TEXT, ++start_type );
	EXPECT_EQ ( HttpParser::response_parser_type::RESPONSE_KEY, ++start_type );
	EXPECT_EQ ( HttpParser::response_parser_type::RESPONSE_VALUE, ++start_type );
	EXPECT_EQ ( HttpParser::response_parser_type::RESPONSE_END, ++start_type );
}

TEST ( HttpParserTest, RequestParserTypeLessThen ) {
	HttpParser::request_parser_type start_type = HttpParser::request_parser_type::REQUEST_METHOD;
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_URI, ++start_type );
	EXPECT_TRUE ( start_type < HttpParser::request_parser_type::REQUEST_KEY );
	EXPECT_FALSE ( start_type > HttpParser::request_parser_type::REQUEST_KEY );
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_PROTOCOL, ++start_type );
	EXPECT_TRUE ( start_type < HttpParser::request_parser_type::REQUEST_KEY );
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_VERSION_MAJOR, ++start_type );
	EXPECT_TRUE ( start_type < HttpParser::request_parser_type::REQUEST_KEY );
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_VERSION_MINOR, ++start_type );
	EXPECT_TRUE ( start_type < HttpParser::request_parser_type::REQUEST_KEY );
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_KEY, ++start_type );
	EXPECT_TRUE ( start_type > HttpParser::request_parser_type::REQUEST_VERSION_MINOR && start_type < HttpParser::request_parser_type::REQUEST_END );
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_VALUE, ++start_type );
	EXPECT_TRUE ( start_type > HttpParser::request_parser_type::REQUEST_VERSION_MINOR && start_type < HttpParser::request_parser_type::REQUEST_END );
	EXPECT_EQ ( HttpParser::request_parser_type::REQUEST_END, ++start_type );
	EXPECT_TRUE ( start_type == HttpParser::request_parser_type::REQUEST_END );
}

TEST ( HttpParserTest, ParseRequestLine ) {
	buffer_t _buffer;
    size_t _size = get_file ( HTTP_TESTFILES + "/raw/request/simple.dump", &_buffer );
	HttpParser _parser;
	HttpParser::RequestParserState _state;
	http::HttpRequest _request;

	size_t _position = _parser.parse_request_status_line ( _state, _request, _buffer, 0, _size );
	EXPECT_EQ ( _position, 16U );

	EXPECT_EQ ( "GET", _request.method() );
	EXPECT_EQ ( "/", _request.uri() );
	EXPECT_EQ ( "HTTP", _request.protocol() );
	EXPECT_EQ ( 1, _request.version_major() );
	EXPECT_EQ ( 1, _request.version_minor() );
}

TEST ( HttpParserTest, ParseResponseLine ) {
	buffer_t _buffer;
    size_t _size = get_file ( HTTP_TESTFILES + "/raw/response/soap_browse_response.dump", &_buffer );
	HttpParser _parser;
	HttpParser::RequestParserState _state;
	http::HttpResponse _response;

	size_t _position = _parser.parse_response_status_line ( _state, _response, _buffer, 0, _size );
	EXPECT_EQ ( _position, 17U );

	EXPECT_EQ ( "HTTP", _response.protocol() );
	EXPECT_EQ ( 1, _response.version_major() );
	EXPECT_EQ ( 1, _response.version_minor() );
	EXPECT_EQ ( http::http_status::OK, _response.status() );
}

TEST ( HttpParserTest, ParseRequestLineAndParameter ) {
	buffer_t _buffer;
    size_t _size = get_file ( HTTP_TESTFILES + "/raw/request/simple.dump", &_buffer );
	HttpParser _parser;
	HttpParser::RequestParserState _state;
	http::HttpRequest _request;

	size_t _position = _parser.parse_request_status_line ( _state, _request, _buffer, 0, _size );
	EXPECT_EQ ( _position, 16U );

	EXPECT_EQ ( "GET", _request.method() );
	EXPECT_EQ ( "/", _request.uri() );
	EXPECT_EQ ( "HTTP", _request.protocol() );
	EXPECT_EQ ( 1, _request.version_major() );
	EXPECT_EQ ( 1, _request.version_minor() );

    _position = _parser.parse_parameter ( _state, _request, _buffer, _position, _size );
	EXPECT_EQ ( _position, _size );

	EXPECT_EQ ( "192.168.0.17:10000", _request.parameter ( "Host" ) );
	EXPECT_EQ ( "keep-alive", _request.parameter ( "Connection" ) );
	EXPECT_EQ ( "1", _request.parameter ( "Upgrade-Insecure-Requests" ) );
	EXPECT_EQ ( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36", _request.parameter ( "User-Agent" ) );
	EXPECT_EQ ( "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8", _request.parameter ( "Accept" ) );
	EXPECT_EQ ( "gzip, deflate, sdch", _request.parameter ( "Accept-Encoding" ) );
	EXPECT_EQ ( "de-DE,de;q=0.8,en-US;q=0.6,en;q=0.4", _request.parameter ( "Accept-Language" ) );
}

TEST ( HttpParserTest, ParseRequestLineAndParameterAndFormData ) {
	buffer_t _buffer;
    size_t _size = get_file ( HTTP_TESTFILES + "/raw/request/form_request_post.dump", &_buffer );
	HttpParser _parser;
	HttpParser::RequestParserState _state;
	http::HttpRequest _request;

	size_t _position = _parser.parse_request_status_line ( _state, _request, _buffer, 0, _size );
	EXPECT_EQ ( _position, 26U );

	EXPECT_EQ ( "POST", _request.method() );
	EXPECT_EQ ( "/form.html", _request.uri() );
	EXPECT_EQ ( "HTTP", _request.protocol() );
	EXPECT_EQ ( 1, _request.version_major() );
	EXPECT_EQ ( 1, _request.version_minor() );

    _position = _parser.parse_parameter ( _state, _request, _buffer, _position, _size );
	EXPECT_EQ ( _position, 575U );

    EXPECT_EQ ( 12U, _request.parameter_map().size() );
	EXPECT_EQ ( "192.168.0.17:9999", _request.parameter ( "Host" ) );
	EXPECT_EQ ( "keep-alive", _request.parameter ( "Connection" ) );
	EXPECT_EQ ( "1", _request.parameter ( "Upgrade-Insecure-Requests" ) );
	EXPECT_EQ ( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36", _request.parameter ( "User-Agent" ) );
	EXPECT_EQ ( "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8", _request.parameter ( "Accept" ) );
	EXPECT_EQ ( "gzip, deflate", _request.parameter ( "Accept-Encoding" ) );
	EXPECT_EQ ( "de-DE,de;q=0.8,en-US;q=0.6,en;q=0.4", _request.parameter ( "Accept-Language" ) );
	EXPECT_EQ ( "http://192.168.0.17:9999/form.html", _request.parameter ( "Referer" ) );
	EXPECT_EQ ( "34", _request.parameter ( "Content-Length" ) );
	EXPECT_EQ ( "max-age=0", _request.parameter ( "Cache-Control" ) );
	EXPECT_EQ ( "http://192.168.0.17:9999", _request.parameter ( "Origin" ) );
	EXPECT_EQ ( "application/x-www-form-urlencoded", _request.parameter ( "Content-Type" ) );

	_position = _parser.parse_body_form_data ( _state, _request, _buffer, _position, _size );
	EXPECT_EQ ( _position, _size );

    EXPECT_EQ ( 3U, _request.attribute_map().size() );
	EXPECT_EQ ( "Heiner", _request.attribute ( "vorname" ) );
	EXPECT_EQ ( "Huber", _request.attribute ( "name" ) );
	EXPECT_EQ ( "0", _request.attribute ( "action" ) );
}

TEST ( HttpParserTest, ParseSimpleRequest ) {
	buffer_t _buffer;
    size_t _size = get_file ( HTTP_TESTFILES + "/raw/request/simple.dump", &_buffer );
	HttpParser _parser;
	http::HttpRequest _request;
	size_t _position = _parser.parse_request ( _request, _buffer, 0, _size );
	EXPECT_EQ ( _position, _size );

	EXPECT_EQ ( "GET", _request.method() );
	EXPECT_EQ ( "/", _request.uri() );
	EXPECT_EQ ( "HTTP", _request.protocol() );
	EXPECT_EQ ( 1, _request.version_major() );
	EXPECT_EQ ( 1, _request.version_minor() );

	EXPECT_EQ ( "192.168.0.17:10000", _request.parameter ( "Host" ) );
	EXPECT_EQ ( "keep-alive", _request.parameter ( "Connection" ) );
	EXPECT_EQ ( "1", _request.parameter ( "Upgrade-Insecure-Requests" ) );
	EXPECT_EQ ( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36", _request.parameter ( "User-Agent" ) );
	EXPECT_EQ ( "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8", _request.parameter ( "Accept" ) );
	EXPECT_EQ ( "gzip, deflate, sdch", _request.parameter ( "Accept-Encoding" ) );
	EXPECT_EQ ( "de-DE,de;q=0.8,en-US;q=0.6,en;q=0.4", _request.parameter ( "Accept-Language" ) );
}

TEST ( HttpParserTest, ParseSoapResponse ) {
	buffer_t _buffer;
    size_t _size = get_file ( HTTP_TESTFILES + "/raw/response/soap_browse_response.dump", &_buffer );
	HttpParser _parser;
	http::HttpResponse _response;
	size_t _position = _parser.parse_response ( _response, _buffer, 0, _size );
	EXPECT_EQ ( _position, 97U );

	EXPECT_EQ ( "HTTP", _response.protocol() );
	EXPECT_EQ ( 1, _response.version_major() );
	EXPECT_EQ ( 1, _response.version_minor() );
	EXPECT_EQ ( http::http_status::OK, _response.status() );

	EXPECT_EQ ( "1416", _response.parameter ( "Content-Length" ) );
	EXPECT_EQ ( "Fri Aug  5 21:09:03 2016", _response.parameter ( "Date" ) );
	EXPECT_EQ ( "text/xml", _response.parameter ( "Content-Type" ) );
}

TEST ( HttpParserTest, ParseSoapLongResponse ) {
	buffer_t _buffer;
    size_t _size = get_file ( HTTP_TESTFILES + "/raw/response/soap_browse_long_response.dump", &_buffer );
	HttpParser _parser;
	http::HttpResponse _response;
	size_t _position = _parser.parse_response ( _response, _buffer, 0, _size );
	EXPECT_EQ ( _position, 98U );

	EXPECT_EQ ( "HTTP", _response.protocol() );
	EXPECT_EQ ( 1, _response.version_major() );
	EXPECT_EQ ( 1, _response.version_minor() );
	EXPECT_EQ ( http::http_status::OK, _response.status() );

	EXPECT_EQ ( "66302", _response.parameter ( "Content-Length" ) );
	EXPECT_EQ ( "Fri Aug  5 21:09:13 2016", _response.parameter ( "Date" ) );
	EXPECT_EQ ( "text/xml", _response.parameter ( "Content-Type" ) );
}

TEST ( HttpParserTest, ParseFormGetRequest ) {
	buffer_t _buffer;
    size_t _size = get_file ( HTTP_TESTFILES + "/raw/request/form_request_get.dump", &_buffer );
	HttpParser _parser;
	http::HttpRequest _request;
	size_t _position = _parser.parse_request ( _request, _buffer, 0, _size );
	EXPECT_EQ ( _position, _size );

	EXPECT_EQ ( "GET", _request.method() );
	EXPECT_EQ ( "/form.html", _request.uri() );
	EXPECT_EQ ( "HTTP", _request.protocol() );
	EXPECT_EQ ( 1, _request.version_major() );
	EXPECT_EQ ( 1, _request.version_minor() );

    EXPECT_EQ ( 8U, _request.parameter_map().size() );
	EXPECT_EQ ( "192.168.0.17:9999", _request.parameter ( "Host" ) );
	EXPECT_EQ ( "keep-alive", _request.parameter ( "Connection" ) );
	EXPECT_EQ ( "1", _request.parameter ( "Upgrade-Insecure-Requests" ) );
	EXPECT_EQ ( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36", _request.parameter ( "User-Agent" ) );
	EXPECT_EQ ( "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8", _request.parameter ( "Accept" ) );
	EXPECT_EQ ( "gzip, deflate, sdch", _request.parameter ( "Accept-Encoding" ) );
	EXPECT_EQ ( "de-DE,de;q=0.8,en-US;q=0.6,en;q=0.4", _request.parameter ( "Accept-Language" ) );
	EXPECT_EQ ( "http://192.168.0.17:9999/form.html", _request.parameter ( "Referer" ) );

    EXPECT_EQ ( 3U, _request.attribute_map().size() );
	EXPECT_EQ ( "Heiner", _request.attribute ( "vorname" ) );
	EXPECT_EQ ( "Huber", _request.attribute ( "name" ) );
	EXPECT_EQ ( "0", _request.attribute ( "action" ) );
}

TEST ( HttpParserTest, ParseFormPostRequest ) {
	buffer_t _buffer;
    size_t _size = get_file ( HTTP_TESTFILES + "/raw/request/form_request_post.dump", &_buffer );
	HttpParser _parser;
	http::HttpRequest _request;
	size_t _position = _parser.parse_request ( _request, _buffer, 0, _size );

	EXPECT_EQ ( _size, _position );

	EXPECT_EQ ( "POST", _request.method() );
	EXPECT_EQ ( "/form.html", _request.uri() );
	EXPECT_EQ ( "HTTP", _request.protocol() );
	EXPECT_EQ ( 1, _request.version_major() );
	EXPECT_EQ ( 1, _request.version_minor() );

    EXPECT_EQ ( 12U, _request.parameter_map().size() );
	EXPECT_EQ ( "192.168.0.17:9999", _request.parameter ( "Host" ) );
	EXPECT_EQ ( "keep-alive", _request.parameter ( "Connection" ) );
	EXPECT_EQ ( "1", _request.parameter ( "Upgrade-Insecure-Requests" ) );
	EXPECT_EQ ( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36", _request.parameter ( "User-Agent" ) );
	EXPECT_EQ ( "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8", _request.parameter ( "Accept" ) );
	EXPECT_EQ ( "gzip, deflate", _request.parameter ( "Accept-Encoding" ) );
	EXPECT_EQ ( "de-DE,de;q=0.8,en-US;q=0.6,en;q=0.4", _request.parameter ( "Accept-Language" ) );
	EXPECT_EQ ( "http://192.168.0.17:9999/form.html", _request.parameter ( "Referer" ) );
	EXPECT_EQ ( "34", _request.parameter ( "Content-Length" ) );
	EXPECT_EQ ( "max-age=0", _request.parameter ( "Cache-Control" ) );
	EXPECT_EQ ( "http://192.168.0.17:9999", _request.parameter ( "Origin" ) );
	EXPECT_EQ ( "application/x-www-form-urlencoded", _request.parameter ( "Content-Type" ) );

    EXPECT_EQ ( 3U, _request.attribute_map().size() );
	EXPECT_EQ ( "Heiner", _request.attribute ( "vorname" ) );
	EXPECT_EQ ( "Huber", _request.attribute ( "name" ) );
	EXPECT_EQ ( "0", _request.attribute ( "action" ) );
}

TEST ( HttpParserTest, ParseFormSoapPost ) {
	buffer_t _buffer;
    size_t _size = get_file ( HTTP_TESTFILES + "/raw/request/soap_browse_request.dump", &_buffer );
	HttpParser _parser;
	http::HttpRequest _request;
	size_t _position = _parser.parse_request ( _request, _buffer, 0, _size );

	EXPECT_EQ ( 676U - 438U, _position );

	EXPECT_EQ ( "POST", _request.method() );
	EXPECT_EQ ( "/ctl/ContentDir", _request.uri() );
	EXPECT_EQ ( "HTTP", _request.protocol() );
	EXPECT_EQ ( 1, _request.version_major() );
	EXPECT_EQ ( 0, _request.version_minor() );

    EXPECT_EQ ( 6U, _request.parameter_map().size() );
	EXPECT_EQ ( "192.168.0.14", _request.parameter ( "Host" ) );
	EXPECT_EQ ( "Coherence PageGetter", _request.parameter ( "User-Agent" ) );
	EXPECT_EQ ( "438", _request.parameter ( "Content-Length" ) );
	EXPECT_EQ ( "\"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"", _request.parameter ( "SoapAction" ) );
	EXPECT_EQ ( "close", _request.parameter ( "Connection" ) );
	EXPECT_EQ ( "text/xml ;charset=\"utf-8\"", _request.parameter ( "Content-Type" ) );
}

TEST ( HttpParserTest, ParseWithBody ) {

	char const * _request = "POST /ctl/ContentDir HTTP/1.0\r\n"
							"Host: 192.168.0.13\r\n"
							"User-Agent: Twisted PageGetter\r\n"
							"Content-Length: 438\r\n"
							"SOAPACTION: \"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"\r\n"
							"content-type: text/xml ;charset=\"utf-8\"\r\n"
							"connection: close\r\n"
							"\r\n"
							"<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:ns0=\"urn:schemas-upnp-org:service:ContentDirectory:1\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><ns0:Browse><ObjectID>0</ObjectID><BrowseFlag>BrowseMetadata</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>0</RequestedCount><SortCriteria /></ns0:Browse></s:Body></s:Envelope>";

	std::array<char, http::BUFFER_SIZE> request;

	for ( size_t i = 0; i < strlen ( _request ); i++ ) {
		request[i] = _request[i];
	}

	char const * response = "<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:ns0=\"urn:schemas-upnp-org:service:ContentDirectory:1\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body><ns0:Browse><ObjectID>0</ObjectID><BrowseFlag>BrowseMetadata</BrowseFlag><Filter>*</Filter><StartingIndex>0</StartingIndex><RequestedCount>0</RequestedCount><SortCriteria /></ns0:Browse></s:Body></s:Envelope>";

	http::HttpRequest http_request;
	HttpParser parser;
	size_t _size = parser.parse_request ( http_request, request, 0, strlen ( _request ) );

	EXPECT_EQ ( 236U, _size );
	EXPECT_EQ ( std::string ( http::method::POST ), http_request.method() );
	EXPECT_EQ ( std::string ( "/ctl/ContentDir" ), http_request.uri() );
	EXPECT_EQ ( std::string ( "HTTP" ), http_request.protocol() );
	EXPECT_EQ ( 1, http_request.version_major() );
	EXPECT_EQ ( 0, http_request.version_minor() );

    EXPECT_EQ ( http_request.parameter_map().size(), 6U );
	EXPECT_EQ ( std::string ( "192.168.0.13" ), http_request.parameter ( "Host" ) );
	EXPECT_EQ ( std::string ( "close" ), http_request.parameter ( "connection" ) );

	char * response_buffer = new char[strlen ( _request ) - _size ];

	for ( size_t i = 0; i <= strlen ( _request ) - _size; ++i ) {
		response_buffer[i] = _request[i + _size];
	}

	EXPECT_EQ ( strlen ( response ), strlen ( response_buffer ) );
	EXPECT_EQ ( std::string ( response ), std::string ( response_buffer ) );
	delete response_buffer;
}

TEST ( HttpParserTest, ParseIncompleteHeader ) {

	char const * _request = "NOTIFY * HTTP/1.1\r\n"
							"Cache-Control: max-age=1800\r\n"
							"Content-Length: 0\r\n"
							"DATE: Sat Dec 28 09:59:08 2013\r\n"
							"Host: 239.255.255.250:1900\r\n";

	std::array<char, http::BUFFER_SIZE> request;

	for ( size_t i = 0; i < strlen ( _request ); i++ ) {
		request[i] = _request[i];
	}

	http::HttpRequest http_request;
	HttpParser http_parser;
	size_t state = http_parser.parse_request ( http_request, request, 0, strlen ( _request ) );
	EXPECT_EQ ( state, 0U );

	EXPECT_EQ ( std::string ( "NOTIFY" ), http_request.method() );
	EXPECT_EQ ( std::string ( "*" ), http_request.uri() );
	EXPECT_EQ ( std::string ( "HTTP" ), http_request.protocol() );
	EXPECT_EQ ( 1, http_request.version_major() );
	EXPECT_EQ ( 1, http_request.version_minor() );


    EXPECT_EQ ( http_request.parameter_map().size(), 4U );
	EXPECT_EQ ( std::string ( "239.255.255.250:1900" ), http_request.parameter ( "Host" ) );
	EXPECT_EQ ( std::string ( "Sat Dec 28 09:59:08 2013" ), http_request.parameter ( "DATE" ) );
	EXPECT_EQ ( std::string ( "max-age=1800" ), http_request.parameter ( "Cache-Control" ) );
	EXPECT_EQ ( std::string ( "0" ), http_request.parameter ( "Content-Length" ) );
}

TEST ( HttpParserTest, ParseChunkedHeader ) {

	http::HttpRequest http_request;
	HttpParser http_parser;

	/* parse first chunk */ {
		std::stringstream _request_stream;
		_request_stream << "GET /index.html HTTP/1.0\r\n";
		_request_stream << "Host: localhost\r\n";

		_request_stream.flush();
		size_t _size = _request_stream.tellp();

		buffer_t _buffer;

		for ( size_t i=0; i<_size; ++i )
		{ _buffer[i] = _request_stream.get(); }

		size_t state = http_parser.parse_request ( http_request, _buffer, 0, _size );
		EXPECT_EQ ( state, 0U );
	}

	/* parse second chunk */ {
		std::stringstream _request_stream;
		_request_stream << "Accept: */*\r\n";
		_request_stream << "Connection: close\r\n\r\n";

		_request_stream.flush();
		size_t _size = _request_stream.tellp();

		buffer_t _buffer;

		for ( size_t i=0; i<_size; ++i )
		{ _buffer[i] = _request_stream.get(); }

		size_t state = http_parser.parse_request ( http_request, _buffer, 0, _size );
		EXPECT_EQ ( 34U, state );
	}
    EXPECT_EQ ( 3U, http_request.parameter_map().size() );
	EXPECT_EQ ( "localhost", http_request.parameter ( http::header::HOST ) );
	EXPECT_EQ ( "*/*", http_request.parameter ( http::header::ACCEPT ) );
	EXPECT_EQ ( "close", http_request.parameter ( http::header::CONNECTION ) );
	EXPECT_FALSE ( http_request.persistent() );
}

TEST ( HttpParserTest, ParseMultiJunks ) {
	/*
	POST /ctl/ContentDir HTTP/1.0
	Content-Type: text/xml; charset="utf-8"
	HOST: 192.168.0.13
	Content-Length: 530
	SOAPACTION: "urn:schemas-upnp-org:service:ContentDirectory:1#Browse"
	Connection: close
	User-Agent: SEC_HHP_iMediaShare/1.0

	<?xml version="1.0" encoding="utf-8"?>
	<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
	   <s:Body>
	      <u:Browse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">
	         <ObjectID>0</ObjectID>
	         <BrowseFlag>BrowseMetadata</BrowseFlag>
	         <Filter>*</Filter>
	         <StartingIndex>0</StartingIndex>
	         <RequestedCount>0</RequestedCount>
	         <SortCriteria></SortCriteria>
	      </u:Browse>
	   </s:Body>
	</s:Envelope>
	 */
	char peer0_0[] = {
		0x50, 0x4f, 0x53, 0x54, 0x20, 0x2f, 0x63, 0x74,
		0x6c, 0x2f, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e,
		0x74, 0x44, 0x69, 0x72, 0x20, 0x48, 0x54, 0x54,
		0x50, 0x2f, 0x31, 0x2e, 0x30, 0x0d, 0x0a, 0x43,
		0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x54,
		0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 0x78,
		0x74, 0x2f, 0x78, 0x6d, 0x6c, 0x3b, 0x20, 0x63,
		0x68, 0x61, 0x72, 0x73, 0x65, 0x74, 0x3d, 0x22,
		0x75, 0x74, 0x66, 0x2d, 0x38, 0x22, 0x0d, 0x0a,
		0x48, 0x4f, 0x53, 0x54, 0x3a, 0x20, 0x31, 0x39,
		0x32, 0x2e, 0x31, 0x36, 0x38, 0x2e, 0x30, 0x2e,
		0x31, 0x33, 0x0d, 0x0a, 0x43, 0x6f, 0x6e, 0x74,
		0x65, 0x6e, 0x74, 0x2d, 0x4c, 0x65, 0x6e, 0x67,
		0x74, 0x68, 0x3a, 0x20, 0x35, 0x33, 0x30, 0x0d,
		0x0a, 0x53, 0x4f, 0x41, 0x50, 0x41, 0x43, 0x54,
		0x49, 0x4f, 0x4e, 0x3a, 0x20, 0x22, 0x75, 0x72,
		0x6e, 0x3a, 0x73, 0x63, 0x68, 0x65, 0x6d, 0x61,
		0x73, 0x2d, 0x75, 0x70, 0x6e, 0x70, 0x2d, 0x6f,
		0x72, 0x67, 0x3a, 0x73, 0x65, 0x72, 0x76, 0x69,
		0x63, 0x65, 0x3a, 0x43, 0x6f, 0x6e, 0x74, 0x65,
		0x6e, 0x74, 0x44, 0x69, 0x72, 0x65, 0x63, 0x74,
		0x6f, 0x72, 0x79, 0x3a, 0x31, 0x23, 0x42, 0x72,
		0x6f, 0x77, 0x73, 0x65, 0x22, 0x0d, 0x0a, 0x43,
		0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x69, 0x6f,
		0x6e, 0x3a, 0x20, 0x63, 0x6c, 0x6f, 0x73, 0x65,
		0x0d, 0x0a, 0x55, 0x73, 0x65, 0x72, 0x2d, 0x41,
		0x67, 0x65, 0x6e, 0x74, 0x3a, 0x20, 0x53, 0x45,
		0x43, 0x5f, 0x48, 0x48, 0x50, 0x5f, 0x69, 0x4d,
		0x65, 0x64, 0x69, 0x61, 0x53, 0x68, 0x61, 0x72,
		0x65, 0x2f, 0x31, 0x2e, 0x30, 0x0d, 0x0a
	};
	char peer0_1[] = {
		0x0d, 0x0a, 0x3c, 0x3f, 0x78, 0x6d, 0x6c, 0x20,
		0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x3d,
		0x22, 0x31, 0x2e, 0x30, 0x22, 0x20, 0x65, 0x6e,
		0x63, 0x6f, 0x64, 0x69, 0x6e, 0x67, 0x3d, 0x22,
		0x75, 0x74, 0x66, 0x2d, 0x38, 0x22, 0x3f, 0x3e,
		0x0a, 0x3c, 0x73, 0x3a, 0x45, 0x6e, 0x76, 0x65,
		0x6c, 0x6f, 0x70, 0x65, 0x20, 0x78, 0x6d, 0x6c,
		0x6e, 0x73, 0x3a, 0x73, 0x3d, 0x22, 0x68, 0x74,
		0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x73, 0x63, 0x68,
		0x65, 0x6d, 0x61, 0x73, 0x2e, 0x78, 0x6d, 0x6c,
		0x73, 0x6f, 0x61, 0x70, 0x2e, 0x6f, 0x72, 0x67,
		0x2f, 0x73, 0x6f, 0x61, 0x70, 0x2f, 0x65, 0x6e,
		0x76, 0x65, 0x6c, 0x6f, 0x70, 0x65, 0x2f, 0x22,
		0x20, 0x73, 0x3a, 0x65, 0x6e, 0x63, 0x6f, 0x64,
		0x69, 0x6e, 0x67, 0x53, 0x74, 0x79, 0x6c, 0x65,
		0x3d, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f,
		0x2f, 0x73, 0x63, 0x68, 0x65, 0x6d, 0x61, 0x73,
		0x2e, 0x78, 0x6d, 0x6c, 0x73, 0x6f, 0x61, 0x70,
		0x2e, 0x6f, 0x72, 0x67, 0x2f, 0x73, 0x6f, 0x61,
		0x70, 0x2f, 0x65, 0x6e, 0x63, 0x6f, 0x64, 0x69,
		0x6e, 0x67, 0x2f, 0x22, 0x3e, 0x0a, 0x20, 0x20,
		0x20, 0x3c, 0x73, 0x3a, 0x42, 0x6f, 0x64, 0x79,
		0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
		0x3c, 0x75, 0x3a, 0x42, 0x72, 0x6f, 0x77, 0x73,
		0x65, 0x20, 0x78, 0x6d, 0x6c, 0x6e, 0x73, 0x3a,
		0x75, 0x3d, 0x22, 0x75, 0x72, 0x6e, 0x3a, 0x73,
		0x63, 0x68, 0x65, 0x6d, 0x61, 0x73, 0x2d, 0x75,
		0x70, 0x6e, 0x70, 0x2d, 0x6f, 0x72, 0x67, 0x3a,
		0x73, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x3a,
		0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x44,
		0x69, 0x72, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x79,
		0x3a, 0x31, 0x22, 0x3e, 0x0a, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x4f,
		0x62, 0x6a, 0x65, 0x63, 0x74, 0x49, 0x44, 0x3e,
		0x30, 0x3c, 0x2f, 0x4f, 0x62, 0x6a, 0x65, 0x63,
		0x74, 0x49, 0x44, 0x3e, 0x0a, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x42,
		0x72, 0x6f, 0x77, 0x73, 0x65, 0x46, 0x6c, 0x61,
		0x67, 0x3e, 0x42, 0x72, 0x6f, 0x77, 0x73, 0x65,
		0x4d, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61,
		0x3c, 0x2f, 0x42, 0x72, 0x6f, 0x77, 0x73, 0x65,
		0x46, 0x6c, 0x61, 0x67, 0x3e, 0x0a, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c,
		0x46, 0x69, 0x6c, 0x74, 0x65, 0x72, 0x3e, 0x2a,
		0x3c, 0x2f, 0x46, 0x69, 0x6c, 0x74, 0x65, 0x72,
		0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x3c, 0x53, 0x74, 0x61, 0x72,
		0x74, 0x69, 0x6e, 0x67, 0x49, 0x6e, 0x64, 0x65,
		0x78, 0x3e, 0x30, 0x3c, 0x2f, 0x53, 0x74, 0x61,
		0x72, 0x74, 0x69, 0x6e, 0x67, 0x49, 0x6e, 0x64,
		0x65, 0x78, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x52, 0x65,
		0x71, 0x75, 0x65, 0x73, 0x74, 0x65, 0x64, 0x43,
		0x6f, 0x75, 0x6e, 0x74, 0x3e, 0x30, 0x3c, 0x2f,
		0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x65,
		0x64, 0x43, 0x6f, 0x75, 0x6e, 0x74, 0x3e, 0x0a,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
		0x20, 0x3c, 0x53, 0x6f, 0x72, 0x74, 0x43, 0x72,
		0x69, 0x74, 0x65, 0x72, 0x69, 0x61, 0x3e, 0x3c,
		0x2f, 0x53, 0x6f, 0x72, 0x74, 0x43, 0x72, 0x69,
		0x74, 0x65, 0x72, 0x69, 0x61, 0x3e, 0x0a, 0x20,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x2f, 0x75,
		0x3a, 0x42, 0x72, 0x6f, 0x77, 0x73, 0x65, 0x3e,
		0x0a, 0x20, 0x20, 0x20, 0x3c, 0x2f, 0x73, 0x3a,
		0x42, 0x6f, 0x64, 0x79, 0x3e, 0x0a, 0x3c, 0x2f,
		0x73, 0x3a, 0x45, 0x6e, 0x76, 0x65, 0x6c, 0x6f,
		0x70, 0x65, 0x3e, 0x0a
	};

	std::array<char, http::BUFFER_SIZE> request;

	for ( size_t i = 0; i < sizeof ( peer0_0 ); i++ ) {
		request[i] = peer0_0[i];
	}

	std::array<char, http::BUFFER_SIZE> request2;

	for ( size_t i = 0; i < sizeof ( peer0_1 ); i++ ) {
		request2[i] = peer0_1[i];
	}

	http::HttpRequest http_request;
	HttpParser http_parser;
	size_t state = http_parser.parse_request ( http_request, request, 0, sizeof ( peer0_0 ) );
	EXPECT_EQ ( 0U, state );
	state = http_parser.parse_request ( http_request, request2, 0, sizeof ( peer0_1 ) );
	EXPECT_EQ ( 2U, state );

	EXPECT_EQ ( std::string ( http::method::POST ), http_request.method() );
	EXPECT_EQ ( std::string ( "/ctl/ContentDir" ), http_request.uri() );
	EXPECT_EQ ( std::string ( "HTTP" ), http_request.protocol() );
	EXPECT_EQ ( 1, http_request.version_major() );
	EXPECT_EQ ( 0, http_request.version_minor() );

    EXPECT_EQ ( http_request.parameter_map().size(), 6U );
	EXPECT_EQ ( std::string ( "text/xml; charset=\"utf-8\"" ), http_request.parameter ( "Content-Type" ) );
	EXPECT_EQ ( std::string ( "192.168.0.13" ), http_request.parameter ( "HOST" ) );
	EXPECT_EQ ( std::string ( "530" ), http_request.parameter ( "Content-Length" ) );
	EXPECT_EQ ( std::string ( "\"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"" ), http_request.parameter ( "SOAPACTION" ) );
	EXPECT_EQ ( std::string ( "close" ), http_request.parameter ( "Connection" ) );
	EXPECT_EQ ( std::string ( "SEC_HHP_iMediaShare/1.0" ), http_request.parameter ( "User-Agent" ) );
}

TEST ( HttpParserTest, ParseChunked ) {
	buffer_t _buffer;
    get_file ( HTTP_TESTFILES + "/raw/request/soap_browse_request.dump", &_buffer );
	HttpParser _parser;
	http::HttpRequest _request;

	size_t _position = 0;
	size_t _res_position = 0;
//    std::cout << "'";
//    for( size_t i = _position; i<4; i++ ) {
//        std::cout << ( _buffer[i] == '\n' ? "\\n" : ( _buffer[i] == '\r' ? "\\r" : std::string( _buffer.data(), i, 1 ) ) );
//    } std::cout << "'" << std::endl;
	_res_position = _parser.parse_request ( _request, _buffer, _position, _position + 4 );

	do {
		_position += 4;
//        std::cout << "'";
//        for( size_t i = _position; i<_position + 4; i++ ) {
//            std::cout << ( _buffer[i] == '\n' ? "\\n" : ( _buffer[i] == '\r' ? "\\r" : std::string( _buffer.data(), i, 1 ) ) );
//        } std::cout << "'" << std::endl;

		_res_position = _parser.parse_request ( _request, _buffer, _position, _position + 4 );
	} while ( _res_position == 0 );

	EXPECT_EQ ( 676U - 438U, _res_position );

	EXPECT_EQ ( "POST", _request.method() );
	EXPECT_EQ ( "/ctl/ContentDir", _request.uri() );
	EXPECT_EQ ( "HTTP", _request.protocol() );
	EXPECT_EQ ( 1, _request.version_major() );
	EXPECT_EQ ( 0, _request.version_minor() );

    EXPECT_EQ ( 6U, _request.parameter_map().size() );
	EXPECT_EQ ( "192.168.0.14", _request.parameter ( "Host" ) );
	EXPECT_EQ ( "Coherence PageGetter", _request.parameter ( "User-Agent" ) );
	EXPECT_EQ ( "438", _request.parameter ( "Content-Length" ) );
	EXPECT_EQ ( "\"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"", _request.parameter ( "SoapAction" ) );
	EXPECT_EQ ( "close", _request.parameter ( "Connection" ) );
	EXPECT_EQ ( "text/xml ;charset=\"utf-8\"", _request.parameter ( "Content-Type" ) );
}

TEST ( HttpParserTest, ParseChunkedWithBody ) {
	buffer_t _buffer;
    size_t _size = get_file ( HTTP_TESTFILES + "/raw/request/form_request_post.dump", &_buffer );
	HttpParser _parser;
	http::HttpRequest _request;

	size_t _position = 0;
	size_t _res_position = 0;
//    std::cout << "'";
//    for( size_t i = _position; i<4; i++ ) {
//        std::cout << ( _buffer[i] == '\n' ? "\\n" : ( _buffer[i] == '\r' ? "\\r" : std::string( _buffer.data(), i, 1 ) ) );
//    } std::cout << "'" << std::endl;
	_res_position = _parser.parse_request ( _request, _buffer, _position, _position + 4 );

	do {
		_position += 4;
//        std::cout << "'";
//        for( size_t i = _position; i<_position + 4 && i < _size; ++i ) {
//            std::cout << ( _buffer[i] == '\n' ? "\\n" : ( _buffer[i] == '\r' ? "\\r" : std::string( _buffer.data(), i, 1 ) ) );
//        } std::cout << "'" << std::endl;

		if ( _position + 4 >= _size ) {
			_res_position = _parser.parse_request ( _request, _buffer, _position, _size );

		} else {
			_res_position = _parser.parse_request ( _request, _buffer, _position, _position + 4 );
		}
	} while ( _res_position == 0 );

	EXPECT_EQ ( _size, _res_position );

	EXPECT_EQ ( "POST", _request.method() );
	EXPECT_EQ ( "/form.html", _request.uri() );
	EXPECT_EQ ( "HTTP", _request.protocol() );
	EXPECT_EQ ( 1, _request.version_major() );
	EXPECT_EQ ( 1, _request.version_minor() );

    EXPECT_EQ ( 12U, _request.parameter_map().size() );
	EXPECT_EQ ( "192.168.0.17:9999", _request.parameter ( "Host" ) );
	EXPECT_EQ ( "keep-alive", _request.parameter ( "Connection" ) );
	EXPECT_EQ ( "1", _request.parameter ( "Upgrade-Insecure-Requests" ) );
	EXPECT_EQ ( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36", _request.parameter ( "User-Agent" ) );
	EXPECT_EQ ( "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8", _request.parameter ( "Accept" ) );
	EXPECT_EQ ( "gzip, deflate", _request.parameter ( "Accept-Encoding" ) );
	EXPECT_EQ ( "de-DE,de;q=0.8,en-US;q=0.6,en;q=0.4", _request.parameter ( "Accept-Language" ) );
	EXPECT_EQ ( "http://192.168.0.17:9999/form.html", _request.parameter ( "Referer" ) );
	EXPECT_EQ ( "34", _request.parameter ( "Content-Length" ) );
	EXPECT_EQ ( "max-age=0", _request.parameter ( "Cache-Control" ) );
	EXPECT_EQ ( "http://192.168.0.17:9999", _request.parameter ( "Origin" ) );
	EXPECT_EQ ( "application/x-www-form-urlencoded", _request.parameter ( "Content-Type" ) );

    EXPECT_EQ ( 3U, _request.attribute_map().size() );
	EXPECT_EQ ( "Heiner", _request.attribute ( "vorname" ) );
	EXPECT_EQ ( "Huber", _request.attribute ( "name" ) );
	EXPECT_EQ ( "0", _request.attribute ( "action" ) );
}

TEST ( HttpParserTest, ParseChunkedWithNewBody ) {
	buffer_t _buffer;
    size_t _size = get_file ( HTTP_TESTFILES + "/raw/request/form_request_post.dump", &_buffer );
	HttpParser _parser;
	http::HttpRequest _request;

	size_t _position = 0;
	size_t _res_position = 0;
//    std::cout << "'";
//    for( size_t i = _position; i<4; i++ ) {
//        std::cout << ( _buffer[i] == '\n' ? "\\n" : ( _buffer[i] == '\r' ? "\\r" : std::string( _buffer.data(), i, 1 ) ) );
//    } std::cout << "'" << std::endl;
	_res_position = _parser.parse_request ( _request, _buffer, _position, _position + 4 );

	do {
		_position += 4;
//        std::cout << "'";
		size_t target_pos = 0;
		size_t target_length = 0;
		buffer_t _buffer2;

		for ( size_t i = _position; i<_position + 4 && i < _size; ++i ) {
//            std::cout << ( _buffer[i] == '\n' ? "\\n" : ( _buffer[i] == '\r' ? "\\r" : std::string( _buffer.data(), i, 1 ) ) );
			_buffer2[ target_pos++] = _buffer[i];
			++target_length;
		} //std::cout << "'" << std::endl;

		_res_position = _parser.parse_request ( _request, _buffer2, 0, target_length );

	} while ( _res_position == 0 );

	EXPECT_EQ ( 1U, _res_position );

	EXPECT_EQ ( "POST", _request.method() );
	EXPECT_EQ ( "/form.html", _request.uri() );
	EXPECT_EQ ( "HTTP", _request.protocol() );
	EXPECT_EQ ( 1, _request.version_major() );
	EXPECT_EQ ( 1, _request.version_minor() );

    EXPECT_EQ ( 12U, _request.parameter_map().size() );
	EXPECT_EQ ( "192.168.0.17:9999", _request.parameter ( "Host" ) );
	EXPECT_EQ ( "keep-alive", _request.parameter ( "Connection" ) );
	EXPECT_EQ ( "1", _request.parameter ( "Upgrade-Insecure-Requests" ) );
	EXPECT_EQ ( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36", _request.parameter ( "User-Agent" ) );
	EXPECT_EQ ( "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8", _request.parameter ( "Accept" ) );
	EXPECT_EQ ( "gzip, deflate", _request.parameter ( "Accept-Encoding" ) );
	EXPECT_EQ ( "de-DE,de;q=0.8,en-US;q=0.6,en;q=0.4", _request.parameter ( "Accept-Language" ) );
	EXPECT_EQ ( "http://192.168.0.17:9999/form.html", _request.parameter ( "Referer" ) );
	EXPECT_EQ ( "34", _request.parameter ( "Content-Length" ) );
	EXPECT_EQ ( "max-age=0", _request.parameter ( "Cache-Control" ) );
	EXPECT_EQ ( "http://192.168.0.17:9999", _request.parameter ( "Origin" ) );
	EXPECT_EQ ( "application/x-www-form-urlencoded", _request.parameter ( "Content-Type" ) );

    EXPECT_EQ ( 3U, _request.attribute_map().size() );
	EXPECT_EQ ( "Heiner", _request.attribute ( "vorname" ) );
	EXPECT_EQ ( "Huber", _request.attribute ( "name" ) );
	EXPECT_EQ ( "0", _request.attribute ( "action" ) );
}

TEST ( HttpParserTest, MultiParse ) {

	HttpParser _parser;
	{
		buffer_t _buffer;
        size_t _size = get_file ( HTTP_TESTFILES + "/raw/request/simple.dump", &_buffer );
		http::HttpRequest _request;
		size_t _position = _parser.parse_request ( _request, _buffer, 0, _size );
		EXPECT_EQ ( _position, _size );

		EXPECT_EQ ( "GET", _request.method() );
		EXPECT_EQ ( "/", _request.uri() );
		EXPECT_EQ ( "HTTP", _request.protocol() );
		EXPECT_EQ ( 1, _request.version_major() );
		EXPECT_EQ ( 1, _request.version_minor() );

		EXPECT_EQ ( "192.168.0.17:10000", _request.parameter ( "Host" ) );
		EXPECT_EQ ( "keep-alive", _request.parameter ( "Connection" ) );
		EXPECT_EQ ( "1", _request.parameter ( "Upgrade-Insecure-Requests" ) );
		EXPECT_EQ ( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36", _request.parameter ( "User-Agent" ) );
		EXPECT_EQ ( "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8", _request.parameter ( "Accept" ) );
		EXPECT_EQ ( "gzip, deflate, sdch", _request.parameter ( "Accept-Encoding" ) );
		EXPECT_EQ ( "de-DE,de;q=0.8,en-US;q=0.6,en;q=0.4", _request.parameter ( "Accept-Language" ) );
	}

	{
		buffer_t _buffer;
        size_t _size = get_file ( HTTP_TESTFILES + "/raw/request/form_request_get.dump", &_buffer );
		http::HttpRequest _request;
		size_t _position = _parser.parse_request ( _request, _buffer, 0, _size );
		EXPECT_EQ ( _position, _size );

		EXPECT_EQ ( "GET", _request.method() );
		EXPECT_EQ ( "/form.html", _request.uri() );
		EXPECT_EQ ( "HTTP", _request.protocol() );
		EXPECT_EQ ( 1, _request.version_major() );
		EXPECT_EQ ( 1, _request.version_minor() );

        EXPECT_EQ ( 8U, _request.parameter_map().size() );
		EXPECT_EQ ( "192.168.0.17:9999", _request.parameter ( "Host" ) );
		EXPECT_EQ ( "keep-alive", _request.parameter ( "Connection" ) );
		EXPECT_EQ ( "1", _request.parameter ( "Upgrade-Insecure-Requests" ) );
		EXPECT_EQ ( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36", _request.parameter ( "User-Agent" ) );
		EXPECT_EQ ( "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8", _request.parameter ( "Accept" ) );
		EXPECT_EQ ( "gzip, deflate, sdch", _request.parameter ( "Accept-Encoding" ) );
		EXPECT_EQ ( "de-DE,de;q=0.8,en-US;q=0.6,en;q=0.4", _request.parameter ( "Accept-Language" ) );
		EXPECT_EQ ( "http://192.168.0.17:9999/form.html", _request.parameter ( "Referer" ) );

        EXPECT_EQ ( 3U, _request.attribute_map().size() );
		EXPECT_EQ ( "Heiner", _request.attribute ( "vorname" ) );
		EXPECT_EQ ( "Huber", _request.attribute ( "name" ) );
		EXPECT_EQ ( "0", _request.attribute ( "action" ) );
	}

	{
		buffer_t _buffer;
        size_t _size = get_file ( HTTP_TESTFILES + "/raw/request/form_request_post.dump", &_buffer );
		http::HttpRequest _request;
		size_t _position = _parser.parse_request ( _request, _buffer, 0, _size );

		EXPECT_EQ ( _size, _position );

		EXPECT_EQ ( "POST", _request.method() );
		EXPECT_EQ ( "/form.html", _request.uri() );
		EXPECT_EQ ( "HTTP", _request.protocol() );
		EXPECT_EQ ( 1, _request.version_major() );
		EXPECT_EQ ( 1, _request.version_minor() );

        EXPECT_EQ ( 12U, _request.parameter_map().size() );
		EXPECT_EQ ( "192.168.0.17:9999", _request.parameter ( "Host" ) );
		EXPECT_EQ ( "keep-alive", _request.parameter ( "Connection" ) );
		EXPECT_EQ ( "1", _request.parameter ( "Upgrade-Insecure-Requests" ) );
		EXPECT_EQ ( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36", _request.parameter ( "User-Agent" ) );
		EXPECT_EQ ( "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8", _request.parameter ( "Accept" ) );
		EXPECT_EQ ( "gzip, deflate", _request.parameter ( "Accept-Encoding" ) );
		EXPECT_EQ ( "de-DE,de;q=0.8,en-US;q=0.6,en;q=0.4", _request.parameter ( "Accept-Language" ) );
		EXPECT_EQ ( "http://192.168.0.17:9999/form.html", _request.parameter ( "Referer" ) );
		EXPECT_EQ ( "34", _request.parameter ( "Content-Length" ) );
		EXPECT_EQ ( "max-age=0", _request.parameter ( "Cache-Control" ) );
		EXPECT_EQ ( "http://192.168.0.17:9999", _request.parameter ( "Origin" ) );
		EXPECT_EQ ( "application/x-www-form-urlencoded", _request.parameter ( "Content-Type" ) );

        EXPECT_EQ ( 3U, _request.attribute_map().size() );
		EXPECT_EQ ( "Heiner", _request.attribute ( "vorname" ) );
		EXPECT_EQ ( "Huber", _request.attribute ( "name" ) );
		EXPECT_EQ ( "0", _request.attribute ( "action" ) );
	}
}

TEST ( HttpParserTest, TestParseSimpleSSDPResponse ) {

	char _request[] = {
		0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x31,
		0x20, 0x32, 0x30, 0x30, 0x20, 0x4f, 0x4b, 0x0d,
		0x0a, 0x43, 0x61, 0x63, 0x68, 0x65, 0x2d, 0x43,
		0x6f, 0x6e, 0x74, 0x72, 0x6f, 0x6c, 0x3a, 0x20,
		0x6d, 0x61, 0x78, 0x2d, 0x61, 0x67, 0x65, 0x3d,
		0x31, 0x38, 0x30, 0x30, 0x0d, 0x0a, 0x44, 0x61,
		0x74, 0x65, 0x3a, 0x20, 0x4d, 0x6f, 0x6e, 0x2c,
		0x20, 0x30, 0x36, 0x20, 0x41, 0x70, 0x72, 0x20,
		0x32, 0x30, 0x31, 0x35, 0x20, 0x31, 0x34, 0x3a,
		0x31, 0x34, 0x3a, 0x34, 0x39, 0x20, 0x47, 0x4d,
		0x54, 0x0d, 0x0a, 0x45, 0x78, 0x74, 0x3a, 0x20,
		0x0d, 0x0a, 0x4c, 0x6f, 0x63, 0x61, 0x74, 0x69,
		0x6f, 0x6e, 0x3a, 0x20, 0x68, 0x74, 0x74, 0x70,
		0x3a, 0x2f, 0x2f, 0x31, 0x39, 0x32, 0x2e, 0x31,
		0x36, 0x38, 0x2e, 0x30, 0x2e, 0x37, 0x3a, 0x39,
		0x30, 0x30, 0x30, 0x2f, 0x70, 0x6c, 0x75, 0x67,
		0x69, 0x6e, 0x73, 0x2f, 0x55, 0x50, 0x6e, 0x50,
		0x2f, 0x4d, 0x65, 0x64, 0x69, 0x61, 0x53, 0x65,
		0x72, 0x76, 0x65, 0x72, 0x2e, 0x78, 0x6d, 0x6c,
		0x0d, 0x0a, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72,
		0x3a, 0x20, 0x4c, 0x69, 0x6e, 0x75, 0x78, 0x2f,
		0x78, 0x38, 0x36, 0x5f, 0x36, 0x34, 0x2d, 0x6c,
		0x69, 0x6e, 0x75, 0x78, 0x20, 0x55, 0x50, 0x6e,
		0x50, 0x2f, 0x31, 0x2e, 0x30, 0x20, 0x44, 0x4c,
		0x4e, 0x41, 0x44, 0x4f, 0x43, 0x2f, 0x31, 0x2e,
		0x35, 0x30, 0x20, 0x4c, 0x6f, 0x67, 0x69, 0x74,
		0x65, 0x63, 0x68, 0x4d, 0x65, 0x64, 0x69, 0x61,
		0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x2f, 0x37,
		0x2e, 0x38, 0x2e, 0x30, 0x2f, 0x31, 0x33, 0x39,
		0x35, 0x34, 0x30, 0x39, 0x39, 0x30, 0x37, 0x0d,
		0x0a, 0x53, 0x54, 0x3a, 0x20, 0x75, 0x70, 0x6e,
		0x70, 0x3a, 0x72, 0x6f, 0x6f, 0x74, 0x64, 0x65,
		0x76, 0x69, 0x63, 0x65, 0x0d, 0x0a, 0x55, 0x53,
		0x4e, 0x3a, 0x20, 0x75, 0x75, 0x69, 0x64, 0x3a,
		0x36, 0x43, 0x41, 0x30, 0x46, 0x34, 0x36, 0x43,
		0x2d, 0x37, 0x44, 0x44, 0x44, 0x2d, 0x34, 0x34,
		0x37, 0x36, 0x2d, 0x39, 0x38, 0x46, 0x35, 0x2d,
		0x37, 0x45, 0x36, 0x43, 0x39, 0x38, 0x36, 0x30,
		0x46, 0x34, 0x38, 0x34, 0x3a, 0x3a, 0x75, 0x70,
		0x6e, 0x70, 0x3a, 0x72, 0x6f, 0x6f, 0x74, 0x64,
		0x65, 0x76, 0x69, 0x63, 0x65, 0x0d, 0x0a, 0x0d,
		0x0a
	};

	std::array<char, http::BUFFER_SIZE> request;

	for ( size_t i = 0; i < sizeof ( _request ); i++ ) {
		request[i] = _request[i];
	}

	http::HttpResponse http_response;
	HttpParser httpParser;
	size_t state = httpParser.parse_response ( http_response, request, 0, sizeof ( _request ) );
	EXPECT_EQ ( sizeof ( _request ), state );
	EXPECT_EQ ( http::http_status::OK, http_response.status() );
	EXPECT_EQ ( "HTTP", http_response.protocol() );
	EXPECT_EQ ( 1, http_response.version_major() );
	EXPECT_EQ ( 1, http_response.version_minor() );

	EXPECT_EQ ( "max-age=1800", http_response.parameter ( "Cache-Control" ) );
	EXPECT_EQ ( "Mon, 06 Apr 2015 14:14:49 GMT", http_response.parameter ( "Date" ) );
	EXPECT_EQ ( "", http_response.parameter ( "Ext" ) );
	EXPECT_EQ ( "http://192.168.0.7:9000/plugins/UPnP/MediaServer.xml", http_response.parameter ( "Location" ) );
	EXPECT_EQ ( "Linux/x86_64-linux UPnP/1.0 DLNADOC/1.50 LogitechMediaServer/7.8.0/1395409907", http_response.parameter ( "Server" ) );
	EXPECT_EQ ( "upnp:rootdevice", http_response.parameter ( "ST" ) );
	EXPECT_EQ ( "uuid:6CA0F46C-7DDD-4476-98F5-7E6C9860F484::upnp:rootdevice", http_response.parameter ( "USN" ) );
}

TEST ( HttpParserTest, MeasurePerformance ) {
	const clock_t begin_time = clock();

	for ( int i = 0; i<10000; ++i ) {

		/*
		POST /ctl/ContentDir HTTP/1.0
		Content-Type: text/xml; charset="utf-8"
		HOST: 192.168.0.13
		Content-Length: 530
		SOAPACTION: "urn:schemas-upnp-org:service:ContentDirectory:1#Browse"
		Connection: close
		User-Agent: SEC_HHP_iMediaShare/1.0

		<?xml version="1.0" encoding="utf-8"?>
		<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
		   <s:Body>
		      <u:Browse xmlns:u="urn:schemas-upnp-org:service:ContentDirectory:1">
		         <ObjectID>0</ObjectID>
		         <BrowseFlag>BrowseMetadata</BrowseFlag>
		         <Filter>*</Filter>
		         <StartingIndex>0</StartingIndex>
		         <RequestedCount>0</RequestedCount>
		         <SortCriteria></SortCriteria>
		      </u:Browse>
		   </s:Body>
		</s:Envelope>
		 */
		char peer0_0[] = {
			0x50, 0x4f, 0x53, 0x54, 0x20, 0x2f, 0x63, 0x74,
			0x6c, 0x2f, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e,
			0x74, 0x44, 0x69, 0x72, 0x20, 0x48, 0x54, 0x54,
			0x50, 0x2f, 0x31, 0x2e, 0x30, 0x0d, 0x0a, 0x43,
			0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x54,
			0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 0x78,
			0x74, 0x2f, 0x78, 0x6d, 0x6c, 0x3b, 0x20, 0x63,
			0x68, 0x61, 0x72, 0x73, 0x65, 0x74, 0x3d, 0x22,
			0x75, 0x74, 0x66, 0x2d, 0x38, 0x22, 0x0d, 0x0a,
			0x48, 0x4f, 0x53, 0x54, 0x3a, 0x20, 0x31, 0x39,
			0x32, 0x2e, 0x31, 0x36, 0x38, 0x2e, 0x30, 0x2e,
			0x31, 0x33, 0x0d, 0x0a, 0x43, 0x6f, 0x6e, 0x74,
			0x65, 0x6e, 0x74, 0x2d, 0x4c, 0x65, 0x6e, 0x67,
			0x74, 0x68, 0x3a, 0x20, 0x35, 0x33, 0x30, 0x0d,
			0x0a, 0x53, 0x4f, 0x41, 0x50, 0x41, 0x43, 0x54,
			0x49, 0x4f, 0x4e, 0x3a, 0x20, 0x22, 0x75, 0x72,
			0x6e, 0x3a, 0x73, 0x63, 0x68, 0x65, 0x6d, 0x61,
			0x73, 0x2d, 0x75, 0x70, 0x6e, 0x70, 0x2d, 0x6f,
			0x72, 0x67, 0x3a, 0x73, 0x65, 0x72, 0x76, 0x69,
			0x63, 0x65, 0x3a, 0x43, 0x6f, 0x6e, 0x74, 0x65,
			0x6e, 0x74, 0x44, 0x69, 0x72, 0x65, 0x63, 0x74,
			0x6f, 0x72, 0x79, 0x3a, 0x31, 0x23, 0x42, 0x72,
			0x6f, 0x77, 0x73, 0x65, 0x22, 0x0d, 0x0a, 0x43,
			0x6f, 0x6e, 0x6e, 0x65, 0x63, 0x74, 0x69, 0x6f,
			0x6e, 0x3a, 0x20, 0x63, 0x6c, 0x6f, 0x73, 0x65,
			0x0d, 0x0a, 0x55, 0x73, 0x65, 0x72, 0x2d, 0x41,
			0x67, 0x65, 0x6e, 0x74, 0x3a, 0x20, 0x53, 0x45,
			0x43, 0x5f, 0x48, 0x48, 0x50, 0x5f, 0x69, 0x4d,
			0x65, 0x64, 0x69, 0x61, 0x53, 0x68, 0x61, 0x72,
			0x65, 0x2f, 0x31, 0x2e, 0x30, 0x0d, 0x0a
		};
		char peer0_1[] = {
			0x0d, 0x0a, 0x3c, 0x3f, 0x78, 0x6d, 0x6c, 0x20,
			0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x3d,
			0x22, 0x31, 0x2e, 0x30, 0x22, 0x20, 0x65, 0x6e,
			0x63, 0x6f, 0x64, 0x69, 0x6e, 0x67, 0x3d, 0x22,
			0x75, 0x74, 0x66, 0x2d, 0x38, 0x22, 0x3f, 0x3e,
			0x0a, 0x3c, 0x73, 0x3a, 0x45, 0x6e, 0x76, 0x65,
			0x6c, 0x6f, 0x70, 0x65, 0x20, 0x78, 0x6d, 0x6c,
			0x6e, 0x73, 0x3a, 0x73, 0x3d, 0x22, 0x68, 0x74,
			0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x73, 0x63, 0x68,
			0x65, 0x6d, 0x61, 0x73, 0x2e, 0x78, 0x6d, 0x6c,
			0x73, 0x6f, 0x61, 0x70, 0x2e, 0x6f, 0x72, 0x67,
			0x2f, 0x73, 0x6f, 0x61, 0x70, 0x2f, 0x65, 0x6e,
			0x76, 0x65, 0x6c, 0x6f, 0x70, 0x65, 0x2f, 0x22,
			0x20, 0x73, 0x3a, 0x65, 0x6e, 0x63, 0x6f, 0x64,
			0x69, 0x6e, 0x67, 0x53, 0x74, 0x79, 0x6c, 0x65,
			0x3d, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f,
			0x2f, 0x73, 0x63, 0x68, 0x65, 0x6d, 0x61, 0x73,
			0x2e, 0x78, 0x6d, 0x6c, 0x73, 0x6f, 0x61, 0x70,
			0x2e, 0x6f, 0x72, 0x67, 0x2f, 0x73, 0x6f, 0x61,
			0x70, 0x2f, 0x65, 0x6e, 0x63, 0x6f, 0x64, 0x69,
			0x6e, 0x67, 0x2f, 0x22, 0x3e, 0x0a, 0x20, 0x20,
			0x20, 0x3c, 0x73, 0x3a, 0x42, 0x6f, 0x64, 0x79,
			0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x3c, 0x75, 0x3a, 0x42, 0x72, 0x6f, 0x77, 0x73,
			0x65, 0x20, 0x78, 0x6d, 0x6c, 0x6e, 0x73, 0x3a,
			0x75, 0x3d, 0x22, 0x75, 0x72, 0x6e, 0x3a, 0x73,
			0x63, 0x68, 0x65, 0x6d, 0x61, 0x73, 0x2d, 0x75,
			0x70, 0x6e, 0x70, 0x2d, 0x6f, 0x72, 0x67, 0x3a,
			0x73, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x3a,
			0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x44,
			0x69, 0x72, 0x65, 0x63, 0x74, 0x6f, 0x72, 0x79,
			0x3a, 0x31, 0x22, 0x3e, 0x0a, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x4f,
			0x62, 0x6a, 0x65, 0x63, 0x74, 0x49, 0x44, 0x3e,
			0x30, 0x3c, 0x2f, 0x4f, 0x62, 0x6a, 0x65, 0x63,
			0x74, 0x49, 0x44, 0x3e, 0x0a, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x42,
			0x72, 0x6f, 0x77, 0x73, 0x65, 0x46, 0x6c, 0x61,
			0x67, 0x3e, 0x42, 0x72, 0x6f, 0x77, 0x73, 0x65,
			0x4d, 0x65, 0x74, 0x61, 0x64, 0x61, 0x74, 0x61,
			0x3c, 0x2f, 0x42, 0x72, 0x6f, 0x77, 0x73, 0x65,
			0x46, 0x6c, 0x61, 0x67, 0x3e, 0x0a, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3c,
			0x46, 0x69, 0x6c, 0x74, 0x65, 0x72, 0x3e, 0x2a,
			0x3c, 0x2f, 0x46, 0x69, 0x6c, 0x74, 0x65, 0x72,
			0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x3c, 0x53, 0x74, 0x61, 0x72,
			0x74, 0x69, 0x6e, 0x67, 0x49, 0x6e, 0x64, 0x65,
			0x78, 0x3e, 0x30, 0x3c, 0x2f, 0x53, 0x74, 0x61,
			0x72, 0x74, 0x69, 0x6e, 0x67, 0x49, 0x6e, 0x64,
			0x65, 0x78, 0x3e, 0x0a, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x52, 0x65,
			0x71, 0x75, 0x65, 0x73, 0x74, 0x65, 0x64, 0x43,
			0x6f, 0x75, 0x6e, 0x74, 0x3e, 0x30, 0x3c, 0x2f,
			0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0x65,
			0x64, 0x43, 0x6f, 0x75, 0x6e, 0x74, 0x3e, 0x0a,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
			0x20, 0x3c, 0x53, 0x6f, 0x72, 0x74, 0x43, 0x72,
			0x69, 0x74, 0x65, 0x72, 0x69, 0x61, 0x3e, 0x3c,
			0x2f, 0x53, 0x6f, 0x72, 0x74, 0x43, 0x72, 0x69,
			0x74, 0x65, 0x72, 0x69, 0x61, 0x3e, 0x0a, 0x20,
			0x20, 0x20, 0x20, 0x20, 0x20, 0x3c, 0x2f, 0x75,
			0x3a, 0x42, 0x72, 0x6f, 0x77, 0x73, 0x65, 0x3e,
			0x0a, 0x20, 0x20, 0x20, 0x3c, 0x2f, 0x73, 0x3a,
			0x42, 0x6f, 0x64, 0x79, 0x3e, 0x0a, 0x3c, 0x2f,
			0x73, 0x3a, 0x45, 0x6e, 0x76, 0x65, 0x6c, 0x6f,
			0x70, 0x65, 0x3e, 0x0a
		};

		std::array<char, http::BUFFER_SIZE> request;

		for ( size_t i = 0; i < sizeof ( peer0_0 ); i++ ) {
			request[i] = peer0_0[i];
		}

		std::array<char, http::BUFFER_SIZE> request2;

		for ( size_t i = 0; i < sizeof ( peer0_1 ); i++ ) {
			request2[i] = peer0_1[i];
		}

		http::HttpRequest http_request;
		HttpParser http_parser;
		size_t state = http_parser.parse_request ( http_request, request, 0, sizeof ( peer0_0 ) );
		EXPECT_EQ ( 0U, state );
		state = http_parser.parse_request ( http_request, request2, 0, sizeof ( peer0_1 ) );
		EXPECT_EQ ( 2U, state );

		EXPECT_EQ ( std::string ( http::method::POST ), http_request.method() );
		EXPECT_EQ ( std::string ( "/ctl/ContentDir" ), http_request.uri() );
		EXPECT_EQ ( std::string ( "HTTP" ), http_request.protocol() );
		EXPECT_EQ ( 1, http_request.version_major() );
		EXPECT_EQ ( 0, http_request.version_minor() );

        EXPECT_EQ ( http_request.parameter_map().size(), 6U );
		EXPECT_EQ ( std::string ( "text/xml; charset=\"utf-8\"" ), http_request.parameter ( "Content-Type" ) );
		EXPECT_EQ ( std::string ( "192.168.0.13" ), http_request.parameter ( "HOST" ) );
		EXPECT_EQ ( std::string ( "530" ), http_request.parameter ( "Content-Length" ) );
		EXPECT_EQ ( std::string ( "\"urn:schemas-upnp-org:service:ContentDirectory:1#Browse\"" ), http_request.parameter ( "SOAPACTION" ) );
		EXPECT_EQ ( std::string ( "close" ), http_request.parameter ( "Connection" ) );
		EXPECT_EQ ( std::string ( "SEC_HHP_iMediaShare/1.0" ), http_request.parameter ( "User-Agent" ) );
	}

	std::cout << "elspsed time: " << float ( clock () - begin_time ) /  CLOCKS_PER_SEC << std::endl;
}

}//namespace utils
}//namespace http
