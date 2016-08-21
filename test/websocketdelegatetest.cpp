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

#include <string>
#include <fstream>

#include "../src/utils/base64.h"
#include "../src/httpserver.h"
#include "../src/websocketdelegate.h"

#include <gtest/gtest.h>

namespace http {
namespace delegate {

TEST ( WebSocketDelegateTest, SHA1 ) {
	std::vector< std::string > _ws_protocols ( { "chat" } );
	WebSocketDelegate ws_ ( _ws_protocols );
	std::string request_key_ = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	std::array< unsigned char, SHA_DIGEST_LENGTH > result_ = { 0xb3, 0x7a, 0x4f, 0x2c, 0xc0, 0x62, 0x4f, 0x16, 0x90, 0xf6,
															   0x46, 0x06, 0xcf, 0x38, 0x59, 0x45, 0xb2, 0xbe, 0xc4, 0xea
															 };
	EXPECT_EQ ( result_, ws_.sha1 ( reinterpret_cast< const unsigned char *> ( request_key_.c_str() ), request_key_.size() ) );
}

TEST ( WebSocketDelegateTest, Base64 ) {
	std::array< unsigned char, SHA_DIGEST_LENGTH > key_ = { 0xb3, 0x7a, 0x4f, 0x2c, 0xc0, 0x62, 0x4f, 0x16, 0x90, 0xf6,
															0x46, 0x06, 0xcf, 0x38, 0x59, 0x45, 0xb2, 0xbe, 0xc4, 0xea
														  };
	EXPECT_EQ ( "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=", base64_encode ( key_.data(), SHA_DIGEST_LENGTH ) );
}

TEST ( WebSocketDelegateTest, CalculateKey ) {
	std::vector< std::string > _ws_protocols ( { "chat" } );
	WebSocketDelegate ws_ ( _ws_protocols );
	std::string request_key_ = "dGhlIHNhbXBsZSBub25jZQ==";
	std::string response_key_ = ws_.calculate_key ( request_key_ );
	EXPECT_EQ ( "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=", response_key_ );
}
}//namespace delegate
}//namespace http
