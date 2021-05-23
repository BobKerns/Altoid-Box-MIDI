#pragma once
#include <string>

const bool DEBUG_KEYTRACKER = false;
const bool DEBUG_MAIN = false;
const bool DEBUG = DEBUG_KEYTRACKER || DEBUG_MAIN;

extern void debug_internal(const std::string &msg);

inline void debug(const std::string &msg) {
    if (DEBUG) {
        debug_internal(msg);
    }
}