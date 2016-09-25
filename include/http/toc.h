#ifndef TOC_H
#define TOC_H

/**
 *\brief the main package for the http classes.
 */
namespace http {


/**
@mainpage lightning http libraries
@section TOC Table of Contents
<hr/>
-# @ref Description
-# @ref Dependencies
 -# @ref Native
-# @ref Build
-# @ref Usage
 -# @ref SyncClient
 -# @ref AsyncClient
 -# @ref Server
-# @ref Delegates
\n
@section Description Description
lightning is a http server/client library written in modern c++ (C++14). The name lightning is from the television series Tom&Jerry
where lightning is the orange cat beside of Tomcat.

This library is licesensed under the [GNU Lesser General Public License](https://www.gnu.org/licenses/lgpl-3.0.txt)

@section Build Build and Installation
this library does not contain any c files that need compilation. You can include the header files in your project.
Depending on the used functionality additional libaries are needed for the compilation of the client code.

@subsection SyncClient HTTP sync client
To build the libarary you need to have the [ASIO](http://think-async.com) headers available, boost is not required for ASIO.
For the regexp pattern matching you have to include googles [RE2](https://github.com/google/re2) RE2 backtracking regular
expression engine. for the basic usage also C++14 compliant compiler is needed.

include lightning with cmake using ExternalPackage_Add:

    ExternalProject_Add(
      lightningcpp
      URL "https://github.com/lightningcpp/lightningcpp/archive/master.zip"
      CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      INSTALL_COMMAND ""
      UPDATE_COMMAND ""
      PATCH_COMMAND ""
    )
    ExternalProject_Get_Property(lightningcpp source_dir)
    set(LIGHTNING_INCLUDE_DIR ${source_dir}/src/)

    include( cmake/external/re2.cmake )
    include( cmake/external/asio.cmake )

//    include( cmake/external/bustache.cmake )
//    SET( INCLUDES ${INCLUDES} ${RE2_INCLUDE_DIR} ${ASIO_INCLUDE_DIR}
//                  ${BUSTACHE_INCLUDE_DIR} )

@subsection compileroptions Compiler options
At compile time you can set the following options.

option(build_example_server "Build the example server." OFF)

option(build_tests "Build all http unit tests." OFF)

HTTP_BUFFER_SIZE The HTTP character buffer size. [8192 bytes]

HTTP_CLIENT_TIMEOUT HTTP connection timeout in seconds. [3 sec]

HTTP_SERVER_THREAD_POOL_SIZE The number of threads created on the ASIO service. [10 threads]

these options have to be set when running the test cases or example server.

TESTFILES The path to the testfiles. [${PROJECT_SOURCE_DIR}/test/files]

DOCFILES TESTFILES The path to the documentation files. [${PROJECT_BINARY_DIR}/doc/]

@section Usage
@subsection SyncClient HTTP sync client
@subsection AsyncClient HTTP async client
@subsection Server HTTP Server
You can bind delegate methods to the server. this delegates will be called when the pattern matches. The methods can contain the
request uri arguments.

    ## delegate implementations ##
    void execute( HttpRequest&, HttpResponse&, ... ) {
            ## Handle Request ##
    }
    class MethodClass {
        void execute( HttpRequest&, HttpResponse&, ... ) {
            ## Handle Request ##
        }
    }

Create a WebServer object first. WebServer needs to be parameterized when defined. with the parameter
you can set the type of server created. Available implementations are HttpServer and HttpsServer.

    #include "http.h"

    http::WebServer< http::HttpServer > server ( "localhost", 9999 );

The parameters address and protocol are passed directly to the ASIO resolver. host can be a hostname or IP-Adress. Port a protocol (i.e. http) or the port number.

Register deleagates to the server.

bind the delegates function:

    server.bind ( "/static",  execute );
    server.bind ( "/cls", &MethodClass::execute, &method_class );

    server.bind< DefaultParameter, HttpRequest&, &HttpResponse, std::string, int > ( "/static/(\\w+)/(\\d+)", execute, _1, _2, _3, _4 );
    server.bind< DefaultParameter, std::string, int > ( "/cls/(\\w+)/(\\d+)", &MethodClass::execute, &method_class, _1, _2, _3, _4 );

    server.bind ( "/lambda", [] ( HttpRequest & request, HttpResponse& ) {
        test_callback->callback ( request.uri() );
    } );
    server.bind< DefaultParameter, std::string, int >( "/lambda/(\\w+)/(\\d+)", [] ( HttpRequest & request, HttpResponse&, std::string name, int id ) {
        test_callback->callback ( request.uri(), name, id );
    } );

the `uri` parmeter can be a concrete url or a RE2 regexp expression. Whis the regular expressions you can retreive the group matches delegate arguments. you
can set the `uri` to "*" and match all requests. the function casting for the bind method is neccesary for the compiler to match the template.

the DefaultParameter template argument defines the HttpParameterParser used for this delegate. The HttpParameterParser takes the HttpRequest and HttpResponse
objects as arguments. The implementation must fill the header parameters of the Response are filled with the needed headers.

    class NewHttpParameterParser {
    public:
        static void execute ( auto& request, auto & response ) {
            if ( ! response.contains_parameter ( header::CONTENT_LENGTH ) ) {
                response.parameter ( header::CONTENT_LENGTH,  std::to_string ( response.tellp() ) );
            }
            ### set more parameters ###
        }
    };

@subsection Delegates Available delegates

- mod::Match Match a request by uri.
- mod::Method Filter requests by method.
- mod::Mstch Mustache template engine.

@section Dependencies Dependencies
Squawk server and client is built using the following 3rd party software:

- [ASIO](http://think-async.com)
- [RE2](https://github.com/google/re2) RE2 backtracking regular expression engine.
- []()mstch (optional)
*/
}
#endif // TOC_H
