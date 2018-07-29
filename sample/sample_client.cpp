
#include <sstream>
#include <memory>

#include "../include/http/httpclient.h"

//call with documentation and testfiles path as params.
int main ( int argc, char * argv[] ) {
    std::cout << "GET: " << argv[1] << std::endl;
    std::stringstream _ss;
    auto _response = http::get ( argv[1], _ss );
    std::cout << static_cast< int > ( _response.status() ) << _ss.str() << std::endl;
    return 0;
}
