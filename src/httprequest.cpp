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

#include "httprequest.h"

namespace http  {

HttpRequest::HttpRequest ( const std::string & path ) :
	method_ ( std::string ( method::GET ) ), uri_ ( path ), protocol_ ( "HTTP" ), remote_ip_ ( std::string ( "" ) ),
	body_size_ ( 0 ), http_version_major_ ( 1 ), http_version_minor_ ( 1 ),
	parameters_ ( std::map< std::string, std::string, utils::KeyICompare >() ),
	attributes_ ( std::map< std::string, std::string, utils::KeyICompare >() ),
	out_body_ ( std::shared_ptr< std::istream > ( new std::stringstream() ) ) {}

HttpRequest::HttpRequest() :
	method_ ( std::string ( method::GET ) ), uri_ ( "" ), protocol_ ( "HTTP" ), remote_ip_ ( std::string ( "" ) ),
	body_size_ ( 0 ), http_version_major_ ( 1 ), http_version_minor_ ( 1 ),
	parameters_ ( std::map< std::string, std::string, utils::KeyICompare >() ),
	attributes_ ( std::map< std::string, std::string, utils::KeyICompare >() ),
	out_body_ ( std::shared_ptr< std::istream > ( new std::stringstream() ) ) {}

void HttpRequest::parameter ( const std::string & name, const std::string & value ) {
	parameters_[name] = value;
}
std::string HttpRequest::parameter ( const std::string & name ) {
	return parameters_[name];
}
bool HttpRequest::contains_parameter ( const std::string & name ) {
	return parameters_.find ( name ) != parameters_.end();
}
std::vector<std::string> HttpRequest::parameter_names() const {
	std::vector< std::string > result;

	for ( auto r : parameters_ )
	{ result.push_back ( r.first ); }

	return result;
}
std::map< std::string, std::string, utils::KeyICompare > HttpRequest::parameterMap() const {
	return parameters_;
}
void HttpRequest::attribute ( const std::string & name, const std::string & value ) {
	attributes_[name] = value;
}
std::string HttpRequest::attribute ( const std::string & name ) {
	return attributes_[name];
}
bool HttpRequest::contains_attribute ( const std::string & name ) {
	return attributes_.find ( name ) != attributes_.end();
}
std::list<std::string> HttpRequest::attributeNames() const {
	std::list< std::string > result;

	for ( auto r : attributes_ )
	{ result.push_back ( r.first ); }

	return result;
}
std::map< std::string, std::string, utils::KeyICompare > HttpRequest::attributeMap() const {
	return attributes_;
}


void HttpRequest::method ( const std::string & method ) {
	method_ = method;
}
std::string HttpRequest::method() const {
	return method_;
}
void HttpRequest::protocol ( const std::string & protocol ) {
	protocol_ = protocol;
}
std::string HttpRequest::protocol() const {
	return protocol_;
}
std::string HttpRequest::uri() const {
	return uri_;
}
void HttpRequest::uri ( const std::string & uri ) {
	uri_ = uri;
}
void HttpRequest::version_major ( const int & http_version_major ) {
	http_version_major_ = http_version_major;
}
int HttpRequest::version_major() const {
	return http_version_major_;
}
void HttpRequest::version_minor ( const int & http_version_minor ) {
	http_version_minor_ = http_version_minor;
}
int HttpRequest::version_minor() const {
	return http_version_minor_;
}
std::string HttpRequest::remote_ip() const {
	return remote_ip_;
}
void HttpRequest::remote_ip ( const std::string & remote_ip ) {
	remote_ip_ = remote_ip;
}
bool HttpRequest::persistent() {
	if ( parameters_[ header::CONNECTION ] == header::CONNECTION_KEEP_ALIVE ) {
		return true;

	} else if ( parameters_[ header::CONNECTION ] == header::CONNECTION_CLOSE ) {
		return false;

	} else if ( http_version_major_ == 1 && http_version_minor_ == 1 ) {
		return true;

	} else { return false; }
}
void HttpRequest::persistent ( bool persistent ) {
	if ( persistent )
	{ parameters_[header::CONNECTION] = header::CONNECTION_KEEP_ALIVE; }

	else
	{ parameters_[header::CONNECTION] = header::CONNECTION_CLOSE; }
}

const std::string HttpRequest::requestBody() const {
	return ( *std::dynamic_pointer_cast<std::stringstream> ( out_body_ ) ).str();
}


size_t HttpRequest::header ( buffer_t & buffer ) {
	// create status line
	int _position = snprintf ( buffer.data(), BUFFER_SIZE, "%s %s %s/%d.%d\r\n", method_.c_str(), uri_.c_str(), protocol_.c_str(), http_version_major_, http_version_minor_ );

	if ( _position < 0 && static_cast< size_t > ( _position ) > BUFFER_SIZE ) { throw http_status::INTERNAL_SERVER_ERROR; }

	for ( auto & _header : parameters_ ) {
		_position += snprintf ( buffer.data() +_position, BUFFER_SIZE-_position, "%s: %s\r\n", _header.first.c_str(), _header.second.c_str() );

		if ( _position < 0 && static_cast< size_t > ( _position ) > BUFFER_SIZE ) { throw http_status::INTERNAL_SERVER_ERROR; }
	}

	_position += snprintf ( buffer.data()+_position, BUFFER_SIZE-_position, "\r\n" );
	return _position;
}
void HttpRequest::content ( buffer_t & body, const size_t & index, const size_t & count ) {
	for ( size_t i = index; i < ( index + count ); i++ ) {
		( *std::dynamic_pointer_cast<std::stringstream> ( out_body_ ) ) << body[i];
	}

	body_size_ += count;

}

std::ostream& operator<< ( std::ostream& out, const http::HttpRequest & request ) {
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
}//namespace http
