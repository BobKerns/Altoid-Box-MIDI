#pragma once

#include "KeyTracker.h"

class ChannelState {
  public:
    const uint8_t channel;
    uint8_t program = 0;
    const char * programName = "(Not set)";
    bool on = true;
    KeyTracker keys;
    ChannelState(uint8_t channel) : channel(channel) {}
};
