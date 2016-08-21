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

#include "httpsession.h"

namespace http {

std::shared_ptr< HttpResponse > HttpSession::parse_request ( buffer_t & buffer, const size_t size ) {
	size_t body_start_ = http_parser_.parse_request ( *request_.get(), buffer, 0, size );

	if ( body_start_ == 0 ) {
		return nullptr;

	} else {
		if ( ! request_->contains_parameter ( http::header::CONTENT_LENGTH ) || request_->parameter ( http::header::CONTENT_LENGTH ) == "0" ) {
			response_ = std::make_shared< HttpResponse >();

			try {
				callback_ ( *request_.get(), *response_.get() );

			} catch ( http::http_status & status_error ) {
				response_->status ( status_error );
			}

			return response_;

		} else {
			size_t _body_length = std::stoul ( request_->parameter ( http::header::CONTENT_LENGTH ) );

			if ( size - body_start_ <= _body_length ) {
				request_->content ( buffer, body_start_, _body_length );
				response_ = std::make_shared< HttpResponse >( );

				try {
					callback_ ( *request_.get(), *response_.get() );

				} catch ( http::http_status & status_error ) {
					response_->status ( status_error );
				}

				return response_;

			} else {
				request_->content ( buffer, body_start_, size - body_start_ );
				return nullptr;
			}
		}
	}
}

std::shared_ptr< HttpRequest > HttpSession::parse_response ( buffer_t & /* buffer */, const size_t /* size */ ) {
	return nullptr; //TODO

}

void HttpSession::reset() {
	request_.reset ( new HttpRequest() );
	response_.reset();
}

bool HttpSession::persistent() {
	return request_->persistent();
}
}//namespace http
