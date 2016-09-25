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
#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

namespace http {
namespace utils {

struct KeyICompare : public std::binary_function< std::string, std::string, bool> {
	bool operator() ( const std::string & lhs, const std::string &rhs ) const {
		return strcasecmp ( lhs.c_str(), rhs.c_str() ) < 0 ;
	}
};

inline std::string trim ( const std::string & str ) {
	std::string my_string = str;
	my_string.erase ( 0, my_string.find_first_not_of ( ' ' ) );
	my_string.erase ( my_string.find_last_not_of ( ' ' ) + 1 );
	return str;
}

inline std::string & trim ( std::string & str ) {
	str.erase ( 0, str.find_first_not_of ( ' ' ) );
	str.erase ( str.find_last_not_of ( ' ' ) + 1 );
	return str;
}
/**
 * URL decode the in string to out string.
 * @brief url_decode
 * @param in the url encoded string.
 * @param out the output string.
 * @return false when the string can not be deocoded.
 */
inline bool url_decode ( const std::string & in, std::string & out ) {
	out.clear();
	out.reserve ( in.size() );

	for ( std::size_t i = 0; i < in.size(); ++i ) {
		if ( in[i] == '%' ) {
			if ( i + 3 <= in.size() ) {
				int value = 0;
				std::istringstream is ( in.substr ( i + 1, 2 ) );

				if ( is >> std::hex >> value ) {
					out += static_cast<char> ( value );
					i += 2;

				} else {
					return false;
				}

			} else {
				return false;
			}

		} else if ( in[i] == '+' ) {
			out += ' ';

		} else {
			out += in[i];
		}
	}

	return true;
}

class UrlEscape {
private:
	static std::string charToHex ( unsigned char c ) {
		short i = c;

		std::stringstream s;

		s << "%" << std::setw ( 2 ) << std::setfill ( '0' ) << std::hex << std::uppercase << i;

		return s.str();
	}
public:
	static std::string urlEncode ( const std::string &toEncode ) {
		std::ostringstream out;

		for ( std::string::size_type i=0; i < toEncode.length(); ++i ) {
			short t = toEncode.at ( i );

			if ( t == 45 ||         // hyphen
					( t >= 48 && t <= 57 ) ||     // 0-9
					( t >= 65 && t <= 90 ) ||     // A-Z
					t == 95 ||          // underscore
					( t >= 97 && t <= 122 ) || // a-z
					t == 126            // tilde
			   ) {
				out << toEncode.at ( i );

			} else {
				out << charToHex ( t );
			}
		}

		return out.str();
	}
};

inline std::vector< std::string > parse_csv ( const std::string & input ) {
	std::vector< std::string > tokens;
	std::istringstream _iss ( input );

	while ( _iss.good() ) {
		std::string substr;
		std::getline ( _iss, substr, ',' );
		tokens.push_back ( trim ( substr ) );
	}

	return tokens;
}

/**
 * @brief tm to string
 * @param time
 * @return time string
 */
inline std::string time_to_string ( struct tm * time ) {
	char buffer_ [80];
	size_t size_ = strftime ( buffer_, 80, "%a, %d %b %Y %H:%M:%S %Z", time );
	return std::string ( buffer_, size_ );
}

/**
 * @brief time_t to string
 * @param time
 * @return time string
 */
inline std::string time_to_string ( time_t * time ) {
	struct tm *tminfo;
	tminfo = gmtime  ( time );
	return ( time_to_string ( tminfo ) );
}

/**
 * @brief parse time from string
 * @param time
 * @return
 */
inline std::time_t parse_time_string ( const std::string & time ) {

	std::tm t = {};
	std::istringstream ss ( time );
	ss.imbue ( std::locale ( "en_US.UTF8" ) );
	//RFC 822, RFC 1123
	ss >> std::get_time ( &t, "%a, %d %b %Y %H:%M:%S %Z" );

	if ( ss.fail() ) {
		//RFC 850, RFC 1036
		ss.clear();
		ss.seekg ( 0 );
		ss >> std::get_time ( &t, "%A, %d-%b-%y %H:%M:%S %Z" );

		if ( ss.fail() ) {
			//ANSI C's asctime
			ss.clear();
			ss.seekg ( 0 );
			ss >> std::get_time ( &t, "%a %b %d %H:%M:%S %Y" );

			if ( ss.fail() ) {
				//TODO error code and category
				throw std::system_error ( EDOM, std::system_category(), "wrong date format: " + time );
			}
		}
	}

	return ( mktime ( &t ) - timezone );
}

/**
 * @brief parse range
 * @param range
 * @return from to int values.
 */
inline std::tuple<int, int> parseRange ( const std::string & range ) {
	std::string clean_range = trim ( range );

	if ( clean_range.compare ( 1, 6, "bytes=" ) ) {
		clean_range = trim ( clean_range.substr ( 6 ) );

		if ( clean_range.find ( "-" ) != std::string::npos ) {
			int start = 0;
			int end = -1;
			std::string from = clean_range.substr ( 0, clean_range.find ( "-" ) );
			start = std::stoi ( from );

			if ( clean_range.find ( "-" ) + 1 != clean_range.size() ) {
				std::string to = clean_range.substr ( clean_range.find ( "-" ) + 1, clean_range.size() );
				end = std::stoi ( to );
			}

			return std::tuple<int, int> ( start, end );
		}
	}

	return std::tuple<int, int> ( 0, -1 );
}

}//namespace http
}//namespace utils
#endif // STRINGUTILS_H
