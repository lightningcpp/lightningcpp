
#include <csignal>

#include <sstream>
#include <memory>

#include "src/webserver.h"
#include "src/httpserver.h"
#include "src/filedelegate.h"
#include "src/httprequest.h"
#include "src/httpresponse.h"
#include "src/websocketdelegate.h"

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
    http::WebServer< http::HttpServer > server ( "192.168.0.17", 9999 );
    std::vector< std::string > _ws_protocols ( { "protocolTwo" } );
    http::delegate::WebSocketDelegate ws_( _ws_protocols );

    //http::delegate::FileDelegate file_delegate( TESTFILES );
    http::delegate::FileDelegate file_delegate( "/home/e3a/build-httpcpp-gcc-Debug/Documentation/html/" );
    server.bind( "/socketserver", std::bind( &http::delegate::WebSocketDelegate::execute, &ws_, std::placeholders::_1, std::placeholders::_2 ) );
    server.bind( "*", std::function< void( http::HttpRequest&, http::HttpResponse& ) >(
        std::bind( &http::delegate::FileDelegate::execute, &file_delegate, std::placeholders::_1, std::placeholders::_2 ) ) );

    // register signal SIGINT and signal handler
    signal(SIGINT, signalHandler);

    while(1){
       sleep(1);
    }

    return 0;
}
