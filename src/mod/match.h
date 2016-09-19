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
#ifndef HTTP_MOD_MATCH_H
#define HTTP_MOD_MATCH_H

#include <sstream>
#include <string>
#include <vector>

#include "re2/re2.h"

#include "../constant.h"
#include "../request.h"
#include "../response.h"

///@cond DOC_INTERNAL
namespace http {
namespace mod {
namespace _match_utils {

template< class S >
inline std::string to_arg ( const S & s ) {
    std::stringstream _buf;
    _buf << s;
    return _buf.str();
}

/* bind the attributes from args */
template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I == sizeof... ( Tp ), void>::type
bind_arg ( const std::vector< std::string >&, http::Request&, std::tuple<Tp...>& ) {}

template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I < sizeof... ( Tp ), void>::type
bind_arg ( const std::vector< std::string > & names, http::Request& request, std::tuple<Tp...>& t ) {
    request.attribute ( names[I], to_arg ( std::get<I> ( t ) ) );
    bind_arg<I + 1, Tp...> ( names, request, t );
}

/* match with uri args */

template< int size, class... Args > struct T_Match_Impl {
    template< class... Names >
    T_Match_Impl ( const std::string& uri, const Names... names ) : uri_ ( uri ), names_ ( { names... } ) {}

    http_status execute ( Request& request, Response& ) {
        std::tuple< Args... > args;

        if ( bind_with_tuple ( request, args, std::index_sequence_for<Args...>() ) ) {
            bind_arg ( names_, request, args );
            return http::http_status::OK;
        } else {
            return http::http_status::NOT_FOUND;
        }
    }

private:
    const std::string uri_;
    std::vector< std::string > names_;

    template< std::size_t... Is >
    inline bool bind_with_tuple ( Request & request, std::tuple<Args...>& tuple, std::index_sequence<Is...> ) {
        return ( RE2::FullMatch ( request.uri(), uri_, &std::get<Is> ( tuple )... ) );
    }
};

/* match with uri */

template< class... Args > struct T_Match_Impl< 0, Args... > {
    template< class... Names >
    T_Match_Impl ( const std::string & uri, Names... ) : _uri ( uri ) {}
    http_status execute ( http::Request & request, http::Response& ) {
        if ( ! ( _uri == "*" || request.uri() == _uri || match ( request.uri(), _uri ) ) ) {
            return http::http_status::NOT_FOUND;
        } else { return http::http_status::OK; }
    }

private:
    const std::string _uri;
    inline bool match ( const std::string & left, const std::string & right ) {
        return RE2::FullMatch ( left, right ); //TODO also use std::regexp
    }
};

template< class... Args >
constexpr bool T_empty_args() {
    return ( sizeof... ( Args ) == 0 );
}
template< class... Args >
constexpr bool T_bigger() {
    return ( sizeof... ( Args ) == 0 );
}
}//namespace _match_utils
}//namespace mod
}//namespace http
///@endcond DOC_INTERNAL

namespace http {
namespace mod {
/**
 * @brief The HTTP match module filters requests by uri.

Possible values for the uri:

- "/foo/bar.html" Match the uri exactly.
- "/foo/.*" Match the uri with regex.
- "/foo/(\\w+)/(\\d+)" Match the uri with regex using placeholders.
- "*" Match all uris.

When placeholders are defined, the values will be stored in Request attribute using the given names.

exmaple usage:

    using namespace http::mod;

    Match<> match( "*" );
    ## matches everything ##

    Match<> match( "/foo" );
    ## matches /foo ##

    Match< std::string, int > match( "/foo/(\\w+)/(\\d+)", "name", "user-id" );
    ## matches /foo/alice/123 ##
    ## request.attribute( "name" ) == "alice" ##
 */
template< class... Args > struct Match : public _match_utils::T_Match_Impl< sizeof... ( Args ), Args... > {
    template< class... Names >
    /**
     * @brief Match a request by uri.
     * @param uri The uri to match.
     * @param names The names of the placeholders in the regex.
     */
    Match ( const std::string & uri, Names... names ) : _match_utils::T_Match_Impl< sizeof... ( Args ), Args... > ( uri, names... ) {}

    Match( const Match& ) = delete;
    Match ( Match&& ) = default;
    Match& operator= ( const Match& ) = delete;
    Match& operator= ( Match&& ) = default;
};
}//namespace mod
}//namespace http
#endif // HTTP_MOD_MATCH_H
