#include <functional>
#include <chrono>
#include <set>

#include <time.h> // for timer_t

struct AlarmTimer {
  using timer_callback_t = std::function<void()>;

  AlarmTimer(timer_callback_t);
  AlarmTimer(AlarmTimer&&) = default;
  ~AlarmTimer();

  void set(std::chrono::time_point<std::chrono::system_clock>);
  void set(std::chrono::milliseconds);

  void cancel();
  static void remove(AlarmTimer*);

private:
  timer_callback_t  callback;
  timer_t           timer_id;

  /* This will keep track of existing timers so we don't leak memory */
  static std::set<AlarmTimer*> g_Alarms;

  static void on_timer_expired(union sigval args);
};
