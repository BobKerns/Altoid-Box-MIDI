/**
 * @copyright Copyright (c) 2021 Bob Kerns
 * License: MIT
 */
#pragma once
#include <functional>

class KeyTracker {
    private:
        static const uint8_t WIDTH = 32;
        static const uint8_t WORDS = 128/WIDTH;
        static const uint32_t MASK = 0xffffffff;
        uint32_t bitmap[WORDS];
        uint8_t channel;
    public:
        using keyMapper = std::function<bool(int)>;
        KeyTracker(uint8_t channel): bitmap(), channel(channel) {}
        void up(uint8_t key);
        void down(uint8_t key);
        void doKeys(keyMapper mapper);
        bool allUp() const;
};
