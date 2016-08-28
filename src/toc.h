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
-# @ref TOC
-# @ref Description
-# @ref Dependencies
 -# @ref Native
-# @ref Build
-# @ref Usage
 -# @ref SyncClient
 -# @ref AsyncClient
 -# @ref Server
-# @ref Delegates
 -# @ref FileServer
 -# @ref WebSocket
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
Create a WebServer object first. WebServer needs to be parameterized when defined. with the parameter
you can set the type of server created. Available implementations are HttpServer and HttpsServer.

    #include "http.h"

    http::WebServer< http::HttpServer > server ( "localhost", 9999 );

The parameters address and protocol are passed directly to the ASIO resolver. host can be a hostname or IP-Adress. Port a protocol (i.e. http) or the port number.

Register deleagates to the server.

bind a static free function:

    server.bind ( "/foo/bar",  http_delegate_t ( delegate_function ) );

bind a class function:

    server.bind ( "/foo/bar/cls", std::bind ( &MethodClass::delegate_function, &cls, std::placeholders::_1, std::placeholders::_2 ) );

bind a class function using parameters in the `uri`:

    server.bind ( "/foo/bar/(\\w+)/(\\d+)",
        std::function< void ( HttpRequest&, HttpResponse&, std::string, int ) > (
            std::bind ( &MethodClass::delegate_function_user, &cls, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 ) ) );

bind a class function using parameters in the `uri` and `bind` a lambda function as delegate:
    server.bind ( "/foo/lambda/(\\w+)/(\\d+)", std::function< void ( HttpRequest&, HttpResponse&, std::string, int ) > (
            [] ( HttpRequest & request, HttpResponse&, std::string name, int id ) {
                std::cout << request.uri() << name << ", " << id << std::endl;
    } ) );

the `uri` parmeter can be a concrete url or a RE2 regexp expression. Whis the regular expressions you can retreive the group matches delegate arguments. you
can set the `uri` to "*" and match all requests. the function casting for the bind method is neccesary for the compiler to match the template.

@subsection Delegates Available Delegates
@subsubsection FileServer File Server
the with the filedelegate you can serve a local file folder.
@subsubsection WebSocket WEB-Socket Server
*/
}
#endif // TOC_H
