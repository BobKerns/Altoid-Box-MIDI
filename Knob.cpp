#include "Knob.h"

unsigned int Knob::next_idx = 0;

callbackFn Knob::localAttachInterrupt(int pin, ISR fn, int mode) {
  attachInterrupt(digitalPinToInterrupt(pin), Callback::next(fn), mode);
}
// Update the state and count.
void Knob::update(void) {
  rotate_millis = millis();
   uint8_t s = state & 3;
   if (digitalRead(pin1)) s |= 4;
   if (digitalRead(pin2)) s |= 8;
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
   }  {
     if (count < min_count) {
        count = min_count;
     }
     if (count > max_count) {
        count = max_count;
     }
   }
}

// Determine which pins support interrupts.
int Knob::calculateInterrupts(int pin1, int pin2, int sw) {
  return (digitalPinToInterrupt(pin1) != NOT_AN_INTERRUPT ? 1 : 0) | (digitalPinToInterrupt(pin2) != NOT_AN_INTERRUPT ? 2 : 0) | ((sw >= 0 && digitalPinToInterrupt(sw) != NOT_AN_INTERRUPT) ? 4 : 0);
}

Knob::Knob(const char *name, int pin1, int pin2, int sw):
  knob_name(name), pin1(pin1), pin2(pin2), sw(sw), idx(next_idx++), interruptFlags(calculateInterrupts(pin1, pin2, sw)) {
}

// Called during setup()
void Knob::start() {
   if (digitalRead(pin1)) state |= 1;
   if (digitalRead(pin2)) state |= 2;
  localAttachInterrupt(pin1, [this](void) -> void {
    update();
  }, CHANGE);
  localAttachInterrupt(pin2, [this](void) -> void {
    update();
  }, CHANGE);
  if (sw) {
    localAttachInterrupt(sw, [this](void) -> void {
      if (digitalRead(sw)) {
        sw_press++;
      } else {
        sw_release++;
      }
    }, CHANGE);
  }
}
int Knob::read() {
  auto resync = [this] {
    switch (count_precision) {
      case Precision::NORMAL:
      auto skew = count & 0x3;
      if (skew) {
        unsigned long now = millis();
        if (now - rotate_millis > 500) {
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
    }
  };
  auto x = 4/count_precision;
  switch (interruptFlags & 0x3) {
    case 0:
      // No need for synchronization
      resync();
      update();
      return (count + count_precision - 1) / x;
    case 1:
    case 2:
      // Need to synchronize and update
      noInterrupts();
      resync();
      update();
      break;
    case 3:
      noInterrupts();
      resync();
  }
  auto val = count;
  interrupts();
  auto user_count = (val) / x;
  if (on_change) {
    if (user_count != previous_count) {
      on_change(*this, previous_count, user_count);
      previous_count = user_count;
    }
  }
  return user_count;
}

void Knob::write(int c) {
  noInterrupts();
  count = c;
  interrupts();
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
