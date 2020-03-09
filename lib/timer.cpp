#include "debug.h"
#include "timer.h"

#include <stdexcept>
#include <signal.h>
#include <time.h>

/* Alarm callback */
// Warning: this will be called from separate thread!
void AlarmTimer::on_timer_expired(union sigval args) {
    AlarmTimer *pTimer = reinterpret_cast<AlarmTimer*>(args.sival_ptr);

    DBG << "Timer " << pTimer->timer_id << " expired...";
    pTimer->callback();
    delete pTimer;
}

AlarmTimer::AlarmTimer(timer_callback_t callback) {
  this->callback = callback;

  struct sigevent event = {
      sigev_value: { sival_ptr : this },
      sigev_signo: 0,
      sigev_notify: SIGEV_THREAD,
      _sigev_un: {
        _sigev_thread: {
          _function : AlarmTimer::on_timer_expired,
          _attribute: nullptr
        }
      }
  };

  if (0 != timer_create(CLOCK_REALTIME, &event, &this->timer_id))
      throw std::runtime_error("Could not create a timer");

  g_Alarms.insert(this);
  DBG << "Created timer " << timer_id << "\n";
}

AlarmTimer::~AlarmTimer() {
  cancel();
  g_Alarms.erase(this);
}

void AlarmTimer::remove(AlarmTimer* timer) {
  auto iterator = g_Alarms.find(timer);

  if (iterator == g_Alarms.end())
    return;

  delete *iterator;
}

void AlarmTimer::set(std::chrono::time_point<std::chrono::system_clock> deadline)
{
  uint64_t ms =
    std::chrono::duration_cast<std::chrono::milliseconds>
      (deadline.time_since_epoch()).count();

  struct itimerspec new_value = {
      it_interval: {0, 0},
      it_value: {
        tv_sec: ms / 1000,
        tv_nsec: (ms % 1000)*1000 // to nanoseconds
      }
  };

  DBG << "Setting timer " << timer_id << " at " << ms << " ms...";

  if (0 != timer_settime(timer_id, TIMER_ABSTIME, &new_value, nullptr))
    throw std::runtime_error("Could not set timer for given time");

  DBG << "done\n";
}

void AlarmTimer::set(std::chrono::milliseconds ms) {
  struct itimerspec new_value = {
      it_interval: {0, 0},
      it_value: {
        tv_sec: ms.count() / 1000,
        tv_nsec: (ms.count() % 1000)*1000 // to nanoseconds
      }
  };

  DBG << "Setting timer " << timer_id << " for " << ms.count() << " ms...";

  if (0 != timer_settime(timer_id, 0, &new_value, nullptr))
    throw std::runtime_error("Could not set timer for given time");

  DBG << "done\n";
}

void AlarmTimer::cancel() {
  if (0 != timer_delete(timer_id))
    throw std::runtime_error("Could not cancel timer");
}

std::set<AlarmTimer*> AlarmTimer::g_Alarms;
