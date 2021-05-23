#pragma once
#include <functional>

class KeyTracker {
  private:
    uint8_t bitmap[128/8];
    uint8_t channel;
  public:
    using keyMapper = std::function<bool(int)>;
    KeyTracker(uint8_t channel): bitmap(), channel(channel) {}
    void up(uint8_t key);
    void down(uint8_t key);
    void doKeys(keyMapper mapper);
};
