
#include <csignal>

#include <sstream>
#include <memory>

#include "src/server.h"
#include "src/httpserver.h"
#include "src/request.h"
#include "src/response.h"
#include "src/mod/file.h"
#include "src/mod/match.h"
#include "src/mod/http.h"
#include "src/mod/ws.h"

void signalHandler( int signum ) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    exit(signum);
}

int main(int argc, char * argv[] ) {
    std::cout << "start web server: " << argc << "{ ";
    for( int i = 0; i<argc; ++i ) {
        std::cout << argv[i] << " ";
    }
    std::cout << "}" << std::endl;

    //create the server
    http::Server< http::HttpServer > server ( "192.168.0.17", "9999" );
    std::vector< std::string > _ws_protocols ( { "protocolTwo" } );
    server.bind( http::mod::Match<>( "/socketserver" ), http::mod::WS( _ws_protocols ), http::mod::Http() );
    server.bind( http::mod::Match<>( "/doc/.*" ), http::mod::File( DOCFILES, "/doc/" ), http::mod::Http() );
    server.bind( http::mod::Match<>( "*" ), http::mod::File( TESTFILES ), http::mod::Http() );

    // register signal SIGINT and signal handler
    signal(SIGINT, signalHandler);

    while(1){
       sleep(1);
    }

    return 0;
}
