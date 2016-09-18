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

#include "../constant.h"
#include "../request.h"
#include "../response.h"

//TODO request without headers
namespace http {
namespace utils {

/**
 * @brief The HttpParser class
 */
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
    size_t parse_request ( Request & request, buffer_t & buffer, size_t start, size_t end ) {
        size_t _position = start;

        if ( parser_state_.request_type_ < request_parser_type::REQUEST_KEY ) {
            _position = parse_request_status_line ( parser_state_, request, buffer, start, end );

            if ( _position == 0 ) { return 0; }
        }

        if ( parser_state_.request_type_ < request_parser_type::REQUEST_END ) {
            _position = parse_parameter ( parser_state_, request, buffer, _position, end );

            if ( _position == 0 ) { return 0; }
        }

        if ( request.contains_parameter ( http::header::CONTENT_TYPE ) && request.parameter ( http::header::CONTENT_TYPE ) == http::header::value::FORM_URLENCODED ) {
            _position = parse_body_form_data ( parser_state_, request, buffer, _position, end );

            if ( _position == 0 ) { return 0; }
        }

        parser_state_.reset();
        return _position;
    }

    /**
	 * @brief parse http response
	 * @param request
	 * @param buffer
	 * @param index
	 * @param end
	 * @return
	 */
    size_t parse_response ( Response & response, buffer_t & buffer, size_t start, size_t end ) {
        size_t _position = start;

        if ( parser_state_.response_type_ < response_parser_type::RESPONSE_KEY ) {
            _position = parse_response_status_line ( parser_state_, response, buffer, start, end );

            if ( _position == 0 ) { return 0; }
        }

        if ( parser_state_.response_type_ < response_parser_type::RESPONSE_END ) {
            _position = parse_parameter ( parser_state_, response, buffer, _position, end );

            if ( _position == 0 ) { return 0; }
        }

        parser_state_.reset();
        return _position;
    }

    ///@cond DOC_INTERNAL
    enum class request_parser_type {
        REQUEST_METHOD, REQUEST_URI, REQUEST_PROTOCOL, REQUEST_VERSION_MAJOR,
        REQUEST_VERSION_MINOR, REQUEST_KEY, REQUEST_VALUE, REQUEST_END
    };

    friend HttpParser::request_parser_type & operator ++ ( HttpParser::request_parser_type & t ) {
        switch ( t ) {
        case HttpParser::request_parser_type::REQUEST_METHOD:
            return t = HttpParser::request_parser_type::REQUEST_URI;

        case HttpParser::request_parser_type::REQUEST_URI:
            return t = HttpParser::request_parser_type::REQUEST_PROTOCOL;

        case HttpParser::request_parser_type::REQUEST_PROTOCOL:
            return t = HttpParser::request_parser_type::REQUEST_VERSION_MAJOR;

        case HttpParser::request_parser_type::REQUEST_VERSION_MAJOR:
            return t = HttpParser::request_parser_type::REQUEST_VERSION_MINOR;

        case HttpParser::request_parser_type::REQUEST_VERSION_MINOR:
            return t = HttpParser::request_parser_type::REQUEST_KEY;

        case HttpParser::request_parser_type::REQUEST_KEY:
            return t = HttpParser::request_parser_type::REQUEST_VALUE;

        case HttpParser::request_parser_type::REQUEST_VALUE:
            return t = HttpParser::request_parser_type::REQUEST_END;

        default:
            return t = HttpParser::request_parser_type::REQUEST_END;
        }
    }
	enum class response_parser_type {
		RESPONSE_PROTOCOL, RESPONSE_VERSION_MAJOR, RESPONSE_VERSION_MINOR, RESPONSE_STATUS,
		RESPONSE_STATUS_TEXT, RESPONSE_KEY, RESPONSE_VALUE, RESPONSE_END
	};
    friend HttpParser::response_parser_type & operator ++ ( HttpParser::response_parser_type & t ) {
        switch ( t ) {
        case HttpParser::response_parser_type::RESPONSE_PROTOCOL:
            return t = HttpParser::response_parser_type::RESPONSE_VERSION_MAJOR;

        case HttpParser::response_parser_type::RESPONSE_VERSION_MAJOR:
            return t = HttpParser::response_parser_type::RESPONSE_VERSION_MINOR;

        case HttpParser::response_parser_type::RESPONSE_VERSION_MINOR:
            return t = HttpParser::response_parser_type::RESPONSE_STATUS;

        case HttpParser::response_parser_type::RESPONSE_STATUS:
            return t = HttpParser::response_parser_type::RESPONSE_STATUS_TEXT;

        case HttpParser::response_parser_type::RESPONSE_STATUS_TEXT:
            return t = HttpParser::response_parser_type::RESPONSE_KEY;

        case HttpParser::response_parser_type::RESPONSE_KEY:
            return t = HttpParser::response_parser_type::RESPONSE_VALUE;

        case HttpParser::response_parser_type::RESPONSE_VALUE:
            return t = HttpParser::response_parser_type::RESPONSE_END;

        default:
            return t = HttpParser::response_parser_type::RESPONSE_END;
        }
    }

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
    static size_t parse_request_status_line ( RequestParserState & parser_state, http::Request & request, buffer_t & buffer, size_t start, size_t end ) {
        parser_state.start_pos_ = start;

        for ( size_t i = start; i<end; ++i ) {
            if ( parser_state.request_type_ == request_parser_type::REQUEST_METHOD &&  buffer[i] == ' ' ) {
                request.method ( combine_string ( parser_state, buffer, i ) );
                parser_state.start_pos_ = i + 1;
                ++ parser_state.request_type_;

            } else if ( parser_state.request_type_ == request_parser_type::REQUEST_URI &&  buffer[i] == ' ' ) {
                std::string _value = combine_string ( parser_state, buffer, i );
                size_t qmPosition = _value.find ( "?" );

                if ( qmPosition != std::string::npos ) {
                    request.uri ( _value.substr ( 0, qmPosition ) );
                    get_attributes ( _value.substr ( qmPosition + 1 ), request );

                } else {
                    std::string _uri;
                    url_decode ( _value, _uri );
                    request.uri ( _uri );
                }

                parser_state.start_pos_ = i + 1;
                ++ parser_state.request_type_;

            } else if ( parser_state.request_type_ == request_parser_type::REQUEST_PROTOCOL &&  buffer[i] == '/' ) {
                request.protocol ( combine_string ( parser_state, buffer, i ) );
                parser_state.start_pos_ = i + 1;
                ++ parser_state.request_type_;

            } else if ( parser_state.request_type_ == request_parser_type::REQUEST_VERSION_MAJOR &&  buffer[i] == '.' ) {
                request.version_major ( std::stoi ( combine_string ( parser_state, buffer, i ) ) );
                parser_state.start_pos_ = i + 1;
                ++ parser_state.request_type_;

            } else if ( parser_state.request_type_ == request_parser_type::REQUEST_VERSION_MINOR &&  buffer[i] == '\r' ) {
                request.version_minor ( std::stoi ( combine_string ( parser_state, buffer, i ) ) );
                parser_state.start_pos_ = i + 1;
                ++ parser_state.request_type_;

            } else if ( parser_state.request_type_ == request_parser_type::REQUEST_KEY &&  buffer[i] == '\n' ) {
                return i + 1;
            }
        }

        parser_state.temp_buffer_.push_back (
            std::string ( buffer.data(), parser_state.start_pos_, end - parser_state.start_pos_ ) );
        return 0;
    }
    static size_t parse_response_status_line ( RequestParserState & parser_state, http::Response & response, buffer_t & buffer, size_t start, size_t end ) {
        parser_state.start_pos_ = start;

        for ( size_t i = start; i<end; ++i ) {
            if ( parser_state.response_type_ == response_parser_type::RESPONSE_PROTOCOL &&  buffer[i] == '/' ) {
                response.protocol ( combine_string ( parser_state, buffer, i ) );
                parser_state.start_pos_ = i + 1;
                ++ parser_state.response_type_;

            } else if ( parser_state.response_type_ == response_parser_type::RESPONSE_VERSION_MAJOR && buffer[i] == '.' ) {
                std::string _value = combine_string ( parser_state, buffer, i );
                response.version_major ( std::stoi ( _value ) );
                parser_state.start_pos_ = i + 1;
                ++ parser_state.response_type_;

            } else if ( parser_state.response_type_ == response_parser_type::RESPONSE_VERSION_MINOR && buffer[i] == ' ' ) {
                std::string _value = combine_string ( parser_state, buffer, i );
                response.version_minor ( std::stoi ( _value ) );
                parser_state.start_pos_ = i + 1;
                ++ parser_state.response_type_;

            } else if ( parser_state.response_type_ == response_parser_type::RESPONSE_STATUS &&  buffer[i] == ' ' ) {
                response.status ( static_cast< http_status > ( std::stoi ( combine_string ( parser_state, buffer, i ) ) ) );
                parser_state.start_pos_ = i + 1;
                ++ parser_state.response_type_;

            } else if ( parser_state.response_type_ == response_parser_type::RESPONSE_STATUS_TEXT &&  buffer[i] == '\r' ) {
                parser_state.start_pos_ = i + 1;
                ++ parser_state.response_type_;

            } else if ( parser_state.response_type_ == response_parser_type::RESPONSE_KEY &&  buffer[i] == '\n' ) {
                return i + 1;
            }
        }

        parser_state.temp_buffer_.push_back (
            std::string ( buffer.data(), parser_state.start_pos_, end - parser_state.start_pos_ ) );
        return 0;
    }
    static size_t parse_parameter ( RequestParserState & parser_state, http::Request & request, buffer_t & buffer, size_t start, size_t end ) {
        parser_state.start_pos_ = start;

        for ( size_t i = start; i<end; ++i ) {
            if ( parser_state.request_type_ == request_parser_type::REQUEST_KEY &&  buffer[i] == ':' ) {
                parser_state.key_ = combine_string ( parser_state, buffer, i );
                parser_state.start_pos_ = i + 1;
                ++ parser_state.request_type_;
                parser_state.line_breaks_ = 0;

            } else if ( parser_state.request_type_ == request_parser_type::REQUEST_VALUE &&  buffer[i] == '\r' ) {
                std::string _value = combine_string ( parser_state, buffer, i );
                request.parameter ( parser_state.key_, trim ( _value  ) );
                parser_state.start_pos_ = i + 1;
                parser_state.request_type_ = request_parser_type::REQUEST_KEY;

            } else if ( parser_state.request_type_ == request_parser_type::REQUEST_KEY &&  buffer[i] == '\n' ) {
                parser_state.start_pos_ = i + 1;

                if ( parser_state.line_breaks_ == 1 ) {
                    parser_state.request_type_ = request_parser_type::REQUEST_END;
                    return i + 1;

                } else {
                    parser_state.line_breaks_ = 1;
                }
            }
        }

        parser_state.temp_buffer_.push_back (
            std::string ( buffer.data(), parser_state.start_pos_, end - parser_state.start_pos_ ) );
        return 0;
    }
    static size_t parse_parameter ( RequestParserState & parser_state, http::Response & response, buffer_t & buffer, size_t start, size_t end ) {
        parser_state.start_pos_ = start;

        for ( size_t i = start; i<end; ++i ) {
            if ( parser_state.response_type_ == response_parser_type::RESPONSE_KEY &&  buffer[i] == ':' ) {
                parser_state.key_ = combine_string ( parser_state, buffer, i );
                parser_state.start_pos_ = i + 1;
                ++ parser_state.response_type_;
                parser_state.line_breaks_ = 0;

            } else if ( parser_state.response_type_ == response_parser_type::RESPONSE_VALUE &&  buffer[i] == '\r' ) {
                std::string _value = combine_string ( parser_state, buffer, i );
                response.parameter ( parser_state.key_, trim ( _value  ) );
                parser_state.start_pos_ = i + 1;
                parser_state.response_type_ = response_parser_type::RESPONSE_KEY;

            } else if ( parser_state.response_type_ == response_parser_type::RESPONSE_KEY &&  buffer[i] == '\n' ) {
                parser_state.start_pos_ = i + 1;

                if ( parser_state.line_breaks_ == 1 ) {
                    parser_state.response_type_ = response_parser_type::RESPONSE_END;
                    return i + 1;

                } else {
                    parser_state.line_breaks_ = 1;
                }
            }
        }

        parser_state.temp_buffer_.push_back (
            std::string ( buffer.data(), parser_state.start_pos_, end - parser_state.start_pos_ ) );
        return 0;
    }
    static size_t parse_body_form_data ( RequestParserState & parser_state, http::Request & request, buffer_t & buffer, size_t start, size_t end ) {
        if ( std::stoul ( request.parameter ( http::header::CONTENT_LENGTH ) ) >= start - end ) {
            get_attributes ( std::string ( buffer.data(), start, start +  std::stoul ( request.parameter ( http::header::CONTENT_LENGTH ) ) ), request );
            return start + std::stoul ( request.parameter ( http::header::CONTENT_LENGTH ) );

        } else {
            std::string _new_string = std::string ( buffer.data(), start, end - start );
            parser_state.temp_buffer_.push_back ( _new_string );
            parser_state.combined_string_size_ += _new_string.size();

            if ( std::stoul ( request.parameter ( http::header::CONTENT_LENGTH ) ) == parser_state.combined_string_size_ ) {
                get_attributes ( parser_state.get_buffer ( "" ), request );
                return end;

            } else {
                return 0;
            }
        }
    }

    /**
     * @brief Get parameters from request line and write them to the request.
     * @param parameters the request line string.
     * @param request the request where to store the parameters.
     */
    static void get_attributes ( const std::string & parameters, Request & request ) {
        enum parse_mode { KEY, VALUE } _mode = KEY;
        std::stringstream _ss_buffer_key;
        std::stringstream _ss_buffer_value;

        for ( size_t i = 0; i < parameters.length(); ++i ) {
            if ( parameters[i] == '=' ) {
                _mode = VALUE;

            } else if ( parameters[i] == '&' ) {
                std::string value;
                url_decode ( _ss_buffer_value.str() , value );
                request.attribute ( _ss_buffer_key.str(), value );
                _ss_buffer_key.str ( std::string() );
                _ss_buffer_key.clear();
                _ss_buffer_value.str ( std::string() );
                _ss_buffer_value.clear();
                _mode = KEY;

            } else if ( _mode == KEY ) {
                _ss_buffer_key << parameters[i];

            } else {
                _ss_buffer_value << parameters[i];
            }
        }

        if ( _mode == VALUE ) {
            std::string _value;
            url_decode ( _ss_buffer_value.str() , _value );
            request.attribute ( _ss_buffer_key.str(), _value );
        }
    }

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
    ///@endcond DOC_INTERNAL
};
}//namespace utils
}//namespace http
#endif // HTTPPARSER_H
