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
#ifndef HTTP_MOD_H
#define HTTP_MOD_H

#include <memory>
#include <tuple>

#include "../httpconfig.h"
#include "../request.h"
#include "../response.h"

namespace http {
namespace mod {

template<std::size_t I = 0, class Rq, class Rs, typename... Tp>
inline typename std::enable_if<I == sizeof... ( Tp ), http_status>::type
T_execute_module ( Rq&, Rs&, std::tuple<Tp...>& ) { return http::http_status::OK; }

template<std::size_t I = 0, class Rq, class Rs, typename... Tp>
inline typename std::enable_if<I < sizeof... ( Tp ), http_status>::type
T_execute_module ( Rq& request, Rs& response, std::tuple<Tp...>& t ) {
    http_status _s = std::get<I> ( t ).execute ( request, response );

    if ( _s != http::http_status::OK ) {
        return _s;

    } else { return T_execute_module<I + 1, Rq, Rs, Tp...> ( request, response, t ); }
}

template< class... Placeholders >
struct Mod : public std::enable_shared_from_this< Mod< Placeholders... > > { //TODO remove ebable shared
public:
    Mod ( Placeholders&&... p ) :_modules ( { std::move( p )... } ) {}
    Mod( const Mod& ) = delete;
    Mod ( Mod&& ) = default;
    Mod& operator= ( const Mod& ) = delete;
    Mod& operator= ( Mod&& ) = default;

    http_status execute ( http::Request& request, http::Response& response ) {
        return T_execute_module ( request, response, _modules );
    }

private:
    std::tuple< Placeholders... > _modules;
};

}//mod http
}//namespace http
#endif // HTTP_MOD_H
