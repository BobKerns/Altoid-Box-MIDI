#pragma once
#include <functional>

class KeyTracker {
  private:
    uint8_t bitmap[128/8];
  public:
    using keyMapper = std::function<bool(int)>;
    KeyTracker(): bitmap() {}
    void up(uint8_t key);
    void down(uint8_t key);
    void doKeys(keyMapper mapper);
};
