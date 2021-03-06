
#include <csignal>

#include <sstream>
#include <memory>

#include "../include/http/server.h"
#include "../include/http/httpserver.h"
#include "../include/http/request.h"
#include "../include/http/response.h"
#include "../include/http/mod/exec.h"
#include "../include/http/mod/file.h"
#include "../include/http/mod/match.h"
#include "../include/http/mod/http.h"
#include "../include/http/mod/ws.h"

void signalHandler( int signum ) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    exit(signum);
}

//call with documentation and testfiles path as params.
int main(int argc, char * argv[] ) {
    std::cout << "start web server: " << argc << "{ ";
    for( int i = 0; i<argc; ++i ) {
        std::cout << i << ":" << argv[i] << " ";
    }
    std::cout << "}" << std::endl;

    //create the server
    http::Server< http::HttpServer > server ( "0.0.0.0", "8888" );
    std::vector< std::string > _ws_protocols ( { "protocolTwo" } );
    server.bind( http::mod::Match<>( "/socketserver" ), http::mod::WS( _ws_protocols ), http::mod::Http() );
    server.bind( http::mod::Match<>( "/doc/.*" ), http::mod::File( argv[1], "/doc/" ), http::mod::Http() );
    server.bind( http::mod::Match<>( "/index2.html" ), http::mod::Exec( [] ( http::Request&, http::Response & response ) {
                     return http::http_status::OK;
                 } ), http::mod::Http() );
    server.bind( http::mod::Match<>( "*" ), http::mod::File( argv[2] ), http::mod::Http() );

    // register signal SIGINT and signal handler
    signal(SIGINT, signalHandler);

    while(1){
       sleep(1);
    }

    return 0;
}
