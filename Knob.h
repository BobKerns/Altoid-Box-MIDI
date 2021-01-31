#ifndef _Knob_h_
#define _Knob_h_

#include "Callback.h"
#include "Arduino.h"

class Knob;

using knobChangeHandler = std::function<void(Knob&, int, int)>;
using knobPressHandler = std::function<void(Knob&, bool)>;

class Knob {
  public:
  enum Precision {
    NORMAL = 1,
    DOUBLE = 2,
    QUAD = 4,
  };
  private:
    // Pins
    const int pin1;
    const int pin2;
    const int sw;
    // State
    volatile int32_t count = 0;
    volatile int32_t previous_count = 0;
    volatile uint16_t state = 0;
    volatile uint16_t sw_press = 0;
    volatile uint16_t sw_release = 0;
    // Indicates whether we need to call update when polled.
    const uint8_t interruptFlags;
    // Sequential index of knobs.
    static unsigned int next_idx;
    // Index of this knob
    const unsigned int idx;
    // Name of this knob.
    const char *knob_name;
    // Allowed range. The initial values mean "not limited".
    // Range values are inclusive.
    static const int32_t NO_MINIMUM = 0x100000000;
    static const int32_t NO_MAXIMUM = 0x7ffffffff;
    int32_t min_count = NO_MINIMUM;
    int32_t max_count = NO_MAXIMUM;
    bool wrap = false;
    Precision count_precision = QUAD;

    // Callbacks
    knobPressHandler on_press;
    knobPressHandler on_release;
    knobChangeHandler on_change;
    
    // AttachInterrupt but accepts closures, etc.
    static callbackFn localAttachInterrupt(int pin, ISR fn, int mode);
    // Update the state and count.
    void update(void);
    // Determine which pins support interrupts.
    static int calculateInterrupts(int pin1, int pin2, int sw);
    
  public:
    Knob(const char *name, int pin1, int pin2, int sw = -1);
    // Called during setup()
    void start();
    int read();
    inline void poll() { read(); }
    void write(int c);
    Knob &minCount(int c = NO_MINIMUM);
    Knob &maxCount(int c = NO_MAXIMUM);
    
    // Range values are inclusive.
    Knob &range(int lowerBound, int upperBound, bool doWrap);
    
    // Range values are inclusive.
    Knob &range(int lowerBound = NO_MINIMUM, int upperBound = NO_MAXIMUM);
    // Range values are inclusive.
    Knob &range(int lowerBound, bool wrap);
  
    std::tuple<int32_t, int32_t, bool> getRange() const;
    std::tuple<int32_t, int32_t, bool> getRawRange() const;
    
    Knob &precision(Precision p);
    Precision getPrecision() const;
  
    const char * getName() const;
    Knob &name(const char *newName);

    // Handler configuration
    inline Knob& onChange(const knobChangeHandler &handler) {
      on_change = handler;
      return *this;
    }

    inline Knob& onPress(const knobPressHandler &handler) {
      on_press = handler;
      return *this;
    }

    inline Knob& onChange(const knobPressHandler &handler) {
      on_release = handler;
      return *this;
    }
};

#endif
