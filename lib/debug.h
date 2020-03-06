#ifdef DEBUG
#include <iostream>
#define DBG (std::cout)
#else

struct ZeroOutput {
  template <typename T>
  ZeroOutput& operator<<(const T) { return *this; };

  static ZeroOutput& getLog() {
    static ZeroOutput out;
    return out;
  }
};

#define DBG ZeroOutput::getLog()
#endif
