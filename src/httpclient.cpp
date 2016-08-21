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

#include "httpclient.h"

namespace http {

void HttpClient::connect() {
	// Get a list of endpoints corresponding to the server name and connect.
	asio::ip::tcp::resolver resolver ( io_service );
	asio::ip::tcp::resolver::query query ( host_, protocol_ );
	asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve ( query );
	asio::connect ( socket, endpoint_iterator );
}

void HttpClient::write ( HttpRequest & request ) {
	request.parameter ( header::HOST, host_ ); //TODO set headers in one place
	size_t _position = request.header ( buffer_ );
	//TODO check that all bytes are written
	/*size_t _written =*/ socket.write_some ( asio::buffer ( buffer_, _position ) );
}
}//namespace http
