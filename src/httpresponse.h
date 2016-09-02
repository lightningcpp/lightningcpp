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
#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <array>
#include <map>
#include <memory>
#include <sstream>

#include "httpconfig.h"
#include "mimetypes.h"

#include <gtest/gtest_prod.h>

namespace http {

/**
 * @brief The HttpResponse class
 * <p>the http response. This class is not copyable only movable.</p>
 */
class HttpResponse {
public:

    HttpResponse() : body_ostream_( std::make_unique< std::stringstream >() ) {}
	HttpResponse ( const HttpResponse& ) = delete;
	HttpResponse ( HttpResponse&& ) = default;
	HttpResponse& operator= ( const HttpResponse& ) = delete;
	HttpResponse& operator= ( HttpResponse&& ) = default;
	~HttpResponse() {}

	/** @brief Set value for the key. */
	void parameter ( const std::string & key, const std::string & value )
	{ parameters_[key] = value; }
	/** @brief Get parameter by key. */
	std::string parameter ( const std::string & key )
	{ return parameters_[key]; }
	/** @brief Check if response contains parameter. */
	bool contains_parameter ( const std::string & key )
	{ return ( parameters_.find ( key ) != parameters_.end() ); }
    /** @brief parameter size. */
    size_t parameter_size () { return parameters_.size(); }
	/** @brief expire time in seconds. if not set no header will be added. */
	void expires ( int seconds ) { expires_ = seconds; }
	/** @brief expire time in seconds. if not set no header will be added. */
	int expires () const { return expires_; }
	/** @brief the http major version. */
	void version_major ( short http_version_major ) { version_major_ = http_version_major; }
	/** @brief the the http major version. */
	short version_major () const { return version_major_; }
	/** @brief the http minor version. */
	void version_minor ( short http_version_minor ) { version_minor_ = http_version_minor; }
	/** @brief the the http minor version. */
	short version_minor () const { return version_minor_; }
	/** @brief Last modified date. if not set no header will be added. */
	void last_modified ( time_t datetime ) { last_modified_ = datetime; }
	/** @brief Last modified date. if not set no header will be added. */
	time_t last_modified () const { return last_modified_; }
	/** @brief the http protocol. */
	void protocol ( const std::string & protocol ) { protocol_ = protocol; }
	/** @brief the the http protocol. */
	std::string protocol () const { return protocol_; }
	/** @brief the remote IP address. */
	void remote_ip ( const std::string & remote_ip ) { remote_ip_ = remote_ip; }
	/** @brief the remote IP address. */
	std::string remote_ip () const { return remote_ip_; }
	/** @brief the buffer read status of this request. */
	void status ( http_status status ) { status_ = status; }
	/** @brief the buffer read status of this request. */
	http_status status () const { return status_; }

    /* ------------------------------------------------------------------------------------------------------------------ */
    /*                                            stream realated methods                                                 */
    /* ------------------------------------------------------------------------------------------------------------------ */

    /**
     * @brief Fill buffer with the header. The complete header must fit into the buffer.
     * @param buffer
     * @return
     */
    size_t header ( buffer_t & buffer ) {
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

    auto tellp()
    { return body_ostream_->tellp(); }

    auto tellg()
    { return (  body_istream_ != nullptr ? body_istream_->tellg() : body_ostream_->tellg() ); }

    auto read( buffer_t & buffer ) {
        if( body_istream_ ) {
            return body_istream_->readsome( buffer.data(), BUFFER_SIZE );
        } else {
            return body_ostream_->readsome( buffer.data(), BUFFER_SIZE );
        }
    }

    void write( buffer_t & buffer, size_t index, size_t size )
    { body_ostream_->write( buffer.data()+index, size ); }

    void istream ( std::unique_ptr< std::istream > && is )
    { body_istream_ = std::move ( is ); }

	/**
	 * @brief write to the buffer
	 * @param value
	 * @return
	 */
    template< class T >
    HttpResponse & operator<< ( const T & value ) {
        *(body_ostream_) << value;
		return *this;
	}
	/* ------------------------------------------------------------------------------------------------------------------ */


	/** @brief output this request as string. */
	friend std::ostream& operator<< ( std::ostream& out, const http::HttpResponse & response ) {
		out << response.protocol_ << "/" << response.version_major_ << "." <<
			response.version_minor_ << " " << static_cast< int > ( response.status_ ) << status_reason_phrase ( response.status_ ) <<
			"\n" << "Parameters:\n";

		for ( auto response_line : response.parameters_ ) {
			out << "\t" << response_line.first << ": " << response_line.second << "\n";
		}

		return out;
	}

private:
	FRIEND_TEST ( HttpServerTest, TestResponse );

	http_status status_ = http_status::OK;
	short version_major_ = 1;
	short version_minor_ = 1;
	int expires_ = 0;
	size_t size_ = 0;
	time_t last_modified_;
	std::string protocol_ = "HTTP";
	std::string remote_ip_;

	std::map<std::string, std::string> parameters_;
    std::unique_ptr< std::stringstream > body_ostream_;
    std::unique_ptr< std::istream > body_istream_;
};
} //namespace http
#endif //HTTPRESPONSE_H
