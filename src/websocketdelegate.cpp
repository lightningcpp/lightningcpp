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
