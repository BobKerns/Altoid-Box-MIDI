/**
 * @copyright Copyright (c) 2021 Bob Kerns
 * License: MIT
 */
#include "debug.h"
#include <Arduino.h>

void debug_internal(const std::string &msg) {
    Serial.println(msg.c_str());
}

const char * const DIGITS = "0123456789abcdef";

std::string hex(uint8_t val) {
    if (DEBUG) {
        char buf[3];
        buf[0] = DIGITS[(val & 0xf0) >> 4];
        buf[1] = DIGITS[val & 0x0f];
        buf[2] = '\0';
        return std::string(buf);
    } else {
        // Minimize code footprint, assuming compiler optimizations
        return "";
    }
}

std::string hex(uint16_t val) {
    return hex(static_cast<uint8_t>((val & 0xff00) >> 8))
        + hex(static_cast<uint8_t>(val & 0x00ff));
}

std::string hex(uint32_t val) {
    return hex(static_cast<uint16_t>((val & 0xffff0000) >> 16))
        + hex(static_cast<uint16_t>(val & 0x0000ffff));
}