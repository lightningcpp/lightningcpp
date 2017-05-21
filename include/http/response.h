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

#include "constant.h"
#include "mimetypes.h"
#include "utils/stringutils.h"

namespace http {

/**
 * @brief Response class
 */
class Response {
public:
    /**
     * @brief Response CTOR.
     */
    Response() : body_ostream_( std::make_unique< std::stringstream >() ) {}
    Response ( const Response& ) = delete;
    Response ( Response&& ) = default;
    Response& operator= ( const Response& ) = delete;
    Response& operator= ( Response&& ) = default;
    ~Response() {}

	/** @brief Set value for the key. */
	void parameter ( const std::string & key, const std::string & value )
	{ parameters_[key] = value; }
	/** @brief Get parameter by key. */
    std::string parameter ( const std::string & key ) const {
        if( parameters_.find( key ) == parameters_.end() ) return "";
        else return parameters_.at( key );
    }
	/** @brief Check if response contains parameter. */
    bool contains_parameter ( const std::string & key ) const
	{ return ( parameters_.find ( key ) != parameters_.end() ); }
    /** @brief parameter size. */
    size_t parameter_size () const { return parameters_.size(); }
    /** @brief remove parameter. */
    void erase ( const std::string& key ) { parameters_.erase( key ); }
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
    size_t header ( char* buffer, std::streamsize size ) {
        // create status line: Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
        int _position = snprintf ( buffer, size, "%s/%d.%d %d %s\r\n",
            protocol_.c_str(), version_major_, version_minor_, static_cast< int > ( status_ ), status_reason_phrases.at( status_ ).c_str() );

        if ( _position < 0 && _position > size ) { throw http_status::INTERNAL_SERVER_ERROR; }

        for ( auto & _header : parameters_ ) {
            _position += snprintf ( buffer+_position, size-_position, "%s: %s\r\n", _header.first.c_str(), _header.second.c_str() );

            if ( _position < 0 && _position > size ) { throw http_status::INTERNAL_SERVER_ERROR; }
        }

        _position += snprintf ( buffer+_position, size-_position, "\r\n" );
        return _position;
    }

    auto tellp()
    { return body_ostream_->tellp(); }

    auto tellg()
    { return (  body_istream_ != nullptr ? body_istream_->tellg() : body_ostream_->tellg() ); }

    auto read( char* buffer, std::streamsize size ) {
        if( body_istream_ ) {
            auto _read_size = body_istream_->readsome( buffer, size );
            //TODO std::cout << "Response: read some size: " << _read_size << std::endl;
            return _read_size;
        } else {
            return body_ostream_->readsome( buffer, size ); //TODO not some
        }
    }

    void write( char* buffer, std::streamsize size )
    { body_ostream_->write( buffer, size ); }

    void istream ( std::unique_ptr< std::istream > && is )
    { body_istream_ = std::move ( is ); }

    /**
     * @brief get the ostream as string.
     * @return
     */
    std::string str() const
    { return body_ostream_->str(); }

	/**
	 * @brief write to the buffer
	 * @param value
	 * @return
	 */
    template< class T >
    Response & operator<< ( const T & value ) {
        *(body_ostream_) << value;
		return *this;
	}
	/* ------------------------------------------------------------------------------------------------------------------ */


	/** @brief output this request as string. */
    friend std::ostream& operator<< ( std::ostream& out, const http::Response & response ) {
		out << response.protocol_ << "/" << response.version_major_ << "." <<
            response.version_minor_ << " " << static_cast< int > ( response.status_ ) << status_reason_phrases.at( response.status_ ) <<
			"\n" << "Parameters:\n";

		for ( auto response_line : response.parameters_ ) {
			out << "\t" << response_line.first << ": " << response_line.second << "\n";
		}

		return out;
	}

    void reset() {
        parameters_.clear();
        if ( body_istream_ ) {
            body_istream_.reset();
        }
        if ( body_ostream_ ) {
            body_ostream_->str( "" );
            body_ostream_->clear();
        }
    }

private:
	http_status status_ = http_status::OK;
	short version_major_ = 1;
	short version_minor_ = 1;
	int expires_ = 0;
	size_t size_ = 0;
	time_t last_modified_;
	std::string protocol_ = "HTTP";
	std::string remote_ip_;

    std::map< std::string, std::string, utils::KeyICompare > parameters_;
    std::unique_ptr< std::stringstream > body_ostream_;
    std::unique_ptr< std::istream > body_istream_;
};
} //namespace http
#endif //HTTPRESPONSE_H
