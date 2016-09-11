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
#ifndef ERROR_H
#define ERROR_H

#include <map>
#include <memory>
#include <string>

#include "../httpconfig.h"
#include "../request.h"
#include "../response.h"

#include "http.h"

namespace http {
namespace mod {

class Error : Http {
public:
    Error() {}
    Error ( const Error& ) = delete;
    Error ( Error&& ) = default;
    Error& operator= ( const Error& ) = delete;
    Error& operator= ( Error&& ) = default;
    ~Error() {}

    http_status execute ( Request& request, Response& response ) {
        if( _error_delegates.find( response.status() ) != _error_delegates.end() ) {
            response << _error_delegates[ response.status() ];
        }
        Http::execute( request, response );
        return response.status();
    }
private:
    std::map<http_status, std::string > _error_delegates {
        { http_status::BAD_GATEWAY, http::response::BAD_GATEWAY },
        { http_status::BAD_REQUEST, http::response::BAD_REQUEST },
        { http_status::CREATED, http::response::CREATED },
        { http_status::FORBIDDEN, http::response::FORBIDDEN },
        { http_status::INTERNAL_SERVER_ERROR, http::response::INTERNAL_SERVER_ERROR },
        { http_status::MOVED_PERMANENTLY, http::response::MOVED_PERMANENTLY },
        { http_status::MULTIPLE_CHOICES, http::response::MULTIPLE_CHOICES },
        { http_status::NOT_FOUND, http::response::NOT_FOUND },
        { http_status::NOT_IMPLEMENTED, http::response::NOT_IMPLEMENTED },
        { http_status::NOT_MODIFIED, http::response::NOT_MODIFIED },
        { http_status::NO_CONTENT, http::response::NO_CONTENT },
        { http_status::SERVICE_UNAVAILABLE, http::response::SERVICE_UNAVAILABLE },
        { http_status::UNAUTHORIZED, http::response::UNAUTHORIZED }
    };
};
}//mod http
}//namespace http
#endif // ERROR_H
