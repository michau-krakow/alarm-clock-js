#include "debug.h"
#include "timer.h"

#include <stdexcept>
#include <signal.h>
#include <time.h>

// Warning: this will be called from separate thread!
void AlarmTimer::on_timer_expired(union sigval args) {
    AlarmTimer *pTimer = (AlarmTimer*) args.sival_ptr;

    DBG << "Timer " << pTimer->timer_id << " expired...";
    pTimer->callback();
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

  DBG << "Created timer " << timer_id << "\n";
}

AlarmTimer::~AlarmTimer() {
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

