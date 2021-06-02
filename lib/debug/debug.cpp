#include "debug.h"
#include <Arduino.h>

void debug_internal(const std::string &msg) {
    Serial.println(msg.c_str());
}
