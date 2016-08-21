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
#ifndef HTTPUTILS
#define HTTPUTILS

#include <iomanip>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include "httprequest.h"

//namespace http {
//namespace utils {

//inline std::string parse_ip ( const std::string& url ) {
//	if ( url.find ( "http://" ) == 0 ) {
//		size_t dot_pos = url.find_first_of ( ":", 7 );
//		size_t slash_pos = url.find_first_of ( "/", 7 );

//		if ( dot_pos == std::string::npos || dot_pos > slash_pos ) {
//			return url.substr ( 7, slash_pos - 7 );

//		} else {
//			return url.substr ( 7, dot_pos - 7 );
//		}

//	} else {
//		std::cout << "url does not start with http" << std::endl;
//		return "";
//	}
//}
//inline int parse_port ( const std::string & url ) {
//	size_t dot_pos = url.find_first_of ( ":", 7 );

//	if ( dot_pos != std::string::npos ) {
//		size_t slash_pos = url.find_first_of ( "/", 7 );

//		if ( slash_pos != std::string::npos ) {
//			return std::stoi ( url.substr ( dot_pos + 1, slash_pos - dot_pos ) );

//		} else {
//			return std::stoi ( url.substr ( dot_pos + 1, url.size() - dot_pos ) );
//		}

//	} else {
//		return 0;
//	}
//}
//inline std::string parse_path ( const std::string& url ) {
//	size_t slash_pos = url.find_first_of ( "/", 7 );

//	if ( slash_pos != std::string::npos ) {
//		return url.substr ( slash_pos, url.size() - slash_pos );

//	} else {
//		return "/";
//	}
//}

#endif // HTTPUTILS

