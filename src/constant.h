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

#include <functional>
#include <map>
#include <string>

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
    _NO_MATCH = 0, //no match, for internal use
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

///** TODO
// * @brief Can response have a bosy.
// * @param status status
// * @return  true if response can have a body
// */
//static bool stat_has_body( http_status s ) {
//    return ( ! ( /* TODO ( request.method() == method::HEAD || */ //error without body
//                        s == http_status::NO_CONTENT ||
//                        s == http_status::NOT_MODIFIED ||
//                        ( static_cast< int > ( s ) >= 100 && static_cast< int > ( s ) <= 199 ) ) );
//}

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

namespace http {
namespace response {
static const std::string    CREATED                 = "<html><head><title>Created</title></head><body><h1>201 Created</h1></body></html>";
static const std::string    ACCEPTED                = "<html><head><title>Accepted</title></head><body><h1>202 Accepted</h1></body></html>";
static const std::string    NO_CONTENT              = "<html><head><title>No Content</title></head><body><h1>204 Content</h1></body></html>";
static const std::string    MULTIPLE_CHOICES        = "<html><head><title>Multiple Choices</title></head><body><h1>300 Multiple Choices</h1></body></html>";
static const std::string    MOVED_PERMANENTLY       = "<html><head><title>Moved Permanently</title></head><body><h1>301 Moved Permanently</h1></body></html>";
static const std::string    MOVED_TEMPORARILY       = "<html><head><title>Moved Temporarily</title></head><body><h1>302 Moved Temporarily</h1></body></html>";
static const std::string    NOT_MODIFIED            = "<html><head><title>Not Modified</title></head><body><h1>304 Not Modified</h1></body></html>";
static const std::string    BAD_REQUEST             = "<html><head><title>Bad Request</title></head><body><h1>400 Bad Request</h1></body></html>";
static const std::string    UNAUTHORIZED            = "<html><head><title>Unauthorized</title></head><body><h1>401 Unauthorized</h1></body></html>";
static const std::string    FORBIDDEN               = "<html><head><title>Forbidden</title></head><body><h1>403 Forbidden</h1></body></html>";
static const std::string    NOT_FOUND               = "<html><head><title>Not Found</title></head><body><h1>404 Not Found</h1></body></html>";
static const std::string    INTERNAL_SERVER_ERROR   = "<html><head><title>Internal Server Error</title></head><body><h1>500 Internal Server Error</h1></body></html>";
static const std::string    NOT_IMPLEMENTED         = "<html><head><title>Not Implemented</title></head><body><h1>501 Not Implemented</h1></body></html>";
static const std::string    BAD_GATEWAY             = "<html><head><title>Bad Gateway</title></head><body><h1>502 Bad Gateway</h1></body></html>";
static const std::string    SERVICE_UNAVAILABLE     = "<html><head><title>Service Unavailable</title></head><body><h1>503 Service Unavailable</h1></body></html>";
} //response
} //http

namespace http {
namespace header {

/* General General Header Fields */

/** Used to specify directives that MUST be obeyed by all caching mechanisms along the request/response chain */
static const std::string    CACHE_CONTROL       = "Cache-Control"; //Section 14.9
/** What type of connection the user-agent would prefer */
static const std::string    CONNECTION          = "Connection"; //Section 14.10
static const std::string    CONNECTION_KEEP_ALIVE = "keep-alive";
static const std::string    CONNECTION_CLOSE    = "close";
/** The date and time that the message was sent (in "HTTP-date" format as defined by RFC 2616) */
static const std::string    DATE                = "Date"; //Section 14.18
static const std::string    PRAGMA              = "Pragma"; //Section 14.32
static const std::string    TRAILER             = "Trailer"; //Section 14.40
/** Encoding of the message body. */
static const std::string    TRANSFER_ENCODING   = "Transfer-Encoding"; //Section 14.41
static const std::string    UPGRADE             = "Upgrade"; //Section 14.42
static const std::string    VIA                 = "Via"; //Section 14.45
static const std::string    WARNING             = "Warning"; //Section 14.46

/* Request Header Fields */

/** Content-Types that are acceptable for the response */
static const std::string    ACCEPT              = "Accept"; //Section 14.1
static const std::string    ACCEPT_CHARSET      = "Accept-Charset"; //Section 14.2
static const std::string    ACCEPT_ENCODING     = "Accept-Encoding"; //Section 14.3
static const std::string    ACCEPT_LANGUAGE     = "Accept-Language"; //Section 14.4
static const std::string    AUTHORIZATION       = "Authorization"; //Section 14.8
static const std::string    EXPECT              = "Expect"; //Section 14.20
static const std::string    FROM                = "From"; //Section 14.22
/** The domain name of the server (for virtual hosting), and the TCP port number on which the server is listening */
static const std::string    HOST                = "Host"; //Section 14.23
static const std::string    IF_MATCH            = "If-Match"; //Section 14.24
/** Allows a 304 Not Modified to be returned if content is unchanged */
static const std::string    IF_MODIFIED_SINCE   = "If-Modified-Since"; //Section 14.25
static const std::string    IF_NONE_MATCH       = "If-None-Match"; //Section 14.26
static const std::string    IF_RANGE            = "If-Range"; //Section 14.27
static const std::string    IF_UNMODIFIED_SINCE = "If-Unmodified-Since"; //Section 14.28
static const std::string    MAX_FORWARDS        = "Max-Forwards"; //Section 14.31
static const std::string    PROXY_AUTHORIZATION = "Proxy-Authorization"; //Section 14.34
/** Request only part of an entity. Bytes are numbered from 0. */
static const std::string    RANGE               = "Range"; //Section 14.35
static const std::string    REFERER             = "Referer"; //Section 14.36
static const std::string    TE                  = "TE"; //Section 14.39
static const std::string    USER_AGENT          = "User-Agent"; //Section 14.43

/* Response Header Fields */

static const std::string    ACCEPT_RANGES       = "Accept-Ranges"; //Section 14.5
static const std::string    AGE                 = "Age"; //Section 14.6
static const std::string    ETAG                = "ETag"; //Section 14.19
static const std::string    LOCATION            = "Location"; //Section 14.30
static const std::string    PROXY_AUTHENTICATE  = "Proxy-Authenticate"; //Section 14.33
static const std::string    RETRY_AFTER         = "Retry-After"; //Section 14.37
static const std::string    SERVER              = "Server"; //Section 14.38
static const std::string    VARY                = "Vary"; //Section 14.44
static const std::string    WWW_AUTHENTICATE    = "WWW-Authenticate"; //Section 14.47

/* Entity Header Fields */

static const std::string    ALLOW               = "Allow"; //Section 14.7
static const std::string    CONTENT_ENCODING    = "Content-Encoding"; //Section 14.11
static const std::string    CONTENT_LANGUAGE    = "Content-Language"; //Section 14.12
/** The length of the response body in octets (8-bit bytes) */
static const std::string    CONTENT_LENGTH      = "Content-Length"; //Section 14.13
static const std::string    CONTENT_LOCATION    = "Content-Location"; //Section 14.14
static const std::string    CONTENT_MD5         = "Content-MD5"; //Section 14.15
static const std::string    CONTENT_RANGE       = "Content-Range"; //Section 14.16
/** The content type of the content body */
static const std::string    CONTENT_TYPE        = "Content-Type"; //Section 14.17
/** Gives the date/time after which the response is considered stale */
static const std::string    EXPIRES             = "Expires"; //Section 14.21
/** The last modified date for the requested object (in "HTTP-date" format as defined by RFC 2616) */
static const std::string    LAST_MODIFIED       = "Last-Modified"; //Section 14.29
/** An opportunity to raise a "File Download" dialogue box for a known MIME type with binary format or suggest a filename for dynamic content. Quotes are necessary with special characters. */
static const std::string    CONTENT_DISPOSITION =   "Content-Disposition";
} //header
} //http

namespace http {
namespace header {
namespace value {
/** Content-Types that are acceptable for the response */
static const std::string    FORM_URLENCODED     =   "application/x-www-form-urlencoded";
}}}

namespace http {
using namespace std::placeholders;

class Request;
class Response;

static const size_t BUFFER_SIZE = HTTP_BUFFER_SIZE;
static const size_t CLIENT_TIMEOUT = HTTP_CLIENT_TIMEOUT;
static const size_t SERVER_THREAD_POOL_SIZE = HTTP_SERVER_THREAD_POOL_SIZE;

typedef std::array< char, BUFFER_SIZE > buffer_t;

typedef std::function< void( char*, std::streamsize ) > writer_t;
typedef std::function< void ( Request&, Response& ) > http_delegate_t;
typedef std::function< http_status ( http::Request&, http::Response& ) > http_connection_t;
}
#endif // CONSTANT_H
