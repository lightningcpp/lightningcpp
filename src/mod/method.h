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
#ifndef HTTP_MOD_METHOD_H
#define HTTP_MOD_METHOD_H
#include <map>
#include <memory>
#include <string>

#include "../constant.h"
#include "../request.h"
#include "../response.h"

#include "mod.h"

///@cond DOC_INTERNAL
namespace http {
namespace mod {
namespace _method_utils {

inline bool stat_is_success( http_status s) {
    return ( static_cast< int > ( s ) >= 200 && static_cast< int > ( s ) < 300 );
}

template<std::size_t I = 0, class Rq, class Rs, typename... Tp>
inline typename std::enable_if<I == sizeof... ( Tp ), http_status>::type
T_execute_method ( Rq&, Rs&, std::tuple<Tp...>& ) { return http::http_status::METHOD_NOT_ALLOWED; }

template<std::size_t I = 0, class Rq, class Rs, typename... Tp>
inline typename std::enable_if<I < sizeof... ( Tp ), http_status>::type
T_execute_method ( Rq& request, Rs& response, std::tuple<Tp...>& t ) {
    http_status _s = std::get<I> ( t ).execute ( request, response );

    if ( _s != http::http_status::_NO_MATCH && _s != http::http_status::METHOD_NOT_ALLOWED ) {
        /* if( stat_is_success( _s ) ) {
            response.status( _s );
        } else */ return _s;

    }
    return T_execute_method<I + 1, Rq, Rs, Tp...> ( request, response, t );
}

template< class... Placeholders >
struct MethodChain {
public:
    MethodChain ( Placeholders&&... p ) :_modules ( std::move( p )... ) {}
    MethodChain ( const MethodChain& ) = delete;
    MethodChain ( MethodChain&& ) = default;
    MethodChain& operator= ( const MethodChain& ) = delete;
    MethodChain& operator= ( MethodChain&& ) = default;

    http_status execute ( http::Request& request, http::Response& response ) {
        return _method_utils::T_execute_method( request, response, _modules );
    }

private:
    std::tuple< Placeholders... > _modules;
};

}//namespace _method_utils
}//namespace mod
}//namespace http
///@endcond DOC_INTERNAL

namespace http {
namespace mod {
/** \brief method implementations */
namespace method {
/** @brief Method with string. */
struct S{
    template< typename... M >
    S( const std::string & method, M&&... mods ) :
        method_ ( method ),
        delegate_ ( std::bind( &http::mod::Mod< M... >::execute, std::make_shared< http::mod::Mod< M... > >( std::move( mods )... ), _1, _2 ) ) {}

    /**
     * @brief execute request
     * @param request
     * @param response
     * @return
     */
    http_status execute ( Request& request, Response& response ) {
        if( request.method() == method_ )  {
            return ( delegate_( request, response ) );
        }
        return http::http_status::_NO_MATCH;
    }

private:
    const std::string method_;
    std::function< http_status ( Request&, Response& ) > delegate_;
};

/** @brief Method CONNECT. */
struct CONNECT : S {
    template< typename... M >
    CONNECT( M&&... mods ) : S( http::method::CONNECT, std::move( mods... ) ) {}
};
/** @brief Method DELETE. */
struct DELETE : S {
    template< typename... M >
    DELETE( M&&... mods ) : S( http::method::DELETE, std::move( mods... ) ) {}
};
/** @brief Method GET. */
struct GET : S {
    template< typename... M >
    GET( M&&... mods ) : S( http::method::GET, std::move( mods... ) ) {}
};
/** @brief Method HEAD. */
struct HEAD : S {
    template< typename... M >
    HEAD( M&&... mods ) : S( http::method::HEAD, std::move( mods... ) ) {}
};
/** @brief Method OPTIONS. */
struct OPTIONS : S {
    template< typename... M >
    OPTIONS( M&&... mods ) : S( http::method::OPTIONS, std::move( mods... ) ) {}
};
/** @brief Method POST. */
struct POST : S {
    template< typename... M >
    POST( M&&... mods ) : S( http::method::POST, std::move( mods... ) ) {}
};
/** @brief Method PUT. */
struct PUT : S {
    template< typename... M >
    PUT( M&&... mods ) : S( http::method::PUT, std::move( mods... ) ) {}
};
/** @brief Method TRACE. */
struct TRACE : S {
    template< typename... M >
    TRACE( M&&... mods ) : S( http::method::TRACE, std::move( mods... ) ) {}
};
}//namespace method
}//namespace mod
}//namespace http

namespace http {
namespace mod {


/**
 * @brief The HTTP method module filters requests by the requested method.

For creation a chain of method Implementations is needed. When
the method matches the delegate will be executed. Otherwise the
status METHOD_NOT_ALLOWED is returned.

exmaple usage:

    using namespace http::mod;
    using namespace http::mod::method;

    Method (
        POST( Exec( [] (Request&, Response&) {
            ## IMPLEMENTATION HERE ##
            return http::http_status::OK;
        })),
        GET( Exec( [] (Request&, Response&) {
            ## IMPLEMENTATION HERE ##
            return http::http_status::OK;
        })),
        S( "SUBSCRIBE", Exec( [] (Request&, Response&) {
            ## IMPLEMENTATION HERE ##
            return http::http_status::OK;
        }))
    )
*/
class Method {
public:
    /**
     * @brief Method class.
     * @param delegates Map with the name and delegates to execute.
     */
    template< typename... M >
    Method( M&&... methods ) : delegates_ ( std::bind(
        &http::mod::_method_utils::MethodChain< M... >::execute, std::make_shared< http::mod::_method_utils::MethodChain< M... > >( std::move( methods )... ), _1, _2 ) ) {}

    Method ( const Method& ) = delete;
    Method ( Method&& ) = default;
    Method& operator= ( const Method& ) = delete;
    Method& operator= ( Method&& ) = default;
    ~Method() {}

    /**
     * @brief execute
     * @param request
     * @param response
     * @return
     */
    http_status execute ( Request& request, Response& response ) {
        http::http_status _s = delegates_( request, response );
        if( _s == http::http_status::_NO_MATCH )
        { return http::http_status::METHOD_NOT_ALLOWED; }
        return _s;
    }
private:
    http_connection_t delegates_;
};
}//namespace mod
}//namespace http
#endif // HTTP_MOD_METHOD_H
