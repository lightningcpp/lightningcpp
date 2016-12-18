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

#include "constant.h"
#include "utils/stringutils.h"

namespace http {

/**
 * @brief Request class.
 */
class Request {
public:

    /**
     * @brief Request CTOR
     */
    Request() :
        method_ ( std::string ( method::GET ) ), uri_ ( "" ), protocol_ ( "HTTP" ), remote_ip_ ( std::string ( "" ) ),
        body_size_ ( 0 ), http_version_major_ ( 1 ), http_version_minor_ ( 1 ),
        parameters_ ( std::map< std::string, std::string, utils::KeyICompare >() ),
        attributes_ ( std::map< std::string, std::string, utils::KeyICompare >() ),
        out_body_ ( std::shared_ptr< std::stringstream > ( new std::stringstream() ) ) {}

    /**
     * @brief Request CTOR
     * @param path the path for this request.
     */
    Request ( const std::string & path ) :
        method_ ( std::string ( method::GET ) ), uri_ ( path ), protocol_ ( "HTTP" ), remote_ip_ ( std::string ( "" ) ),
        body_size_ ( 0 ), http_version_major_ ( 1 ), http_version_minor_ ( 1 ),
        parameters_ ( std::map< std::string, std::string, utils::KeyICompare >() ),
        attributes_ ( std::map< std::string, std::string, utils::KeyICompare >() ),
        out_body_ ( std::shared_ptr< std::stringstream > ( new std::stringstream() ) ) {}

    Request ( const Request& ) = delete;
    Request ( Request&& ) = delete;
    Request& operator= ( const Request& ) = delete;
    Request& operator= ( Request&& ) = delete;
    ~Request() {}

	/** @brief Set a request parameter. */
    void parameter ( const std::string & name, const std::string & value )
    { parameters_[name] = value; }
	/** @brief Get a request paremeter */
    std::string parameter ( const std::string & name ) const {
        if( parameters_.find( name ) == parameters_.end() ) return "";
        else return parameters_.at( name );
    }
	/** @brief Contains request parameter by key. */
    bool contains_parameter ( const std::string & name ) const
    { return parameters_.find ( name ) != parameters_.end(); }
	/** @brief Get the request parameter names. */
    std::vector<std::string> parameter_names() const {
        std::vector< std::string > result;

        for ( auto r : parameters_ )
        { result.push_back ( r.first ); }

        return result;
    }
	/** @brief Get a copy of the request parameter map. */
    std::map< std::string, std::string, utils::KeyICompare > parameter_map() const
    { return parameters_; }

	/** @brief Set a request attribute. */
    void attribute ( const std::string & name, const std::string & value )
    { attributes_[name] = value; }
	/** @brief Get a request attribute */
    std::string attribute ( const std::string & name )
    { return attributes_[name]; }
	/** @brief Contains request attribute by key. */
    bool contains_attribute ( const std::string & name )
    { return attributes_.find ( name ) != attributes_.end(); }
	/** @brief Get the request attribute names. */
    std::list<std::string> attribute_names() const {
        std::list< std::string > result;

        for ( auto r : attributes_ )
        { result.push_back ( r.first ); }

        return result;
    }
	/** @brief Get a copy of the request parameter map. */
    std::map< std::string, std::string, utils::KeyICompare > attribute_map() const
    { return attributes_; }

	/** @brief Set the request method. */
    void method ( const std::string & method )
    { method_ = method; }
	/** @brief Get the request method. */
    std::string method() const
    { return method_; }
	/** @brief Set the request protocol. */
    void protocol ( const std::string & protocol )
    { protocol_ = protocol; }
	/** @brief Get the request protocol. */
    std::string protocol() const
    { return protocol_; }
	/** @brief Get the request path. */
    std::string uri() const
    { return uri_; }
	/** @brief Set the request path. */
    void uri ( const std::string & uri )
    { uri_ = uri; }
	/** @brief Set the http major version. */
    void version_major ( const int & http_version_major )
    { http_version_major_ = http_version_major; }
	/** @brief Get the http major version. */
    int version_major() const
    { return http_version_major_; }
	/** @brief Set the http minor version. */
    void version_minor ( const int & http_version_minor )
    { http_version_minor_ = http_version_minor; }
	/** @brief Get the http minor version. */
    int version_minor() const
    { return http_version_minor_; }
	/** @brief Get the remote IP. */
    std::string remote_ip() const
    { return remote_ip_; }
	/** @brief Set the remote IP. */
    void remote_ip ( const std::string & remote_ip )
    { remote_ip_ = remote_ip; }
	/** @brief Persistent connection. */
    bool persistent() {
        if ( parameters_[ header::CONNECTION ] == header::CONNECTION_KEEP_ALIVE ) {
            return true;

        } else if ( parameters_[ header::CONNECTION ] == header::CONNECTION_CLOSE ) {
            return false;

        } else if ( http_version_major_ == 1 && http_version_minor_ == 1 ) {
            return true;

        } else { return false; }
    }
	/** @brief Set persistend connection. */
    void persistent ( bool persistent ) {
        if ( persistent )
        { parameters_[header::CONNECTION] = header::CONNECTION_KEEP_ALIVE; }

        else
        { parameters_[header::CONNECTION] = header::CONNECTION_CLOSE; }
    }


    /* ------------------------------------------------------------------------------------------------------------------ */
    /*                                            stream realated methods                                                 */
    /* ------------------------------------------------------------------------------------------------------------------ */

	/**
	 * @brief write the header to the buffer.
	 * @param buffer
	 * @return
	 */
    std::streamsize header ( char* buffer, std::streamsize size ) {
        // create status line
        int _position = snprintf ( buffer, size, "%s %s %s/%d.%d\r\n", method_.c_str(), uri_.c_str(), protocol_.c_str(), http_version_major_, http_version_minor_ );

        if ( _position < 0 && _position > size ) { std::cout << "buffer to smal for header. "; return 0; }

        for ( auto & _header : parameters_ ) {
            _position += snprintf ( buffer+_position, size-_position, "%s: %s\r\n", _header.first.c_str(), _header.second.c_str() );

            if ( _position < 0 && _position > size ) { std::cout << "buffer to smal for header. "; return 0; }
        }
        _position += snprintf ( buffer+_position, size-_position, "\r\n" );
        return _position;
    }

	/**
	 * @brief content write content to the buffer.
	 * @param buffer
	 * @param index
	 * @param count
	 */
    void write ( char* buffer, std::streamsize size ) {
        out_body_->write( buffer, size );
    }

    auto tellp()
    { return out_body_->tellp(); }

    auto tellg()
    { return out_body_->tellg(); }

	/**
	 * @brief operator <<
	 * @param in
	 */
    template< class T >
    void operator<< ( const T & in ) {
        out_body_ << in;
    }

    std::string str() const
    { return ( out_body_->str() ); }

    /* ------------------------------------------------------------------------------------------------------------------ */


    /**
	 * @brief output the request as string.
	 * @param out
	 * @param request
	 * @return
	 */
    friend std::ostream& operator<< ( std::ostream& out, const Request & request ) {

        out << "http::HttpRequest: \n";
        out << request.method_ << " " << request.uri_ << " " << request.protocol_ << "/" << request.http_version_major_ << "." << request.http_version_minor_ << "\n";
        out << "Parameters:\n";

        for ( auto request_line : request.parameters_ ) {
            out << "\t" << request_line.first << ": " << request_line.second << "\n";
        }

        if ( request.attributes_.size() > 0 ) {
            out << "Attributes:\n";

            for ( auto parameter : request.attributes_ ) {
                out << "\t" << parameter.first << ": " << parameter.second << "\n";
            }
        }

        return out;
    }

    void reset() {
        parameters_.clear();
        attributes_.clear();
        if( out_body_ ) {
            out_body_->str();
            out_body_->clear();
        }
    }

private:
	std::string method_, uri_, protocol_, remote_ip_;
	size_t body_size_;
	short http_version_major_, http_version_minor_;

	std::map< std::string, std::string, utils::KeyICompare > parameters_;
	std::map< std::string, std::string, utils::KeyICompare > attributes_;

    std::shared_ptr< std::stringstream > out_body_;
};
} //http
#endif // HTTPREQUEST

