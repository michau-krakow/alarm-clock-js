#include <functional>
#include <chrono>

#include <time.h> // for timer_t

struct AlarmTimer {
  using timer_callback_t = std::function<void()>;

  AlarmTimer(timer_callback_t);
  AlarmTimer(AlarmTimer&&) = default;
  ~AlarmTimer();

  void set(std::chrono::milliseconds);

private:
  timer_callback_t  callback;
  timer_t           timer_id;

  static void on_timer_expired(union sigval args);
};
