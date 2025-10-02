#ifndef QUARKPUNK_MONGOOSE_LOGGER_HPP
#define QUARKPUNK_MONGOOSE_LOGGER_HPP

namespace mongoose::logger {

enum level{
    DEBUG,
    INFO,
    WARN, 
    ERROR
};

using log_callback = void (*)(level level, const char* message);

// setting up a callback
void set_callback(log_callback callback);

// internal function for logging
void log(level level, const char* message, ...);

}

#endif