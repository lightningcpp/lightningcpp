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
#ifndef HTTP_H
#define HTTP_H

#include <memory>
#include <string>

#include "../constant.h"
#include "../request.h"
#include "../response.h"

namespace http {
namespace mod {

class Http  {
public:
    Http() {}
    Http ( const Http& ) = delete;
    Http ( Http&& ) = default;
    Http& operator= ( const Http& ) = delete;
    Http& operator= ( Http&& ) = default;
    ~Http() {}

    static http_status execute ( Request& request, Response& response ) {

        //set the status line
        response.version_major ( request.version_major() );
        response.version_minor ( request.version_minor() );
        response.protocol ( request.protocol() );

        //set content length
        if ( ! response.contains_parameter ( header::CONTENT_LENGTH ) ) {
            response.parameter ( header::CONTENT_LENGTH,  std::to_string ( response.tellp() ) );
        }

        //add expiration date
        if ( response.expires() ) {
            time_t now = time ( nullptr );
            struct tm then_tm = *gmtime ( &now );
            then_tm.tm_sec += response.expires();
            mktime ( &then_tm );
            response.parameter ( header::EXPIRES, http::utils::time_to_string ( &then_tm ) );
        }

        //add now
        time_t now = time ( nullptr );
        struct tm now_tm = *gmtime ( &now );
        mktime ( &now_tm );
        response.parameter ( header::DATE, http::utils::time_to_string ( &now_tm ) );

        //TODO more
        //add mime-type
        // ss << header::CONTENT_TYPE << ": " << parameters_[header::CONTENT_TYPE] << "\r\n";

        return http_status::OK;
    }
};
}//mod http
}//namespace http
#endif // HTTP_H
