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
#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include <array>
#include <list>
#include <iostream>
#include <sstream>

#include "../httpconfig.h"
#include "../httprequest.h"
#include "../httpresponse.h"

#include <gtest/gtest_prod.h>

//TODO request without headers
namespace http {
namespace utils {

class HttpParser {
public:
	HttpParser() {}
	~HttpParser() {}

	/**
	 * @brief parse http request
	 * @param request
	 * @param buffer
	 * @param index
	 * @param end
	 * @return
	 */
	size_t parse_request ( http::HttpRequest & request, buffer_t & buffer, size_t index, size_t end );
	/**
	 * @brief parse http response
	 * @param request
	 * @param buffer
	 * @param index
	 * @param end
	 * @return
	 */
	size_t parse_response ( http::HttpResponse & request, buffer_t & buffer, size_t index, size_t end );

private:
	FRIEND_TEST ( HttpParserTest, RequestParserType );
	FRIEND_TEST ( HttpParserTest, ResponseParserType );
	FRIEND_TEST ( HttpParserTest, RequestParserTypeLessThen );
	FRIEND_TEST ( HttpParserTest, ParseRequestLine );
	FRIEND_TEST ( HttpParserTest, ParseResponseLine );
	FRIEND_TEST ( HttpParserTest, ParseRequestLineAndParameter );
	FRIEND_TEST ( HttpParserTest, ParseRequestLineAndParameterAndFormData );

	enum class request_parser_type {
		REQUEST_METHOD, REQUEST_URI, REQUEST_PROTOCOL, REQUEST_VERSION_MAJOR,
		REQUEST_VERSION_MINOR, REQUEST_KEY, REQUEST_VALUE, REQUEST_END
	};
	friend HttpParser::request_parser_type & operator ++ ( HttpParser::request_parser_type & t );

	enum class response_parser_type {
		RESPONSE_PROTOCOL, RESPONSE_VERSION_MAJOR, RESPONSE_VERSION_MINOR, RESPONSE_STATUS,
		RESPONSE_STATUS_TEXT, RESPONSE_KEY, RESPONSE_VALUE, RESPONSE_END
	};
	friend HttpParser::response_parser_type & operator ++ ( HttpParser::response_parser_type & t );

	struct RequestParserState {
		short line_breaks_ = 0;
		request_parser_type request_type_ = request_parser_type::REQUEST_METHOD;
		response_parser_type response_type_ = response_parser_type::RESPONSE_PROTOCOL;
		size_t start_pos_ = 0;
		std::string key_;
		std::list< std::string > temp_buffer_;
		size_t combined_string_size_ = 0;

		void reset() {
			line_breaks_ = 0;
			request_type_ = request_parser_type::REQUEST_METHOD;
			response_type_ = response_parser_type::RESPONSE_PROTOCOL;
			key_ = std::string();
			temp_buffer_.clear();
			combined_string_size_ = 0;
		}

		/**
		 * @brief get and erase the buffer
		 * @param last_token
		 * @return
		 */
		std::string get_buffer ( const std::string & last_token ) {
			std::stringstream _buf;

			for ( auto & __item : temp_buffer_ ) {
				_buf << __item;
			}

			_buf << last_token;
			temp_buffer_.clear();
			return _buf.str();
		}
	};

	RequestParserState parser_state_; //TODO handle multiple states

	static size_t parse_request_status_line ( RequestParserState & request_parser_state, http::HttpRequest & request, buffer_t & buffer, size_t start, size_t end );
	static size_t parse_response_status_line ( RequestParserState & request_parser_state, http::HttpResponse & response, buffer_t & buffer, size_t start, size_t end );
	static size_t parse_parameter ( RequestParserState & request_parser_state, http::HttpRequest & request, buffer_t & buffer, size_t start, size_t end );
	static size_t parse_parameter ( RequestParserState & request_parser_state, http::HttpResponse & response, buffer_t & buffer, size_t start, size_t end );
	static size_t parse_body_form_data ( RequestParserState & request_parser_state, http::HttpRequest & request, buffer_t & buffer, size_t start, size_t end );

	/**
	 * @brief Get parameters from request line and write them to the request.
	 * @param parameters the request line string.
	 * @param request the request where to store the parameters.
	 */
	static inline void get_attributes ( const std::string & parameters, HttpRequest & request );
	/**
	 * @brief combine string from buffer
	 * @param request_parser_state
	 * @param position
	 * @return
	 */
	static inline std::string combine_string ( RequestParserState & parser_state, buffer_t & buffer, const size_t position ) {
		size_t _last_token_size = ( position < parser_state.start_pos_ ? 0 : position - parser_state.start_pos_ );

		if ( parser_state.temp_buffer_.empty() ) {
			return ( _last_token_size == 0 ? std::string() : std::string ( buffer.data(), parser_state.start_pos_, _last_token_size ) );

		} else {
			return parser_state.get_buffer ( _last_token_size == 0? std::string() : std::string ( buffer.data(), parser_state.start_pos_, _last_token_size ) );
		}
	}
};
}//namespace utils
}//namespace http
#endif // HTTPPARSER_H
