#pragma once

#include <Knob.h>
#include <ChannelState.h>
#include <DisplayMgr.h>


extern void onNoteOn(byte cable, byte channel, byte note, byte velocity);
extern void onNoteOff(byte cable, byte channel, byte note, byte velocity);
extern void onProgramChange(byte cable,  byte channel, byte b2);
extern void onKnobChange(const Knob& knob, uint8_t channel, uint32_t pos);
extern void onKnobClick(const Knob& knob, uint8_t channel);
extern void noteMsg(boolean on, byte cable, const char* msg, byte channel, byte note, byte velocity);

extern Knob knobA;
extern Knob knobB;
extern Knob knobC;

using DMenu = Menu<Display>;


extern DMenu programMenu;
extern DMenu kitMenu;