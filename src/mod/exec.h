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
#ifndef HTTP_MOD_EXEC_H
#define HTTP_MOD_EXEC_H

#include <string>

#include "../constant.h"
#include "../request.h"
#include "../response.h"

namespace http {
namespace mod {

/**
 * @brief Execute a delegate method.

The CTOR takes a std::function object of the type http_connection_t. The method must
return a HTTP status.

Example Usage:

    Exec x( [] ( Request&, Response& ) {
        ## code here ##
        return http::http_status::OK;
    } );

You can use std::bind to set the delegate with any existing method.

 */
class Exec  {
public:
    /**
     * @brief Create the Exec Object with a delegate method.
     * @param f delegate function pointer
     */
    Exec ( auto&& f ) : _f ( std::move ( f ) ) {}
    Exec ( const Exec& ) = delete;
    Exec ( Exec&& ) = default;
    Exec& operator= ( const Exec& ) = delete;
    Exec& operator= ( Exec&& ) = default;
    ~Exec() {}

    http_status execute ( Request& request, Response& response ) {
        return _f ( request, response );
    }
private:
    http_connection_t _f;
};
}//namespace mod
}//namespace http
#endif // HTTP_MOD_EXEC_H
