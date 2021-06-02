/**
 * @copyright Copyright (c) 2021 Bob Kerns
 * License: MIT
 */
#pragma once
#include <string>

// Debugging support.

#ifdef DEBUG_KEYTRACKER
#undef DEBUG_KEYTRACKER
const bool DEBUG_KEYTRACKER = true;
#else
const bool DEBUG_KEYTRACKER = false;
#endif

#ifdef DEBUG_MAIN
#undef DEBUG_MAIN
const bool DEBUG_MAIN = true;
#else
const bool DEBUG_MAIN = false;
#endif

const bool DEBUG = DEBUG_KEYTRACKER || DEBUG_MAIN;

extern void debug_internal(const std::string &msg);

inline void debug(const std::string &msg) {
    if (DEBUG) {
        debug_internal(msg);
    }
}

extern std::string hex(uint8_t val);
extern std::string hex(uint16_t val);
extern std::string hex(uint32_t val);