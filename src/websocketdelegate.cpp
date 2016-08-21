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

#include <math.h>

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include "utils/base64.h"

#include "websocketdelegate.h"

namespace http {
namespace delegate {

void WebSocketDelegate::execute ( HttpRequest & request, HttpResponse & response ) {
	std::cout << "WebSocket:" << request.uri() << std::endl;

	for ( auto & item : request.parameterMap() ) {
		std::cout << item.first << "= '" << item.second << "'" << std::endl;
	}

	if ( request.contains_parameter ( "Connection" ) && request.parameter ( "Connection" ) == "Upgrade" &&
			request.contains_parameter ( "Upgrade" ) && request.parameter ( "Upgrade" ) == "websocket" ) {
		std::cout << "WebSocket Request" << std::endl;
		response.status ( http_status::SWITCHING_PROTOCOL );
		response.parameter ( "Sec-WebSocket-Accept", calculate_key ( request.parameter ( "Sec-Websocket-Key" ) ) );
		response.parameter ( "Upgrade", "websocket" );
		response.parameter ( "Connection", "Upgrade" );

		if ( request.contains_parameter ( "Sec-WebSocket-Protocol" ) ) {
			for ( auto & __proto : utils::parse_csv ( request.parameter ( "Sec-WebSocket-Protocol" ) ) ) {
				if ( std::find ( protocols_.begin(), protocols_.end(), __proto ) != protocols_.end() ) {
					response.parameter ( "Sec-WebSocket-Protocol", __proto );
					break;
				}
			}

			if ( ! response.contains_parameter ( "Sec-WebSocket-Protocol" ) ) {
				std::cout << "no valid protocol found." << std::endl;
			}
		}

	} else {
		std::cout << "Regular Request" << std::endl;
	}
}

std::string WebSocketDelegate::calculate_key ( const std::string & request_key ) {
	std::stringstream ss_;
	ss_ << request_key << _key;
	std::string raw_key_ = ss_.str();

	const std::string key_string_ = ss_.str();
	std::array< unsigned char, SHA_DIGEST_LENGTH > result_ = sha1 ( reinterpret_cast< const unsigned char *> ( key_string_.c_str() ), key_string_.size() );
	return base64_encode ( result_.data(), SHA_DIGEST_LENGTH );
}
std::array< unsigned char, SHA_DIGEST_LENGTH > WebSocketDelegate::sha1 ( const unsigned char *input, int length ) {

	// unsigned char * hash_ = new unsigned char[SHA_DIGEST_LENGTH]; //as array<>
	std::array< unsigned char, SHA_DIGEST_LENGTH > hash_;
	SHA1 ( input, length, hash_.data() );
	return hash_;
}
}//namespace delegate
}//namespace http
