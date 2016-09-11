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
#ifndef CONSTANT_H
#define CONSTANT_H

namespace http {

/**
 * \brief the method names
 */
namespace method {
static const std::string    CONNECT =   "CONNECT";
static const std::string    DELETE  =   "DELETE";
static const std::string    GET     =   "GET";
static const std::string    HEAD    =   "HEAD";
static const std::string    OPTIONS =   "OPTIONS";
static const std::string    POST    =   "POST";
static const std::string    PUT     =   "PUT";
static const std::string    TRACE   =   "TRACE";
} //method


/**
 * The http status types.
 * @brief http status types.
 */
enum class http_status {
    CONTINUE = 100, //Section 10.1.1: Continue
    SWITCHING_PROTOCOL = 101, //Section 10.1.2: Switching Protocols
    OK = 200, //Section 10.2.1: OK
    CREATED = 201, //Section 10.2.2: Created
    ACCEPTED = 202, //Section 10.2.3: Accepted
    NON_AUTHORATIVE = 203, //Section 10.2.4: Non-Authoritative Information
    NO_CONTENT = 204, //Section 10.2.5: No Content
    RESET_CONTENT = 205, //Section 10.2.6: Reset Content
    PARTIAL_CONTENT = 206, //Section 10.2.7: Partial Content
    MULTIPLE_CHOICES = 300, //Section 10.3.1: Multiple Choices
    MOVED_PERMANENTLY = 301, //Section 10.3.2: Moved Permanently
    FOUND = 302, //Section 10.3.3: Found
    SEE_OTHER = 303, //Section 10.3.4: See Other
    NOT_MODIFIED = 304, //Section 10.3.5: Not Modified
    USE_PROXY = 305, //Section 10.3.6: Use Proxy
    TEMPORARY_REDIRECT = 307, //Section 10.3.8: Temporary Redirect
    BAD_REQUEST = 400, //Section 10.4.1: Bad Request
    UNAUTHORIZED = 401, //Section 10.4.2: Unauthorized
    PAYMENT_REQUIRED = 402, //Section 10.4.3: Payment Required
    FORBIDDEN = 403, //Section 10.4.4: Forbidden
    NOT_FOUND = 404, //Section 10.4.5: Not Found
    METHOD_NOT_ALLOWED = 405, //Section 10.4.6: Method Not Allowed
    NOT_ACCEPTABLE = 406, //Section 10.4.7: Not Acceptable
    PROXY_AUTHENTICATION_REQUIRED = 407, //Section 10.4.8: Proxy Authentication Required
    REQUEST_TIME_OUT = 408, //Section 10.4.9: Request Time-out
    CONFLICT = 409, //Section 10.4.10: Conflict
    GONE = 410, //Section 10.4.11: Gone
    LENGTH_REQUIRED = 411, //Section 10.4.12: Length Required
    PRECONDITION_FAILED = 412, //Section 10.4.13: Precondition Failed
    REQUEST_ENTITY_TOO_LARGE = 413, //Section 10.4.14: Request Entity Too Large
    REQUEST_URI_TOO_LARGE = 414, //Section 10.4.15: Request-URI Too Large
    UNSUPPORTED_MEDIA_TYPE = 415, //Section 10.4.16: Unsupported Media Type
    REQUEST_RANGE_NOT_SATISFIABLE = 416, //Section 10.4.17: Requested range not satisfiable
    EXPECTATION_FAILED = 417, //Section 10.4.18: Expectation Failed
    INTERNAL_SERVER_ERROR = 500, //Section 10.5.1: Internal Server Error
    NOT_IMPLEMENTED = 501, //Section 10.5.2: Not Implemented
    BAD_GATEWAY = 502, //Section 10.5.3: Bad Gateway
    SERVICE_UNAVAILABLE = 503, //Section 10.5.4: Service Unavailable
    GATEWAY_TIMEOUT = 504, //Section 10.5.5: Gateway Time-out
    HTTP_VERSION_NOT_SUPPORTED = 505 //Section 10.5.6: HTTP Version not supported
};

/**
 * @brief Is status a success state.
 * @param s status
 * @return
 */
static bool stat_is_success( http_status s) {
    return ( static_cast< int > ( s ) >= 200 && static_cast< int > ( s ) < 300 );
}

/**
 * @brief Can response have a bosy.
 * @param status status
 * @return  true if response can have a body
 */
static bool stat_has_body( http_status s ) {
    return ( ! ( /* TODO ( request.method() == method::HEAD || */ //error without body
                        s == http_status::NO_CONTENT ||
                        s == http_status::NOT_MODIFIED ||
                        ( static_cast< int > ( s ) >= 100 && static_cast< int > ( s ) <= 199 ) ) );
}

static const std::map< http_status, std::string > status_reason_phrases = {
            { http_status::SWITCHING_PROTOCOL,  "Switching Protocols" },
            { http_status::OK,                  "OK" },
            { http_status::PARTIAL_CONTENT,     "OK" },
            { http_status::CREATED,             "Created" },
            { http_status::ACCEPTED,            "Accepted" },
            { http_status::NO_CONTENT,          "No Content" },
            { http_status::MULTIPLE_CHOICES,    "Multiple Choices" },
            { http_status::MOVED_PERMANENTLY,   "Moved Permanently" },
            { http_status::NOT_MODIFIED,        "Not Modified" },
            { http_status::BAD_REQUEST,         "Bad Request" },
            { http_status::UNAUTHORIZED,        "Unauthorized" },
            { http_status::FORBIDDEN,           "Forbidden" },
            { http_status::NOT_FOUND,           "Not Found" },
            { http_status::INTERNAL_SERVER_ERROR, "Internal Server Error" },
            { http_status::NOT_IMPLEMENTED,     "Not Implemented" },
            { http_status::BAD_GATEWAY,         "Bad Gateway" },
            { http_status::SERVICE_UNAVAILABLE, "Unavailable" } };

}//namespace http
#endif // CONSTANT_H
