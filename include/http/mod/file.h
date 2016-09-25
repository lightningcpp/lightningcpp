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
#ifndef HTTP_MOD_FILE_H
#define HTTP_MOD_FILE_H

#include <sys/stat.h>

#include <fstream>
#include <string>

#include "../constant.h"
#include "../request.h"
#include "../response.h"

///@cond DOC_INTERNAL
namespace http {
namespace mod {
namespace _file_utils {
static inline void parse_filename ( const std::string & path, std::string & filename, std::string & extension ) {
    std::size_t last_slash_pos = path.find_last_of ( "/" );
    std::size_t last_dot_pos = path.find_last_of ( "." );

    if ( last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos ) {
        extension = path.substr ( last_dot_pos + 1 );

    } else { extension = "txt"; }

    if ( last_slash_pos != std::string::npos && last_slash_pos < path.size() ) {
        filename = path.substr ( last_slash_pos + 1 );

    } else { throw http_status::BAD_REQUEST; }
}
}//namespace _file_utils
}//namespace mod
}//namespace http
///@endcond DOC_INTERNAL

namespace http {
namespace mod {
/**
 * @brief The file module.
 */
class File  {
public:
    /**
     * @brief File
     * @param docroot
     * @param prefix
     */
    File( const std::string & docroot, const std::string & prefix = "" ) : docroot_( docroot ), prefix_( prefix ) {}
    File ( const File& ) = delete;
    File ( File&& ) = default;
    File& operator= ( const File& ) = delete;
    File& operator= ( File&& ) = default;
    ~File() {}

    http_status execute ( Request& request, Response& response ) {

        /* test if the path is valid */
        if ( request.uri().empty() || request.uri() [0] != '/' || request.uri().find ( ".." ) != std::string::npos ) {
            return http::http_status::BAD_REQUEST;
        }

        std::stringstream ss_;
        if( prefix_.empty() ) { //handle prefix
            ss_ << docroot_ << request.uri();
        } else {
            ss_ << docroot_ << request.uri().substr( prefix_.size() );
        }

        struct stat filestatus;
        std::string extension_, filename_;
        stat ( ss_.str().c_str(), &filestatus );

        //when the request path is a directory add index.html
        if ( S_ISDIR ( filestatus.st_mode ) ) {
            filename_ = "index.html";
            extension_ = "html";
            ss_ << "/index.html";
            stat ( ss_.str().c_str(), &filestatus );

        } else {
            //get the filename and extension
            _file_utils::parse_filename ( ss_.str(), filename_, extension_ );
        }

        if ( S_ISREG ( filestatus.st_mode ) ) {
            /* test if the file has changed */
            std::string s_time_ = http::utils::time_to_string ( &filestatus.st_mtime );

            if ( request.contains_parameter ( http::header::IF_MODIFIED_SINCE ) &&
                    request.parameter ( http::header::IF_MODIFIED_SINCE ) == s_time_ ) {
                return http_status::NOT_MODIFIED;
            }

            //add headers
            response.parameter ( header::CONTENT_LENGTH, std::to_string ( filestatus.st_size ) );
            response.parameter ( http::header::CONTENT_DISPOSITION, "inline; filename= \"" + filename_ + "\"" );
            response.parameter ( http::header::CONTENT_TYPE, mime::mime_type ( mime::mime_type ( extension_ ) ) );
            response.last_modified ( filestatus.st_mtime );
            response.expires ( 3600 * 24 );

            //process request
            if ( request.method() == http::method::GET ) {
                get ( request, response, ss_.str(), filestatus.st_size );

            } else if ( request.method() != http::method::HEAD )
            { return http::http_status::NOT_IMPLEMENTED; }

        } else { return http::http_status::NOT_FOUND; }

        return http_status::OK;
    }
private:
    const std::string docroot_, prefix_;

    void get ( Request & request, Response & response, const std::string & filename, const size_t file_size ) {

        // Open the file to send back.
        std::unique_ptr < std::ifstream > is = std::make_unique< std::ifstream > ( filename.c_str(), std::ios::in | std::ios::binary );

        if ( !is->is_open() ) {
            throw http_status::NOT_FOUND;
        }

        // Fill out the reply to be sent to the client.
        if ( request.contains_parameter ( http::header::RANGE ) ) {
            std::cout << "get Range" << std::endl;
            response.status ( http_status::PARTIAL_CONTENT );
            std::tuple<int, int> range = http::utils::parseRange ( request.parameter ( http::header::RANGE ) );
            std::cout << "get range: " << std::get<0> ( range ) << "-" << std::get<1> ( range ) << std::endl;
            response.parameter ( "Content-Range", "bytes " + std::to_string ( std::get<0> ( range ) ) + "-" +
                                 ( std::get<1> ( range ) == -1 ? std::to_string ( file_size - 1 ) :
                                   std::to_string ( std::get<1> ( range ) - 1 ) ) +
                                 "/" + std::to_string ( file_size ) );
            response.parameter ( header::CONTENT_LENGTH, ( std::get<1> ( range ) == -1 ? std::to_string ( file_size - std::get<0> ( range ) ) :
                                 std::to_string ( std::get<1> ( range ) - std::get<0> ( range ) ) ) );
            is->seekg ( std::get<0> ( range ), std::ios_base::beg ); //TODO check if range is available

        } else {
            response.parameter ( header::CONTENT_LENGTH, std::to_string ( file_size ) );
            response.status ( http_status::OK );
        }

        response.istream ( std::move ( is ) );
    }
};
}//namespace mod
}//namespace http
#endif // HTTP_MOD_FILE_H
