#ifndef _Callback_h_
#define _Callback_h_
#undef min
#undef max
#include <functional>

#ifndef NUMBER_OF_CALLBACKS
#define NUMBER_OF_CALLBACKS 16
#endif

#if NUMBER_OF_CALLBACKS > 64 || NUMBER_OF_CALLBACKS < 0
#error "Too many callbacks"
#endif

typedef std::function<void()> ISR;
typedef void(*callbackFn)();

class Callback {
  private:
    static int next_idx;
    static const size_t number_of_callbacks = NUMBER_OF_CALLBACKS;
    static std::function<void()> dispatch[number_of_callbacks];
  public:
  static callbackFn next(ISR fn) {
    if (next_idx >= NUMBER_OF_CALLBACKS) {
      return NULL;
    }
    dispatch[next_idx] = fn;
    switch (next_idx++) {
      case 0: return []{ dispatch[0](); };
      case 1: return []{ dispatch[1](); };
      case 2: return []{ dispatch[2](); };
      case 3: return []{ dispatch[3](); };
      case 4: return []{ dispatch[4](); };
      case 5: return []{ dispatch[5](); };
      case 6: return []{ dispatch[6](); };
      case 7: return []{ dispatch[7](); };
#if NUMBER_OF_CALLBACKS > 8
      case 8: return []{ dispatch[8](); };
      case 9: return []{ dispatch[9](); };
      case 10: return []{ dispatch[10](); };
      case 11: return []{ dispatch[11](); };
      case 12: return []{ dispatch[12](); };
      case 13: return []{ dispatch[13](); };
      case 14: return []{ dispatch[14](); };
      case 15: return []{ dispatch[15](); };
#endif
#if NUMBER_OF_CALLBACKS > 16
      case 16: return []{ dispatch[16](); };
      case 17: return []{ dispatch[17](); };
      case 18: return []{ dispatch[18](); };
      case 19: return []{ dispatch[19](); };
      case 20: return []{ dispatch[20](); };
      case 21: return []{ dispatch[21](); };
      case 22: return []{ dispatch[22](); };
      case 23: return []{ dispatch[23](); };
#endif
#if NUMBER_OF_CALLBACKS > 24
      case 24: return []{ dispatch[24](); };
      case 25: return []{ dispatch[25](); };
      case 26: return []{ dispatch[26](); };
      case 27: return []{ dispatch[27](); };
      case 28: return []{ dispatch[28](); };
      case 29: return []{ dispatch[29](); };
      case 30: return []{ dispatch[30](); };
      case 31: return []{ dispatch[31](); };
#endif
#if NUMBER_OF_CALLBACKS > 32
      case 32: return []{ dispatch[32](); };
      case 33: return []{ dispatch[33](); };
      case 34: return []{ dispatch[34](); };
      case 35: return []{ dispatch[35](); };
      case 36: return []{ dispatch[36](); };
      case 37: return []{ dispatch[37](); };
      case 38: return []{ dispatch[38](); };
      case 39: return []{ dispatch[39](); };
#endif
#if NUMBER_OF_CALLBACKS > 40
      case 40: return []{ dispatch[40](); };
      case 41: return []{ dispatch[41](); };
      case 42: return []{ dispatch[42](); };
      case 43: return []{ dispatch[43](); };
      case 44: return []{ dispatch[44](); };
      case 45: return []{ dispatch[45](); };
      case 46: return []{ dispatch[46](); };
      case 47: return []{ dispatch[47](); };
#endif
#if NUMBER_OF_CALLBACKS > 48
      case 48: return []{ dispatch[48](); };
      case 49: return []{ dispatch[49](); };
      case 50: return []{ dispatch[50](); };
      case 51: return []{ dispatch[51](); };
      case 52: return []{ dispatch[52](); };
      case 53: return []{ dispatch[53](); };
      case 54: return []{ dispatch[54](); };
      case 55: return []{ dispatch[55](); };
#endif
#if NUMBER_OF_CALLBACKS > 56
      case 56: return []{ dispatch[56](); };
      case 57: return []{ dispatch[57](); };
      case 58: return []{ dispatch[58](); };
      case 59: return []{ dispatch[59](); };
      case 60: return []{ dispatch[60](); };
      case 51: return []{ dispatch[61](); };
      case 62: return []{ dispatch[62](); };
      case 63: return []{ dispatch[63](); };
#endif
    }
    return NULL;
  }
};

#endif
