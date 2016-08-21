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

#include "httputils.h"
#include "mimetypes.h"

#include <gtest/gtest_prod.h>

namespace http {

class HttpRequest;

class DefaultParameter {
public:
	static void execute ( auto&, auto & response ) {
		if ( ! response.contains_parameter ( header::CONTENT_LENGTH ) ) {
			response.parameter ( header::CONTENT_LENGTH,  std::to_string ( response.size() ) );

		}

		//TODO add HOST,

		//add expiration date
		if ( response.expires() ) {
			time_t now = time ( nullptr );
			struct tm then_tm = *gmtime ( &now );
			then_tm.tm_sec += response.expires();
			mktime ( &then_tm );
			response.parameter ( header::EXPIRES, http::utils::time_to_string ( &then_tm ) );
		}

		//add now
		time_t now = time ( nullptr );
		struct tm now_tm = *gmtime ( &now );
		mktime ( &now_tm );
		response.parameter ( header::DATE, http::utils::time_to_string ( &now_tm ) );

		//add mime-type
		// ss << header::CONTENT_TYPE << ": " << parameters_[header::CONTENT_TYPE] << "\r\n";
	}
};

class EmptyParameter {
public:
	static void execute ( auto&, auto& ) {}
};

/**
 * @brief The HttpResponse class
 * <p>the http response. This class is not copyable only movable.</p>
 */
class HttpResponse {
public:
//	enum RESPONSE_STATE { NONE, CONNECTING, READY, COMPLETE, INCOMPLETE } state = NONE;

	HttpResponse() {}
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

	/** @brief reset the stream and header map. */
	void reset ();
	/** @brief The body of this response. The caller controlles the lifetime. */


	/* stream realated methods */

	/**
	 * @brief Fill buffer with the header. The complete header must fit into the buffer.
	 * @param buffer
	 * @return
	 */
	size_t header ( buffer_t & buffer );

	/* ------------------------------------------------------------------------------------------------------------------ */


	void istream ( std::unique_ptr< std::istream > && is );

	/**
	 * @brief read the content and store in buffer
	 * @param buffer array
	 * @return size of content in the buffer
	 */
	size_t read ( buffer_t & buffer );

	/* ------------------------------------------------------------------------------------------------------------------ */


	template< class T >
	/**
	 * @brief write to the buffer
	 * @param value
	 * @return
	 */
	HttpResponse & operator<< ( const T & value ) {
		body_ostream_ << value;
		return *this;
	}

	/** @brief the size of the response body. */
	size_t size () { return body_ostream_.tellp(); }

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

	std::stringstream body_ostream_;
	std::unique_ptr< std::istream > body_istream_;
};
/**
 * the server delegate method.
 */
typedef std::function< std::shared_ptr< HttpResponse > ( buffer_t, size_t size ) > server_delegate_t;
} //namespace http
#endif //HTTPRESPONSE_H
