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
#ifndef HTTP_MOD_MSTCH_H
#define HTTP_MOD_MSTCH_H

#include <string>

#include <mstch/mstch.hpp>

#include "../constant.h"
#include "../request.h"
#include "../response.h"

///@cond DOC_INTERNAL
namespace http {
namespace mod {
namespace _mstch_utils {

}//namespace _mstch_utils
}//namespace mod
}//namespace http
///@endcond DOC_INTERNAL

namespace http {
namespace mod {

class Mstch  {
public:
//    explicit Mstch ( const std::string & t, mstch::map context ) : template_ ( t ), context_( context ) {}
//    Mstch ( const Mstch& ) = delete;
//    Mstch ( Mstch&& ) = default;
//    Mstch& operator= ( const Mstch& ) = delete;
//    Mstch& operator= ( Mstch&& ) = default;
//    ~Mstch() {}

//    http_status execute ( Request&, Response& response ) {
//        response << mstch::render(template_, context_ );
//        return http::http_status::OK;
//    }
//private:
//    const std::string template_;
//    const mstch::map context_;
};
}//namespace mod
}//namespace http
#endif //HTTP_MOD_MSTCH_H
