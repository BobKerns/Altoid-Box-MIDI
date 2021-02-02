#pragma once

class ChannelState {
  public:
    const uint8_t channel;
    uint8_t program = 0;
    const char * programName = "(Not set)";
    bool on = true;
    ChannelState(uint8_t channel) : channel(channel) {}
};
