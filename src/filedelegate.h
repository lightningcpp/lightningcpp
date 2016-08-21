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
#ifndef FILEDELEGATE_H
#define FILEDELEGATE_H

#include <string>

#include "httprequest.h"
#include "httpresponse.h"

#include <gtest/gtest_prod.h>

namespace http {
namespace delegate {

/**
 * @brief Delegate to handle requests to files.
 <p>This delegate serves files from the filesystem. The request uri has to be absolute, any relative url without leading slash (/) or with a
 reference to the parent directory (..) will be rejected with a BAD_REQUEST status. In general the browser shall resolve the .. in url's.<br/>
 <strong>supported requests:</strong>
 <ul>
    <li>HEAD and GET requests</li>
    <li>create a content-disposition header</li>
    <li>GET byte range</li>
    <li>If_Modified_Since cache control</li>
 </ul>
 <strong>create and bind the file delegate:</strong>
 <pre>
 TODO
 </pre>
 </p>
 */
class FileDelegate {
public:
	/**
	 * @brief FileDelegate CTOR
	 * @param docroot Path to the files.
	 */
	explicit FileDelegate ( const std::string & docroot ) : docroot_ ( docroot ) {}
	FileDelegate ( const FileDelegate& ) = delete;
	FileDelegate ( FileDelegate&& ) = delete;
	FileDelegate& operator= ( const FileDelegate& ) = delete;
	FileDelegate& operator= ( FileDelegate&& ) = delete;
	~FileDelegate() {}

	/**
	 * @brief execute request
	 * @param request
	 * @param response
	 */
	void execute ( HttpRequest & request, HttpResponse & response );

private:
	std::string docroot_;
	void get ( HttpRequest & request, HttpResponse & response, const std::string & filename, const size_t file_size );
	FRIEND_TEST ( FileDelegateTest, ParseFilename );
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
};
}//namespace delegate
}//namespace http
#endif // FILEDELEGATE_H
