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
#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <vector>

#include <asio.hpp>

#include "../src/constant.h"

inline void client_header ( const std::string & host, const std::string & port, std::vector< asio::streambuf* > request, asio::streambuf & response ) {
	asio::io_service io_service;
	asio::ip::tcp::resolver resolver ( io_service );
	asio::ip::tcp::resolver::query query ( host, port );
	asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve ( query );

	asio::ip::tcp::socket socket ( io_service );
	asio::connect ( socket, endpoint_iterator );

	// Send the request.
	for ( auto r : request ) {
		asio::write ( socket, *r );
	}

	asio::read_until ( socket, response, "\r\n\r\n" );
}

inline bool compare_streams ( std::istream & r_stream, std::istream & l_stream ) {

    bool _result = true;
	char *lBuffer = new char[http::BUFFER_SIZE]();
	char *rBuffer = new char[http::BUFFER_SIZE]();

	do {
		l_stream.read ( lBuffer, http::BUFFER_SIZE );
		r_stream.read ( rBuffer, http::BUFFER_SIZE );

		if ( l_stream.tellg() != r_stream.tellg() ) {
                        _result = false;
		}

		if ( std::memcmp ( lBuffer, rBuffer, l_stream.gcount() ) != 0 ) {
                        _result = false;
		}
        } while ( _result && ( l_stream.good() || r_stream.good() ) );

        delete[] lBuffer;
        delete[] rBuffer;
        return _result;
}

#endif // TESTUTILS_H
