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
#ifndef EXEC_H
#define EXEC_H

#include <memory>
#include <string>

#include "../httpconfig.h"
#include "../request.h"
#include "../response.h"

namespace http {
namespace mod {

class Exec  {
public:
    Exec ( std::function< http_status ( http::Request&, http::Response& ) >&& f ) : _f ( std::move ( f ) ) {}
    Exec ( const Exec& ) = delete;
    Exec ( Exec&& ) = default;
    Exec& operator= ( const Exec& ) = delete;
    Exec& operator= ( Exec&& ) = default;
    ~Exec() {}

    http_status execute ( Request& request, Response& response ) {
        return _f ( request, response );
    }
private:
    std::function< http_status ( http::Request&, http::Response& ) > _f;
};
}//mod http
}//namespace http
#endif // EXEC_H
