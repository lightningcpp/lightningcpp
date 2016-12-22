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

#include <string>

#include "../include/http/request.h"
#include "../include/http/response.h"

#include <gtest/gtest.h>

//includes for macros
#include <memory>
#include <unordered_map>
#include <string>

#include "../include/http/server.h"
#include "../include/http/httpserver.h"
#include "../include/http/mod/file.h"
#include "../include/http/mod/http.h"
#include "../include/http/mod/match.h"

namespace http {

#define SERVER( host, port ) \
    std::unique_ptr< Server< HttpServer > > __server = std::make_unique< Server< HttpServer > > ( host, port );

#define DOCROOT( uri, path ) \
    __server->bind( http::mod::Match<>( uri ), http::mod::File( path ), http::mod::Http() );

    struct ObjectRegistry {
    virtual ~ObjectRegistry() = default;

    template <typename D>
    struct Registrar {
        explicit Registrar(std::string const & name)
 //       { /* Base::registrate(name, &D::create); */ }
    { std::cout << "register" << name << std::endl; }
    };

private:
//    static std::unordered_map<std::string, create_f *> & registry() {
//        static std::unordered_map<std::string, Base::create_f *> impl;
//        return impl;
//    }
};

#define SERVE( id, uri, request, response  ) \
    void id( request, response )

#define webclass( id, uri, request, response  ) \
    class id { \
    public:\
        id() {} \
        void id_callback( Request&, Response& ); \
    }; \
    static std::shared_ptr< id > _ptr = std::make_shared< id >(); \
    static ObjectRegistry::Registrar< id > _object = ObjectRegistry::Registrar< id > ( uri  ); \
    void id::id_callback( request, response )


    webclass( delegate, "/delegate/foo/bar", Request&, Response& ) {

    }

TEST ( RegistryTest, TestRegister ) {

    SERVER( "localhost", "9000" )
    DOCROOT( "*", TESTFILES )
    //REGISTER( &method, this )

    //EXPECT_EQ( http::http_status::OK, match.execute ( _request, _response ) );
}
}//namespace http
