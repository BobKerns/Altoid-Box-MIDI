#pragma once

#include "KeyTracker.h"
#include "Knob.h"

class ChannelState {
  public:
    const uint8_t channel;
    uint8_t program = 0;
    const char * programName = "(Not set)";
    bool on = true;
    KeyTracker keys;
    Knob *knob;
    ChannelState(uint8_t channel, Knob *knob = nullptr) : channel(channel), knob(knob) {}
};
