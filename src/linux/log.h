#ifndef LOG_H
#define LOG_H

#include <syslog.h>


inline namespace syslog {
inline log( const std::string & message ) {
    openlog ("http", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    syslog (LOG_NOTICE, message );
    //syslog (LOG_INFO, "A tree falls in a forest");

    closelog ();
}
}//namespace syslog

#endif // LOG_H
