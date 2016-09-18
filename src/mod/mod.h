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

#include "../constant.h"
#include "../request.h"
#include "../response.h"

///@cond DOC_INTERNAL
namespace http {
namespace mod {
namespace _mod_utils {

inline bool stat_is_success( http_status s) {
    return ( static_cast< int > ( s ) >= 200 && static_cast< int > ( s ) < 300 );
}

template<std::size_t I = 0, class Rq, class Rs, typename... Tp>
inline typename std::enable_if<I == sizeof... ( Tp ), http_status>::type
T_execute_module ( Rq&, Rs&, std::tuple<Tp...>& ) { return http::http_status::OK; }

template<std::size_t I = 0, class Rq, class Rs, typename... Tp>
inline typename std::enable_if<I < sizeof... ( Tp ), http_status>::type
T_execute_module ( Rq& request, Rs& response, std::tuple<Tp...>& t ) {
    http_status _s = std::get<I> ( t ).execute ( request, response );

    if ( _s != http::http_status::_NO_MATCH ) {
        if( stat_is_success( _s ) ) {
            response.status( _s );
        } else return _s;

    }
    return T_execute_module<I + 1, Rq, Rs, Tp...> ( request, response, t );
}
}//namespace _mod_utils
}//namespace mod
}//namespace http

namespace http {
namespace mod {

template< class... Placeholders >
/**
 * @brief The Mod struct executes all the registered modules.
 */
struct Mod {
public:
    /**
     * @brief Create a new Mod object.
     * @param p the modules.
     */
    Mod ( Placeholders&&... p ) :_modules ( { std::move( p )... } ) {}
    Mod( const Mod& ) = delete;
    Mod ( Mod&& ) = default;
    Mod& operator= ( const Mod& ) = delete;
    Mod& operator= ( Mod&& ) = default;

    http_status execute ( http::Request& request, http::Response& response ) {
        return _mod_utils::T_execute_module ( request, response, _modules );
    }

private:
    std::tuple< Placeholders... > _modules;
};
}//namespace mod
}//namespace http
///@endcond DOC_INTERNAL
#endif // HTTP_MOD_H
