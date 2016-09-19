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

@section Dependencies Dependencies
Squawk server and client is built using the following 3rd party software:

- [ASIO](http://think-async.com)
- [RE2](https://github.com/google/re2) RE2 backtracking regular expression engine.
- []()mstch (optional)

@subsection Native Native dependencies
The deveopment build of these libraries have to be installed on the build system:
@section Build Build and Installation
lightning must be compiled with a C++14 compliant compiler.
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

*/
}
#endif // TOC_H
