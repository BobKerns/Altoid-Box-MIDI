
#include <Arduino.h>
#include <KeyTracker.h>
#include <debug.h>

// The keys go up
void KeyTracker::up(uint8_t key) {
    auto i = key / 8;
    auto mask = 0xff ^ (1 << (key % 8));
    bitmap[i] = bitmap[i] & mask;
    if (DEBUG_KEYTRACKER) {
        debug(std::string("OFF KEY ") + std::to_string(channel) + " " +std::to_string(key) + " " + std::to_string(i) + " " + std::to_string(mask) + " " + std::to_string(bitmap[i]));
    }
}

// And the keys go down
void KeyTracker::down(uint8_t key) {
    auto i = key / 8;
    auto mask = 1 << (key % 8);
    bitmap[i] = bitmap[i] | mask;
    if (DEBUG_KEYTRACKER) {
        debug((std::string("ON KEY ") + std::to_string(channel) + " " +std::to_string(key) + " " +std::to_string(key) + " " + std::to_string(i) + " " + std::to_string(mask) + " " + std::to_string(bitmap[i])));
    }
}

// Iterate over the keys that are down.
void KeyTracker::doKeys(keyMapper mapper) {
    for (uint8_t i = 0; i < 16; i++) {
        auto b = bitmap[i];
        if (b) {
            for (uint8_t j = 0; j < 8; j++) {
                if (b & (1 << j)) {
                    auto key = i * 8 + j;
                    if (!mapper(key)) {
                        up(key);
                    }
                }
            }
        }
    }
}
