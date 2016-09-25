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
#ifndef HTTP_MOD_LOG_H
#define HTTP_MOD_LOG_H

#include <cstdio>

#include <map>
#include <memory>
#include <string>

#include "../constant.h"
#include "../request.h"
#include "../response.h"

#include "http.h"

namespace http {
namespace mod {

class Log : Http {
public:
    Log() {}
    Log( const Log& ) = delete;
    Log( Log&& ) = default;
    Log& operator= ( const Log& ) = delete;
    Log& operator= ( Log&& ) = default;
    ~Log() {}

    static http_status execute ( Request& request, Response& response ) {

        // remotehost rfc931 authuser [date] "request" status bytes
        printf( "%s rfc931 %s [date] \"request\" %.3u %u", //TODO
                request.remote_ip(), "anonymous", request.uri(), response.parameter( http::header::CONTENT_LENGTH ) );
        return http::http_status::OK;
    }
};
}//namespace mod
}//namespace http
#endif // HTTP_MOD_LOG_H
