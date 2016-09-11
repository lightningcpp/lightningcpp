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
#ifndef HTTP_SOCKET_H
#define HTTP_SOCKET_H

namespace http {

/**
 * @brief The Socket class
 */
class Socket {
public:
    Socket ( asio::io_service& io_service ) :
            strand_ ( io_service ), socket_ ( io_service ) {}

    Socket ( const Socket& ) = delete;
    Socket ( Socket&& ) = delete;
    Socket& operator= ( const Socket& ) = delete;
    Socket& operator= ( Socket&& ) = delete;
    ~Socket() {}

    /** Get the socket associated with the connection. */
    auto& socket()
    { return socket_; }

    /**
     * @brief Read from connection.
     * @param buffer
     * @param delegate
     */
    void read ( buffer_t & buffer, auto delegate ) {
            socket_.async_read_some ( asio::buffer ( buffer ), strand_.wrap ( delegate ) );
    }

    /**
     * @brief Write to connection.
     * @param buffer
     * @param size
     * @param delegate
     */
    void write ( buffer_t & buffer, size_t size, auto delegate ) {
            asio::async_write ( socket_, asio::buffer ( buffer, size ), strand_.wrap ( delegate ) );
    }

    /**
     * @brief Close the connection.
     */
    void close() {
            asio::error_code ignored_ec;
            socket_.shutdown ( asio::ip::tcp::socket::shutdown_both, ignored_ec );
    }

private:
    /** Strand to ensure the connection's handlers are not called concurrently. */
    asio::io_service::strand strand_;
    /** Socket for the connection. */
    asio::ip::tcp::socket socket_;
};
typedef std::unique_ptr< Socket > socket_ptr;
}//namespace http
#endif // HTTP_SOCKET_H
