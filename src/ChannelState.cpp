#include "ChannelState.h"
#include "cables.h"

void ChannelState::sendProgramChanges() {
    auto now = millis();;
    for (int i = 0; i < 16; i++) {
        auto &cs = currentState[i];
        if (cs.send_program_at && cs.send_program_at <= now) {
            cs.sendProgramChange();
        }
    }
}

void ChannelState::sendProgramChange() {
    keys.doKeys([this](uint8_t key){
        CABLE1.sendNoteOff(key, 0, channel + 1);
        return false;
    });
    CABLE1.sendProgramChange(send_program, channel + 1);
    program = send_program;
    programName = send_program_name;
    send_program_at = 0;
}

void ChannelState::queueProgramChange(uint8_t program, const char * programName) {
    send_program_at = millis() + send_delay;
    send_program = program;;
    send_program_name = programName;
    on = true;
}