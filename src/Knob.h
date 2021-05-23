#pragma once
#include "Callback.h"
#include "Arduino.h"

class Knob;

using knobChangeHandler = std::function<void(Knob&, int, int)>;
using knobPressHandler = std::function<void(Knob&, bool)>;

class Knob {
  public:
  const unsigned int ROTATE_GUARD_MS = 500; // How long to time out on rotary motion
  enum Precision {
    NORMAL = 1,
    DOUBLE = 2,
    QUAD = 4,
  };
  enum PinMode {
    NOPULLUP,
    PULLUP,
    PULLDOWN,
    NONE // Call pinMode manually before
  };
  private:
    // Name of this knob.
    const char *knob_name;
    // Pins
    const int clk;
    const int dt;
    const int sw;

    // States of the pushbutton debounce handling.
    enum SwitchState {            // --PENDING- I
                                  // ON OFF TIM M T CONDITION   HH TRANSITION TARGET
                                  // == === === = = ==========  == =========================
      IDLE,                       //  -  -   -  I T       ON    -- => PRESSED_DEBOUNCE
      PRESSED_DEBOUNCE,           //  1  -   1  I - TIME, ON    -- => PRESSED
                                  //            I T TIME, OFF   -- => PRESSED_RELEASED_DEBOUNCE
      PRESSED,                    //  1  -   -  M -       ON    P- => PRESSED_HANDLED
                                  //            M T       OFF   P- => HANDLED_RELEASED_DEBOUNCE
                                  //            I T       OFF   -- => PRESSED_RELEASED_DEBOUNCE
      PRESSED_HANDLED,            //  -  -   -  I T       OFF   -- => HANDLED_RELEASED_DEBOUNCE
      HANDLED_RELEASED_DEBOUNCE,  //  -  1   1  I - TIME, OFF   -- => RELEASED
                                  //            I T TIME, ON    -- => RELEASED_PRESSED_DEBOUNCE
      RELEASED,                   //  -  1   -  M T       ON    R- => PRESSED_DEBOUNCE
                                  //            M -       OFF   R- => IDLE
                                  //            I T       ON    -- => RELEASED_PRESSED_DEBOUNCE
      RELEASED_PRESSED_DEBOUNCE,  //  1   1  1  M -       -     R- -> PRESSED_DEBOUNCE
                                  //            I - TIME  -     -- => RELEASED_PRESSED
      RELEASED_PRESSED,           //  1   1  -  M -       -     RP => PRESSED => PRESSED_HANDLED *
      PRESSED_RELEASED_DEBOUNCE,  //  1   1  1  M -       -     P- => HANDLED_RELEASED_DEBOUNCE
                                  //            I - TIME  -     -- => PRESSED_RELEASED
      PRESSED_RELEASED            //  1   1  -  M -       ON    PR => PRESSED_HANDLED => IDLE *
      // PENDING columns:
      //   ON  -- pressed event pending
      //   OFF -- released event pending
      //   TIM -- Timeout pending
      // I/M column
      //   I -- Can happen at Interrupt or Main level
      //   M -- Only at Main level
      // T column (Set Timer)
      //   T if timer should be set taking this transition
      // CONDITION column (all must be satisfied to transition)
      //   ON   -- Switch state is on
      //   OFF  -- Switch state is off
      //   TIME -- Timeout expired
      // HH: handler calls
      //   P -- on_press
      //   R -- on_release
      //   * Two handler calls result in an immediate double transition.
      // The interrupt handler is a partial state machine, omitting the transitions that can
      // only be taken by the main level (i.e. by calling the handler).
    };
    // State
    volatile int32_t count = 0;
    int32_t previous_count = 0;
    // State of the rotatry encoder
    volatile uint16_t state = 0;
    volatile unsigned long rotate_millis = 0;
    // State of the pushbutton switch
    volatile SwitchState sw_state = IDLE;
    volatile unsigned long sw_state_ms = 0;
    // Sequential index of knobs.
    static unsigned int next_idx;
    // Index of this knob
    const unsigned int idx;
    // Indicates whether we need to call update when polled.
    const uint8_t interruptFlags;
    // Allowed range. The initial values mean "not limited".
    // Range values are inclusive.
    static const int32_t NO_MINIMUM = 0x10000000;
    static const int32_t NO_MAXIMUM = 0x7fffffff;
    int32_t min_count = NO_MINIMUM;
    int32_t max_count = NO_MAXIMUM;
    bool wrap = false;
    Precision count_precision = QUAD;

    // Callbacks
    knobPressHandler on_press;
    knobPressHandler on_release;
    knobChangeHandler on_change;

    // AttachInterrupt but accepts closures, etc.
    static void localAttachInterrupt(int pin, ISR fn, int mode);
    // Update the quadrature state and count.
    void updateCount();
    // Update the switch state (debounce, etc.)
    void updateSwitch();
    // Determine which pins support interrupts.
    static int calculateInterrupts(int pin1, int pin2, int sw);

    // Constrain the count to be within the range.
    void constrainCount();

    const uint8_t debounce_ms = 20;

  public:
    Knob(const char *name, int clk, int dt, int sw = -1);
    // Called during setup()
    void start(PinMode mode1 = NOPULLUP, PinMode mode2 = NOPULLUP, PinMode modeSw = NOPULLUP);
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

    inline Knob& onRelease(const knobPressHandler &handler) {
      on_release = handler;
      return *this;
    }

#ifdef KNOB_TRACE
    const char *switchState();
#endif
};
