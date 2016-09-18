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

#include "../src/mod/match.h"
#include "../src/mod/ws.h"
#include "../src/utils/base64.h"

#include <gtest/gtest.h>

namespace http {
namespace mod {

TEST ( ModWsTest, SHA1 ) {
    std::vector< std::string > _ws_protocols ( { "chat" } );
    WS ws_ ( _ws_protocols );
    std::string request_key_ = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::array< unsigned char, SHA_DIGEST_LENGTH > result_ = { 0xb3, 0x7a, 0x4f, 0x2c, 0xc0, 0x62, 0x4f, 0x16, 0x90, 0xf6,
                                                               0x46, 0x06, 0xcf, 0x38, 0x59, 0x45, 0xb2, 0xbe, 0xc4, 0xea
                                                             };
    EXPECT_EQ ( result_, _ws_utils::sha1 ( reinterpret_cast< const unsigned char *> ( request_key_.c_str() ), request_key_.size() ) );
}

TEST ( ModWsTest, Base64 ) {
    std::array< unsigned char, SHA_DIGEST_LENGTH > key_ = { 0xb3, 0x7a, 0x4f, 0x2c, 0xc0, 0x62, 0x4f, 0x16, 0x90, 0xf6,
                                                            0x46, 0x06, 0xcf, 0x38, 0x59, 0x45, 0xb2, 0xbe, 0xc4, 0xea
                                                          };
    EXPECT_EQ ( "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=", Base64::base64_encode ( key_.data(), SHA_DIGEST_LENGTH ) );
}

TEST ( WebSocketDelegateTest, CalculateKey ) {
    std::vector< std::string > _ws_protocols ( { "chat" } );
    WS ws_ ( _ws_protocols );
    std::string request_key_ = "dGhlIHNhbXBsZSBub25jZQ==";
    std::string response_key_ = _ws_utils::calculate_key ( request_key_ );
    EXPECT_EQ ( "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=", response_key_ );
}

TEST ( ModWsTest, TestCreate ) {
    //EXPECT_EQ( http::http_status::OK, match.execute ( _request, _response ) );
}
}//namespace mod
}//namespace http
