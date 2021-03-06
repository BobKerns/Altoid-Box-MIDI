/**
 * @copyright Copyright (c) 2021 Bob Kerns
 * License: MIT
 */
//define KNOB_TRACE
#include "Knob.h"

#ifdef KNOB_TRACE
#include <DisplayMgr.h>
#endif

unsigned int Knob::next_idx = 0;

void Knob::localAttachInterrupt(int pin, ISR fn, int mode) {
    attachInterrupt(digitalPinToInterrupt(pin), Callback::next(fn), mode);
}

// Update the state and count.
void Knob::updateCount(void) {
    rotate_millis = millis();
    uint8_t s = state & 3;
    if (digitalRead(dt)) s |= 4;
    if (digitalRead(clk)) s |= 8;
    switch (s) {
        case 0: case 5: case 10: case 15:
            break;
        case 1: case 7: case 8: case 14:
            count++; break;
        case 2: case 4: case 11: case 13:
            count--; break;
        case 3: case 12:
            count += 2; break;
        default:
            count -= 2; break;
    }
    constrainCount();
    state = (s >> 2);
}

void Knob::constrainCount() {
    if (wrap) {
        auto size = max_count - min_count + 1;
        while (count >= max_count) {
            count -= size;
        }
        while (count < min_count) {
            count += size;
         }
    } else {
        if (count < min_count) {
            count = min_count;
        }
        if (count > max_count) {
            count = max_count;
        }
    }
}

// This runs both at interrupt level and main program level
void Knob::updateSwitch() {
    auto press = !digitalRead(sw);
    // Transitions with 'M' in the I/M collumn must be handled at the main program level by checkSwitch/handlerSwitch
    switch (sw_state) {
        //  -  -   -  I T       ON    -- => PRESSED_DEBOUNCE
        case IDLE:
        if (press) {
            sw_state = PRESSED_DEBOUNCE;
            sw_state_ms = millis();
        }
        break;
        //  1  -   1  I - TIME, ON    -- => PRESSED
        //            I T TIME, OFF   -- => PRESSED_RELEASED_DEBOUNCE
        case PRESSED_DEBOUNCE: {
            auto now = millis();
            if (sw_state_ms + debounce_ms < now) {
                if (press) {
                sw_state = PRESSED;
                } else {
                sw_state = PRESSED_RELEASED_DEBOUNCE;
                sw_state_ms = now;
                }
            }
            break;
        }
        //  1  -   -  M -       ON    P- => PRESSED_HANDLED
        //            M T       OFF   P- => HANDLED_RELEASED_DEBOUNCE
        //            I T       OFF   -- => PRESSED_RELEASED_DEBOUNCE
        case PRESSED:
            if (!press) {
                sw_state = PRESSED_RELEASED_DEBOUNCE;
                sw_state_ms = millis();
            }
            break;
        //  -  -   -  I T       OFF   -- => HANDLED_RELEASED_DEBOUNCE
        case PRESSED_HANDLED:
            if (!press) {
                sw_state = HANDLED_RELEASED_DEBOUNCE;
                sw_state_ms = millis();
            }
            break;
        //  -  1   1  I - TIME, OFF   -- => RELEASED
        //            I T TIME, ON    -- => RELEASED_PRESSED_DEBOUNCE
        case HANDLED_RELEASED_DEBOUNCE: {
        auto now = millis();
            if (sw_state_ms + debounce_ms < now) {
                if (!press) {
                    sw_state = RELEASED;
                } else {
                    sw_state = RELEASED_PRESSED_DEBOUNCE;
                    sw_state_ms = now;
                }
            }
            break;
            }
        //  -  1   -  M T       ON    R- => PRESSED_DEBOUNCE
        //            M -       OFF   R- => IDLE
        //            I T       ON    -- => RELEASED_PRESSED_DEBOUNCE
        case RELEASED:
            if (press) {
                sw_state = RELEASED_PRESSED_DEBOUNCE;
                sw_state_ms = millis();
            }
            break;
        //  1   1  1  M -       -     R- -> PRESSED_DEBOUNCE
        //            I - TIME  -     -- => RELEASED_PRESSED
        case RELEASED_PRESSED_DEBOUNCE:
            if (sw_state_ms + debounce_ms < millis()) {
                sw_state = RELEASED_PRESSED;
            }
            break;
        //  1   1  -  M -       -     RP => PRESSED => PRESSED_HANDLED *
        case RELEASED_PRESSED:
            break;
        //  1   1  1  M -       -     P- => HANDLED_RELEASED_DEBOUNCE
        //            I - TIME  -     -- => PRESSED_RELEASED
        case PRESSED_RELEASED_DEBOUNCE: {
            if (sw_state_ms + debounce_ms < millis()) {
                sw_state = PRESSED_RELEASED;
            }
            break;
        }
        //  1   1  -  M -       ON    PR => PRESSED_HANDLED => IDLE *
        case PRESSED_RELEASED:
            break;
    }
}

// Determine which pins support interrupts.
int Knob::calculateInterrupts(int clk, int dt, int sw) {
    return (digitalPinToInterrupt(clk) != NOT_AN_INTERRUPT ? 1 : 0)
        | (digitalPinToInterrupt(dt) != NOT_AN_INTERRUPT ? 2 : 0)
        | ((sw >= 0 && digitalPinToInterrupt(sw) != NOT_AN_INTERRUPT) ? 4 : 0);
}

Knob::Knob(const char *name, int clk, int dt, int sw):
    knob_name(name), clk(clk), dt(dt), sw(sw), idx(next_idx++), interruptFlags(calculateInterrupts(clk, dt, sw)) {
}

// Called during setup()
void Knob::start(PinMode modeClk, PinMode modeDt, PinMode modeSw) {
    auto setMode = [](int pin, PinMode mode) {
        switch (mode) {
        case NOPULLUP:
            pinMode(pin, INPUT);
            break;
        case PULLUP:
            pinMode(pin, INPUT_PULLUP);
            break;
        case PULLDOWN:
            pinMode(pin, INPUT);
            digitalWrite(pin, LOW);
            break;
        case NONE: ;
        }
    };
    setMode(clk, modeClk);
    setMode(dt, modeDt);
    if (sw >= 0) {
        setMode(sw, modeSw);
    }
    delay(500);
    if (digitalRead(clk)) state |= 1;
    if (digitalRead(dt)) state |= 2;
    auto update = [this]{ updateCount(); };
    localAttachInterrupt(clk, update, CHANGE);
    localAttachInterrupt(dt, update, CHANGE);
    update();
    if (sw >= 0) {
        updateSwitch();
        localAttachInterrupt(
            sw,
            [this](void) -> void {
                updateSwitch();
            },
            CHANGE);
    }
}

int Knob::read() {
  auto resync = [this] {
    switch (count_precision) {
        case Precision::NORMAL:  {
            auto skew = count & 0x3;
            if (skew) {
                unsigned long now = millis();
                if (now - rotate_millis > ROTATE_GUARD_MS) {
                    // Round, unless TDC.
                    switch (skew) {
                        case 1:
                        count = count - 1;
                        constrainCount();
                        break;
                        case 3:
                        count = count + 1;
                        constrainCount();
                        break;
                    }
                }
            }
            break;
        }
        case Precision::DOUBLE: ;
        case Precision::QUAD: ;
    }
  };
  auto handleCount = [this](int32_t val){
    auto x = 4/count_precision;
    auto user_count = (val) / x;
    if (on_change) {
      if (user_count != previous_count) {
        on_change(*this, previous_count, user_count);
        previous_count = user_count;
      }
    }
    return user_count;
  };
  // The main program portion of the state machine. This runs while interrupts are locked,
  // so the dispatch is performed again by handleSwitch to actually run the handlers.
  auto checkSwitch = [this](SwitchState cur_state) -> SwitchState {
    if (sw >= 0) {
      switch (cur_state) {
        //  1  -   -  M -       ON    P- => PRESSED_HANDLED
        //            M T       OFF   P- => HANDLED_RELEASED_DEBOUNCE
        //            I T       OFF   -- => PRESSED_RELEASED_DEBOUNCE
        case PRESSED:
          if (!digitalRead(sw)) {
            sw_state = PRESSED_HANDLED;
          } else {
            sw_state = HANDLED_RELEASED_DEBOUNCE;
            sw_state_ms = millis();
          }
          break;
        //  -  1   -  M T       ON    R- => PRESSED_DEBOUNCE
        //            M -       OFF   R- => IDLE
        //            I T       ON    -- => RELEASED_PRESSED_DEBOUNCE
        case RELEASED:
          if (!digitalRead(sw)) {
            sw_state = PRESSED_DEBOUNCE;
            sw_state_ms = millis();
          } else {
            sw_state = IDLE;
          }
          break;
        //  1   1  -  M -       -     RP => PRESSED => PRESSED_HANDLED *
        case RELEASED_PRESSED:
          sw_state = PRESSED_HANDLED;
          break;
        //  1   1  -  M -       ON    PR => PRESSED_HANDLED => IDLE *
        case PRESSED_RELEASED:
          sw_state = IDLE;
          break;
        //  1   1  1  M -       -     R- -> PRESSED_DEBOUNCE
        //            I - TIME  -     -- => RELEASED_PRESSED
        case RELEASED_PRESSED_DEBOUNCE:
          sw_state = PRESSED_DEBOUNCE;
          break;
        //  1   1  1  M -       -     P- => HANDLED_RELEASED_DEBOUNCE
        //            I - TIME  0     -- => PRESSED_RELEASED
        case PRESSED_RELEASED_DEBOUNCE:
          sw_state = HANDLED_RELEASED_DEBOUNCE;
          break;
        // Handled at interrupt level.
        case PRESSED_DEBOUNCE:
        case PRESSED_HANDLED:
        case HANDLED_RELEASED_DEBOUNCE:
        case IDLE:
            break;
      }
#ifdef KNOB_TRACE
     if (idx == 0) {
        showBodyFor(1000, [this]{ display.printFixed(0, 16, switchState(), STYLE_NORMAL); });
     }
#endif
    }
    return cur_state;
  };
  // Run the switch handlers. The state changes are already performed by checkSwitch w/ interrupts locked.
  // This runs with interrupts enabled.
  auto handleSwitch = [this](SwitchState cur_state){
    switch (cur_state) {
        case PRESSED_RELEASED_DEBOUNCE:
        case PRESSED:
            if (on_press) on_press(*this, true);
            break;
        case RELEASED_PRESSED_DEBOUNCE:
        case RELEASED:
            if (on_release) on_release(*this, false);
            break;
        //  1   1  -  M -       -     RP => PRESSED => PRESSED_HANDLED *
        case RELEASED_PRESSED:
            if (on_release) on_release(*this, false);
            if (on_press) on_press(*this, true);
            break;
        //  1   1  -  M -       ON    PR => PRESSED_HANDLED => IDLE *
        case PRESSED_RELEASED:
            if (on_press) on_press(*this, true);
            if (on_release) on_release(*this, false);
            break;

        // Handled at interrupt level.
        case PRESSED_DEBOUNCE:
        case PRESSED_HANDLED:
        case HANDLED_RELEASED_DEBOUNCE:
        case IDLE:
            break;
    }
  };
    switch (interruptFlags & 0x3) {
        case 0: {
            // No need for synchronization
            resync();
            updateCount();
            auto userCount = handleCount(count);
            if (sw >= 0) {
                updateSwitch(); // Interrupt-level portion
                handleSwitch(checkSwitch(sw_state));  // main-level portion
            }
            return userCount;
        }
        case 1:
        case 2:
        // Need to synchronize and update
            noInterrupts();
            resync();
            updateCount();
            break;
        case 3:
            noInterrupts();
            resync();
    }
    updateSwitch();
    auto val = count;
    // Pick up the current state, and if we're going to call, perform that transition while interrupts are locked.
    auto cur_state = sw_state;
    if (sw >= 0) {
        checkSwitch(sw_state);
    }
    interrupts();
    auto user_count = handleCount(val);
    if (sw >= 0) {
        handleSwitch(cur_state);
    }
    return user_count;
}

void Knob::write(int c) {
    // Keeping as much of the code outside of the locked range of interrupts as possible.
    auto now = millis();
        if (now > rotate_millis + ROTATE_GUARD_MS) {
        auto incr = 4/count_precision;
        auto range = (max_count - min_count);
        auto ncount = (c * incr - min_count) % range + min_count;
        auto ncount_max = ((c + 1) * incr - min_count) % range + min_count;
        // Don't set if it would not be an actual user-level change in the value.
        if (ncount == ncount_max - 1) {
            // Optimized case. Like normal, but anything not equal counts as new.
            noInterrupts();
            if (ncount != count) {
                count = previous_count = c;
                state = 0;
            }
            interrupts();
        } else if (ncount < ncount_max) {
            // Normal case; new counts are either smaller, or ncount_max or larger.
            noInterrupts();
            if (ncount < count || count >= ncount_max) {
                count = previous_count = c;
                state = 0;
            }
            interrupts();
        } else {
            // Max has wrapped around; anything less than our count is new.
            noInterrupts();
            if (count < ncount) {
                count = previous_count = c;
                state = 0;
            }
            interrupts();
        }
    }
}

Knob &Knob::minCount(int c) {
    auto x = 4/count_precision;
    auto nval = c == NO_MINIMUM ? NO_MINIMUM : x * c;
    noInterrupts();
    min_count = nval;
    interrupts();
    return *this;
}
Knob &Knob::maxCount(int c) {
    auto x = 4/count_precision;
    auto nval = c == NO_MAXIMUM ? NO_MAXIMUM : x * c + count_precision - 1;
    noInterrupts();
    max_count = nval;
    interrupts();
    return *this;
}

// Range values are inclusive.
Knob &Knob::range(int lowerBound, int upperBound, bool doWrap) {
    auto x = 4/count_precision;
    auto nLower = lowerBound == NO_MINIMUM ? NO_MINIMUM : lowerBound * x;
    auto nUpper = upperBound == NO_MAXIMUM ? NO_MAXIMUM : upperBound * x + count_precision - 1;
    noInterrupts();
    min_count = nLower;
    max_count = nUpper;
    wrap = doWrap;
    interrupts();
    return *this;
}

// Range values are inclusive.
Knob &Knob::range(int lowerBound, int upperBound) {
    return range(lowerBound, upperBound, wrap);
}

// Range values are inclusive.
Knob &Knob::range(int lowerBound, bool wrap) {
    return range(lowerBound, NO_MAXIMUM, wrap);
}

std::tuple<int32_t, int32_t, bool> Knob::getRange() const {
    auto x = 4/count_precision;
    return std::make_tuple((min_count) / x, (max_count - count_precision + 1) / x, wrap);
}

std::tuple<int32_t, int32_t, bool> Knob::getRawRange() const {
    return std::make_tuple(min_count, max_count, wrap);
}

Knob &Knob::precision(Precision p) {
    // not referenced at interrupt level.
    int32_t minRange, maxRange;
    boolean wrap;
    std::tie(minRange, maxRange, wrap) = getRange();
    count_precision = p;
    // Recalculate the range;
    range(minRange, maxRange, wrap);
    return *this;
}

Knob::Precision Knob::getPrecision() const {
    return count_precision;
}

const char * Knob::getName() const {
    return knob_name;
}

Knob &Knob::name(const char *newName) {
    char *copy = new char[strlen(newName) + 1];
    strcpy(copy, newName);
    knob_name = copy;
    return *this;
}

#ifdef KNOB_TRACE
const char *Knob::switchState() {
    switch (sw_state) {
        case IDLE: return "IDLE";
        case PRESSED: return "PRESSED";
        case PRESSED_DEBOUNCE: return "PRESSED_DEBOUNCE";
        case PRESSED_HANDLED: return "PRESSED_HANDLED";
        case HANDLED_RELEASED_DEBOUNCE: return "H-R-DEBOUNCE";
        case RELEASED: return "RELEASED";
        case RELEASED_PRESSED: return "RELEASED_PRESSED";
        case PRESSED_RELEASED: return "PRESSED_RELEASED";
        case RELEASED_PRESSED_DEBOUNCE: return "R-P-DEBOUNCE";
        case PRESSED_RELEASED_DEBOUNCE: return "P-R-DEBOUNCE";
        default: return ">>UNKNOWN<<";
    }
}
#endif
