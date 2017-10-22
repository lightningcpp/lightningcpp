#ifndef LOG_H
#define LOG_H

#include <syslog.h>

#include <string>

namespace http {

inline namespace _syslog {
template< typename... Args >
inline void log( const std::string & message, Args... args ) {

    openlog ("http", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog (LOG_NOTICE, message.c_str(), args... );
    //syslog (LOG_INFO, "A tree falls in a forest");
    closelog ();
}
}//namespace _syslog
}//namespace http
#endif // LOG_H
