#pragma once

#include <KeyTracker.h>
#include <Knob.h>
#include <Menu.h>
#include <DisplayMgr.h>
using DMenu = Menu<Display>;

class ChannelState {
    public:
        const uint8_t channel;
        uint8_t program = 0;
        const char * programName = "(Not set)";
        bool on = true;
        Knob *knob;
        KeyTracker keys;
        DMenu *menu = nullptr;
        static ChannelState currentState[16];
        ChannelState(uint8_t channel, Knob *knob = nullptr) : channel(channel), knob(knob), keys(channel) {}
        static void sendProgramChanges();
        void queueProgramChange(uint8_t program, const char * programName);
    private:
        const unsigned int send_delay = 500;
        unsigned long send_program_at = 0;
        uint8_t send_program = 0;
        const char * send_program_name = nullptr;
        void sendProgramChange();
};
