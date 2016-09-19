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
#ifndef CHUNKED_H
#define CHUNKED_H

#include <array>
#include <sstream>

#include "../constant.h"
#include "../response.h"

namespace http {
namespace utils {

class Chunked {
public:
    Chunked( writer_t writer ) : _writer( writer ) {}

    void write ( buffer_t buffer, size_t index, size_t size ) {

        for( size_t i=index; i<size; ++i ) {
            //search size
            if ( status_ == END ) {
                std::cout << "END, extra character : " << buffer[i] << std::endl;
            } else if ( status_ == CHUNK ) {
                if( chunk_read_ == 0 ) {
                    status_ = START;
                    ss_.str("");
                    ss_.clear();
                    chunk_size_ = 0;
                    chunk_read_ = 0;
                } else {
                    if( chunk_read_ > size - i ) {
                        _writer( buffer.data()+i, size - i );
                        chunk_read_ -= ( size - i );
                        i = size + 1;
                    } else {
                        _writer( buffer.data()+i, chunk_read_ );
                        i = i + chunk_read_;
                        chunk_read_ = 0;
                    }
                }

            } else if( status_ == START && buffer[i] == '\n' ) {
                if( ! ss_.str().empty() ) {
                    status_ = CHUNK;
                    chunk_size_ = std::stoul( ss_.str(), 0, 16 );
                    if( chunk_size_ == 0 ) {
                        status_ = END;
                    } else if( chunk_size_ > size - i ) {
                        _writer( buffer.data()+i+1  /* skip \n */, size - i - 1 );
                        chunk_read_ = chunk_size_ - ( size - i - 1 );
                        return;

                    } else {
                        _writer( buffer.data()+i+1 /* skip \n */, chunk_size_ );
                    }

                    if ( i + chunk_size_ < size ) {
                        i = i + chunk_size_ + 1;
                        ss_.str("");
                        ss_.clear();
                    }
                }
            } else if( buffer[i] != '\r' ) {
                ss_ << buffer[i];
            }
        }
    }

    void read( buffer_t buffer, size_t index, size_t size ) {
        if( size == 0 ) {
            //TODO _reader( "trailer", 0, trailer.size() );
        } else {
            //TODO _reader( "header", 0, header.size() );
            _reader( buffer.data()+index, size );
        }
    }

private:
    enum status { START, CHUNK, END } status_ = START;
    size_t chunk_size_ = 0, chunk_read_ = 0;
    writer_t _writer;
    writer_t _reader;
    std::stringstream ss_;
};
}//namespace utils
}//namespace http
#endif // CHUNKED_H
