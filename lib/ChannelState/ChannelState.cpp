/**
 * @copyright Copyright (c) 2021 Bob Kerns
 * License: MIT
 */
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

void ChannelState::allNotesOff() {
    keys.doKeys([this](uint8_t key){
        CABLE1.sendNoteOff(key, 0, channel + 1);
        return false;
    });
}

void ChannelState::sendProgramChange() {
    allNotesOff();
    CABLE1.sendProgramChange(send_program, channel + 1);
    program = send_program;
    programName = send_program_name;
    send_program_at = 0;
}

void ChannelState::queueProgramChange(uint8_t program, const char * programName) {
    send_program_at = millis() + send_delay;
    send_program = program;
    send_program_name = programName;
    on = true;
}

void ChannelState::programChanged(uint8_t pgm) {
    if (!send_program_at) {
        if (program != pgm) {
            if (!keys.allUp()) {
                CABLE1.sendProgramChange(program, channel + 1);
                allNotesOff();
                program = pgm;
                CABLE1.sendProgramChange(program, channel + 1);
            }
        }
    }
}