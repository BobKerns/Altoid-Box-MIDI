
#include <Arduino.h>
#include <KeyTracker.h>
#include <debug.h>

static std::string show_bitmap(const uint32_t *bitmap) {
    if (DEBUG_KEYTRACKER) {
        return hex(bitmap[0]) + "/" + hex(bitmap[1]) + "/" + hex(bitmap[2]) + "/" + hex(bitmap[3]);
    } else {
        // Minimize code footprint, assuming compiler optimizations
        return "";
    }
}

// The keys go up
void KeyTracker::up(uint8_t key) {
    auto i = key /WIDTH;
    auto mask = MASK ^ (0x80000000 >> (key % WIDTH));
    bitmap[i] = bitmap[i] & mask;
    if (DEBUG_KEYTRACKER) {
        debug(std::string("OFF KEY ") + std::to_string(channel) + " " +std::to_string(key) + " " + std::to_string(i) + " " + hex(mask) + " " + show_bitmap(bitmap));
    }
}

// And the keys go down
void KeyTracker::down(uint8_t key) {
    auto i = key / WIDTH;
    uint32_t mask = 0x80000000 >> (key % WIDTH);
    bitmap[i] = bitmap[i] | mask;
    if (DEBUG_KEYTRACKER) {
        debug((std::string("ON KEY ") + std::to_string(channel) + " " + std::to_string(key) + " " + std::to_string(i) + " " + hex(mask) + " " + show_bitmap(bitmap)));
    }
}

// Iterate over the keys that are down.
void KeyTracker::doKeys(keyMapper mapper) {
    for (uint8_t i = 0; i < WORDS; i++) {
        auto b = bitmap[i];
        if (b) {
            for (uint8_t j = 0; j < WIDTH; j++) {
                if (b & (0x80000000 >> j)) {
                    auto key = i * WIDTH + j;
                    if (!mapper(key)) {
                        up(key);
                    }
                }
            }
        }
    }
}

bool KeyTracker::allUp() const {
    auto result = !bitmap[0] && !bitmap[1] && !bitmap[2] && !bitmap[3];
    if (DEBUG_KEYTRACKER) {
        if (result) {
            debug("ALLUP YES " + show_bitmap(bitmap));
        } else {
            debug ("ALLUP NO");
        }
    }
    return result;
}
