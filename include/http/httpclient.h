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
#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>

#include <asio.hpp>
#include <asio/ssl.hpp>

#include "request.h"
#include "response.h"
#include "utils/httpparser.h"

namespace http {


template< class Output, class Enable = void >
struct Writer {
    static void result_write ( Output&, buffer_t&, size_t, size_t ) {}
};
//template<> //TODO
//struct Writer< http::HttpRequest > {
//static void result_write( http::HttpRequest & output, buffer_t & buffer, size_t position, size_t size ) {
//    std::cout << "write request buffer: " << size << std::endl;
//}
//};
template< class Output >
struct Writer< Output, typename std::enable_if< std::is_base_of< std::ostream, Output >::value >::type > {
    static void result_write ( Output & output, buffer_t & buffer, size_t position, size_t size ) {
        output.write ( buffer.data() + position, size );

        if ( ! output.good() ) { //TODO
            std::cout << "write failed" << std::endl;
        }
    }
};


class Http {
public:
    Http ( const std::string & host, const std::string & protocol ) : host_ ( host ), protocol_ ( protocol ), io_service(), socket ( io_service ) {}

protected:
    std::string host_;
    std::string protocol_;
    asio::io_service io_service;
    asio::ip::tcp::socket socket;

    void connect() {
        // Get a list of endpoints corresponding to the server name and connect.
        asio::ip::tcp::resolver resolver ( io_service );
        asio::ip::tcp::resolver::query query ( host_, protocol_ );
        asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve ( query );
        asio::connect ( socket, endpoint_iterator );
    }
};

class Https {
public:
    Https ( const std::string & host, const std::string & protocol ) :
        host_ ( host ), protocol_ ( protocol ), io_service(), ctx( asio::ssl::context::sslv23 ), socket( io_service, ctx ) {}

protected:
    void connect() {

        asio::ip::tcp::resolver resolver(io_service);
        asio::ip::tcp::resolver::query query( host_, protocol_ );
        asio::connect( socket.lowest_layer(), resolver.resolve( query ) );
        socket.lowest_layer().set_option( asio::ip::tcp::no_delay( true ) );

        // Perform SSL handshake and verify the remote host's
        // certificate.
        socket.set_verify_mode( asio::ssl::verify_none );
        // socket.set_verify_mode( asio::ssl::verify_peer);
        socket.set_verify_callback( asio::ssl::rfc2818_verification( host_ ) );
        socket.handshake( asio::ssl::stream<asio::ip::tcp::socket>::client );

//        // Get a list of endpoints corresponding to the server name and connect.
//        asio::ip::tcp::resolver resolver ( io_service );
//        asio::ip::tcp::resolver::query query ( host_, protocol_ );
//        asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve ( query );
//        // SSL context
//        asio::ssl::context ctx( asio::ssl::context::sslv23 );
//        ctx.load_verify_file("/etc/ssl/certs/ca-certificates.crt");


//        std::error_code handshake(
//            asio::handshake_type type,
//            const ConstBufferSequence & buffers,
//            boost::system::error_code & ec);


//        socket.set_verify_mode(asio::ssl::verify_peer);
//        socket.set_verify_callback(
//            std::bind(&Https::verify_certificate, this, _1, _2));

//        asio::async_connect(socket.lowest_layer(), endpoint_iterator,
//            std::bind(&Https::handle_connect, this,
//              std::placeholders::_1 ) );
//        //asio::connect ( T::socket, endpoint_iterator );
    }

    bool verify_certificate(bool preverified,
         asio::ssl::verify_context& ctx) {
       // The verify callback can be used to check whether the certificate that is
       // being presented is valid for the peer. For example, RFC 2818 describes
       // the steps involved in doing this for HTTPS. Consult the OpenSSL
       // documentation for more details. Note that the callback is called once
       // for each certificate in the certificate chain, starting from the root
       // certificate authority.

       // In this example we will simply print the certificate's subject name.
       char subject_name[256];
       X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
       X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
       std::cout << "Verifying " << subject_name << "\n";

       return preverified;
     }

    void handle_connect( const std::error_code& error ) {
       if (!error) {
         socket.async_handshake( asio::ssl::stream_base::client,
             std::bind(&Https::handle_handshake, this,
               std::placeholders::_1 ) );
       }
       else
       {
         std::cout << "Connect failed: " << error.message() << "\n";
       }
     }

    void handle_handshake( const std::error_code& error ) {
      if (!error) {
//        std::cout << "Enter message: ";
//        std::cin.getline(request_, max_length);
//        size_t request_length = strlen(request_);

//        asio::async_write(socket_,
//            asio::buffer(request_, request_length),
//            std::bind( &client::handle_write, this,
//              std::placeholders::_1,
//              std::placeholders::_2 ) );
      }
      else {
        std::cout << "Handshake failed: " << error.message() << "\n";
      }
    }

    std::string host_;
    std::string protocol_;
    asio::io_service io_service;
    asio::ssl::context ctx;
    asio::ssl::stream<asio::ip::tcp::socket> socket;
};

template< typename T >
class HttpClient : public T {
public:
    HttpClient ( const std::string & host, const std::string & protocol ) : T( host, protocol ) {}

    template< class Request, class Output >
    Response get ( Request & request, Output & output ) {
//TODO        if ( ! T::socket.is_open() ) {
//TODO			// std::cout << "connect: " << host_ << ":" << protocol_ << std::endl; //TODO cout
            T::connect();
//		}

		write ( request );

		//read response
		Response _response;
		read ( _response, output );
		return _response;
	}

private:
    buffer_t request_buffer_;
    buffer_t buffer_;
    utils::HttpParser http_parser_;

    template< class Request >
    void write ( Request & request ) {
        std::string _hostname = std::string( T::host_ );
        //if( )
        //.append( ":" ).append( T::protocol_ );
        request.parameter ( header::HOST, _hostname );
        request.parameter ( header::CONTENT_LENGTH, std::to_string( request.tellp() ) );
        size_t _position = request.header ( buffer_.data(), BUFFER_SIZE );
        //TODO check that all bytes are written
        size_t _written = 0;
        do {
            _written = T::socket.write_some ( asio::buffer ( buffer_, _position ) );
        } while( _written < _position );
        size_t _body_written = 0;
        do {
            _body_written = T::socket.write_some ( asio::buffer ( request.str(), request.str().length() ) );
        } while( _body_written < request.tellp() );
    }

	template< class Output >
	void read ( Response & response, Output & output ) {
		int _position = 0;
		asio::error_code error;
		int _len;

		do { //TODO when response is garbage
            _len = T::socket.read_some ( asio::buffer ( buffer_ ), error );
			_position = http_parser_.parse_response ( response, buffer_, 0, _len );
		} while (  _position == 0 && !error );

		if ( error && error != asio::error::eof ) { throw error; }

		//TODO do requests checks better
		if ( response.protocol() != "HTTP" ) {
			throw http_status::BAD_REQUEST;
		}

		if ( response.status() != http_status::OK ) {
			if ( /* REQUEST NOT AVAILABLE: request.method() == method::HEAD || */ //error without body
				response.status() == http_status::NO_CONTENT ||
				response.status() == http_status::NOT_MODIFIED ||
				( static_cast< int > ( response.status() ) >= 100 && static_cast< int > ( response.status() ) <= 199 ) ) {
				return;
			}
		}

		size_t _read_content = 0; //the recieved content size

		if ( _len - _position > 0 ) { //read the rest of the buffer
			Writer< Output >::result_write ( output, buffer_, _position, _len - _position );
			_read_content = _len - _position;
		}

		//read body from socket
		size_t _content_length = ( response.contains_parameter ( header::CONTENT_LENGTH ) ?
								   std::stoul ( response.parameter ( header::CONTENT_LENGTH ) ) :
								   0 );

		if ( _content_length > 0 && !error ) {
			while ( _read_content < _content_length && !error ) { //get content
                _len = T::socket.read_some ( asio::buffer ( buffer_ ), error );
				Writer< Output >::result_write ( output, buffer_, 0, _len );
				_read_content += _len;
			};
		}

		if ( error ) { std::cout << "error in reading from socket." << std::endl; } //TODO

		output.flush();
	}
};

template< class Output >
static Response get( const std::string& path, Output& output ) {
    utils::UrlParser p { path };
    if( p.secure() ) {
        HttpClient< Https > _client ( p.host(), p.proto() );
        Request _request ( p.path() );
        return _client.get ( _request, output );
    } else {
        HttpClient< Http > _client ( p.host(), p.proto() );
        Request _request ( p.path() );
        return _client.get ( _request, output );
    }
}
}//namespace http
#endif // HTTPCLIENT_H
