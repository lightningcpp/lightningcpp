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
#ifndef WEBSOCKETDELEGATE_H
#define WEBSOCKETDELEGATE_H

#include <string>
#include <vector>

#include <openssl/sha.h>

#include "httprequest.h"
#include "httpresponse.h"

#include <gtest/gtest_prod.h>

namespace http {
namespace delegate {

class WebSocketDelegate {
public:
	/**
	 * @brief WebSocketDelegate CTOR
	 */
	explicit WebSocketDelegate ( std::vector< std::string > & protocols ) : protocols_ ( protocols ) {}
	WebSocketDelegate ( const WebSocketDelegate& ) = delete;
	WebSocketDelegate ( WebSocketDelegate&& ) = delete;
	WebSocketDelegate& operator= ( const WebSocketDelegate& ) = delete;
	WebSocketDelegate& operator= ( WebSocketDelegate&& ) = delete;
	~WebSocketDelegate() {}

	/**
	 * @brief execute request
	 * @param request
	 * @param response
	 */
	void execute ( HttpRequest & request, HttpResponse & response );

private:
	const std::string _key = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	std::vector< std::string > protocols_;
	FRIEND_TEST ( WebSocketDelegateTest, CalculateKey );
	std::string calculate_key ( const std::string & request_key );
	FRIEND_TEST ( WebSocketDelegateTest, SHA1 );
	static std::array< unsigned char, SHA_DIGEST_LENGTH > sha1 ( const unsigned char *input, int length );
};
}//namespace delegate
}//namespace http
#endif // WEBSOCKETDELEGATE_H
