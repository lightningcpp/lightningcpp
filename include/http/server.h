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
#ifndef WebServer22_H
#define WebServer22_H

#include <functional>
#include <type_traits>

#include "constant.h"
#include "request.h"
#include "response.h"

#include "mod/mod.h"
#include "mod/error.h"
#include "mod/http.h"

#include "linux/log.h"

namespace http {

template< typename T >
/**
 * @brief The WebServer2 class.
 *
 * the request execution call flow:
 *\m s c
 *  T, WebServer2, WebServer2Delegate, HeaderParameter, Delegate;
 *
 *  T->WebServer2 [label="Request, Response", URL="\ref WebServer2::execute( Request&, Response& )"];
 *  WebServer2->WebServer2Delegate [label="execute(Request&, Response&)"];
 *  WebServer2Delegate->Delegate [label="execute( \n Request&, \n Response&, \n Args... )" ];
 *  WebServer2Delegate<-Delegate [label="bool" ];
 *  WebServer2Delegate->HeaderParameter [label="set header parameter"];
 *  WebServer2Delegate<-HeaderParameter [label="void"];
 *  WebServer2<-WebServer2Delegate [label="void"];
 *  T<-WebServer2 [label="void"];
 *\ e n d m s c
 */
class Server : T {
public:
        Server ( const std::string & ip, const std::string & protocol ) :
                T ( ip, protocol, http_delegate_t ( std::bind ( &Server::request, this, std::placeholders::_1, std::placeholders::_2 ) ) ) {
            bind_error( mod::Error(), mod::Http() ); //TODO UGLY
        }
        Server ( const Server& ) = delete;
        Server ( Server&& ) = delete;
        Server& operator= ( const Server& ) = delete;
        Server& operator= ( Server&& ) = delete;
        virtual ~Server() {}

    template< class... M >
    void bind( M&&... mods ) {
        delegates_.push_back ( std::bind(
            &mod::Mod< M... >::execute, std::make_shared< mod::Mod< M... > >( std::move( mods )... ), _1, _2 ) );
    }

    template< class... M >
    void bind_error( M&&... mods ) {
        error_delegates_ = std::bind(
            &mod::Mod< M... >::execute, std::make_shared< mod::Mod< M... > >( std::move( mods )... ), _1, _2 );
    }

    void request ( Request & request, Response & response ) {
        std::cout << request.method() << ":" << request.uri() << " { ";
        for( auto& name : request.parameter_names() ) {
            std::cout << name << "=" << request.parameter_map().at( name ) << ", ";
        }
        std::cout << std::endl;

        //http::log( "request %s", request.remote_ip() );
        http_status _s = http_status::INTERNAL_SERVER_ERROR;
        for( auto & d : delegates_ ) {
            _s = d( request, response );
            if (  _s == http_status::OK ) {
                response.status( _s );
                return;
            }
        }
        if( _s == http::http_status::_NO_MATCH ) {
            response.status( http::http_status::NOT_FOUND );
        } else {
            response.status( _s );
        }
        error_delegates_( request, response );
    }

private:
    std::vector< std::function< http_status ( Request&, Response& ) > > delegates_;
    std::function< http_status ( Request&, Response& ) > error_delegates_;
};

}//namespace http
#endif // WebServer22_H
