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
#ifndef HTTPREQUEST
#define HTTPREQUEST

#include <map>
#include <memory>
#include <list>
#include <string>
#include <sstream>
#include <vector>

#include "httpconfig.h"
#include "utils/stringutils.h"

namespace http {

/**
 * @brief Http Request class.
 */
class HttpRequest {
public:
	HttpRequest();
	HttpRequest ( const std::string & path );

	HttpRequest ( const HttpRequest& ) = delete;
	HttpRequest ( HttpRequest&& ) = delete;
	HttpRequest& operator= ( const HttpRequest& ) = delete;
	HttpRequest& operator= ( HttpRequest&& ) = delete;
	~HttpRequest() {}

	/** @brief Set a request parameter. */
	void parameter ( const std::string & name, const std::string & value );
	/** @brief Get a request paremeter */
	std::string parameter ( const std::string & name );
	/** @brief Contains request parameter by key. */
	bool contains_parameter ( const std::string & name );
	/** @brief Get the request parameter names. */
	std::vector<std::string> parameter_names() const;
	/** @brief Get a copy of the request parameter map. */
	std::map< std::string, std::string, utils::KeyICompare > parameterMap() const;
	/** @brief Set a request attribute. */
	void attribute ( const std::string & name, const std::string & value );
	/** @brief Get a request attribute */
	std::string attribute ( const std::string & name );
	/** @brief Contains request attribute by key. */
	bool contains_attribute ( const std::string & name );
	/** @brief Get the request attribute names. */
	std::list<std::string> attributeNames() const;
	/** @brief Get a copy of the request parameter map. */
	std::map< std::string, std::string, utils::KeyICompare > attributeMap() const;

	/** @brief Set the request method. */
	void method ( const std::string & method );
	/** @brief Get the request method. */
	std::string method() const;
	/** @brief Set the request protocol. */
	void protocol ( const std::string & protocol );
	/** @brief Get the request protocol. */
	std::string protocol() const;
	/** @brief Get the request path. */
	std::string uri() const;
	/** @brief Set the request path. */
	void uri ( const std::string & uri );
	/** @brief Set the http major version. */
	void version_major ( const int & http_version_major );
	/** @brief Get the http major version. */
	int version_major() const;
	/** @brief Set the http minor version. */
	void version_minor ( const int & http_version_minor );
	/** @brief Get the http minor version. */
	int version_minor() const;
	/** @brief Get the remote IP. */
	std::string remote_ip() const;
	/** @brief Set the remote IP. */
	void remote_ip ( const std::string & remote_ip );
	/** @brief Persistent connection. */
	bool persistent();
	/** @brief Set persistend connection. */
	void persistent ( bool persistent );


	/**
	 * @brief write the header to the buffer.
	 * @param buffer
	 * @return
	 */
	size_t header ( buffer_t & buffer );
	/**
	 * @brief content write content to the buffer.
	 * @param buffer
	 * @param index
	 * @param count
	 */
	void content ( buffer_t & buffer, const size_t & index, const size_t & count );

	/** TODO
	 * @brief Get the request body.
	 * @return
	 */
	const std::string requestBody() const;

	template< class T >
	/**
	 * @brief operator <<
	 * @param in
	 */
	void operator<< ( const T & in ) {
		( *std::dynamic_pointer_cast<std::stringstream> ( out_body_ ) ) << in;
	}

	/**
	 * @brief output the request as string.
	 * @param out
	 * @param request
	 * @return
	 */
	friend std::ostream& operator<< ( std::ostream& out, const http::HttpRequest & request );

private:
	std::string method_, uri_, protocol_, remote_ip_;
	size_t body_size_;
	short http_version_major_, http_version_minor_;

	std::map< std::string, std::string, utils::KeyICompare > parameters_;
	std::map< std::string, std::string, utils::KeyICompare > attributes_;

	std::shared_ptr< std::istream > out_body_;
};

} //http
#endif // HTTPREQUEST

