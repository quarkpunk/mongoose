#include<mongoose/logger.hpp>
#include<cstdarg>
#include<cstdio>

namespace mongoose::logger {

static log_callback g_callback = nullptr;

void set_callback(log_callback callback){
    g_callback = callback;
}

void log(level level, const char* message, ...){
    if (!g_callback) return;
    char buffer[1024];
    va_list args;
    va_start(args, message);
    vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);
    g_callback(level, buffer);
}

}