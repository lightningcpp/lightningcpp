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
#ifndef ERRORDELEGATE_H
#define ERRORDELEGATE_H

#include <memory>
#include <string>

#include "httprequest.h"
#include "httpresponse.h"

namespace http {

template< class HeaderParameter = DefaultParameter >
/**
 * @brief The ErrorDelegate class
 */
class ErrorDelegate {
public:
	/**
	 * @brief Create Error Delegate
	 * @param body the body of the error response.
	 */
	ErrorDelegate ( std::string body ) : body_ ( body ) {}
	/**
	 * @brief execute
	 * @param request
	 * @param response
	 */
	void execute ( HttpRequest & request, HttpResponse& response ) const {
		response << body_;
		HeaderParameter::execute ( request, response );
	}
	/**
	 * @brief bind to server.
	 * @param body the body of the error response.
	 * @return
	 */
	static http_delegate_t bind ( std::string body ) {
		std::shared_ptr< ErrorDelegate > delegate = std::make_shared< ErrorDelegate > ( body );
		return std::bind ( &ErrorDelegate::execute, delegate, std::placeholders::_1, std::placeholders::_2 );
	}

private:
	std::string body_;
};
}//namespace http
#endif // ERRORDELEGATE_H
