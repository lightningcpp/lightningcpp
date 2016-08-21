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
#ifndef HTTPSESSION_H
#define HTTPSESSION_H

#include <array>
#include <functional>
#include <memory>

#include "httpconfig.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "utils/httpparser.h"

namespace http {

typedef std::function< void ( HttpRequest&, HttpResponse & ) > callback_ptr;
/**
 * @brief The HttpSession manages different types of connection and creates request, response objects.
 */
class HttpSession {
public:
	HttpSession ( callback_ptr callback ) : callback_ ( callback ), request_ ( std::make_shared< HttpRequest >() ) {}

	std::shared_ptr< HttpResponse > parse_request ( buffer_t & buffer, const size_t size );
	std::shared_ptr< HttpRequest > parse_response ( buffer_t & buffer, const size_t size );

	/**
	 * @brief reset the state between requests.
	 */
	void reset();
	/**
	 * @brief is sessoin persistent
	 * @return
	 */
	bool persistent();

private:
	callback_ptr callback_;
	utils::HttpParser http_parser_;
	std::shared_ptr< HttpRequest > request_;
	std::shared_ptr< HttpResponse > response_;
};
}//namespace http
#endif // HTTPSESSION_H
