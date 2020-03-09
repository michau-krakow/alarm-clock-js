#include <functional>
#include <chrono>

#include "debug.h"
#include "timer.h"

// At the time of writing, there is a Napi::Date class available in NAPI version 5.
// However, NAPI 5 have only been released on Nodejs 10.15+ but is not released to Nodejs 12.x nor 13.x.
// TODO: Remove when NAPI version 5 is made available for all Nodejs version.
#include "js_date_object.h"

#define NAPI_VERSION 4
#define NAPI_CPP_EXCEPTIONS
#include "napi.h"


namespace AlarmClock {

Napi::Value SetImmediate(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1) {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return env.Null();
  }

  if (!info[0].IsFunction()) {
    Napi::TypeError::New(env, "Argument should be a function").ThrowAsJavaScriptException();
    return env.Null();
  }

  Napi::Function cb = info[0].As<Napi::Function>();
  cb.Call(env.Global(), {});

  return Napi::Value();
}

Napi::Value SetAlarm(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 2)
    throw Napi::Error::New(env, "Wrong number of arguments");

  if (!info[0].IsFunction())
    throw Napi::TypeError::New(env, "Argument should be a function");

  MaybeDate value(info[1]);
  std::chrono::time_point<std::chrono::system_clock> time;
  int32_t milliseconds;

  if (value.IsDate()) {
    time = std::chrono::time_point<std::chrono::system_clock>(value.Milliseconds());
    if (time < std::chrono::system_clock::now())
      throw Napi::Error::New(env, "Timer expiration date cannot be in the past" );
  }
  else
  if (value.IsNumber()) {
    if ((milliseconds = value.As<Napi::Number>().Int32Value()) <= 0)
      throw Napi::Error::New(env, "A second argument should be greater than zero");
  }
  else
    throw Napi::TypeError::New(env, "Second argument should be a date or number" );

  Napi::Function callback = info[0].As<Napi::Function>();
  Napi::ThreadSafeFunction tsFun =
    Napi::ThreadSafeFunction::New(env, callback, "TimerCallback", /* max_queue_size */ 0, /* initial_thread_count */ 1);

  auto expirationFn = [tsFun]() mutable {
    auto status = tsFun.BlockingCall();
    DBG << "Call Status = " << (status ? "ok" : "failed") << "\n";
    tsFun.Release();
  };

  AlarmTimer *alarm = new AlarmTimer(expirationFn);

  auto cancelationFn = [alarm, tsFun](const Napi::CallbackInfo&) mutable -> Napi::Value {
    AlarmTimer::remove(alarm);
    tsFun.Release();  // no longer required
    return Napi::Value();
  };

  if (value.IsDate())
    alarm->set(time);
  else
    alarm->set(std::chrono::milliseconds(milliseconds));

  return Napi::Function::New(env, cancelationFn);
}

Napi::Object InitModule(Napi::Env env, Napi::Object exports) {
  exports.Set(
    Napi::String::New(env, "setImmediate"),
    Napi::Function::New(env, SetImmediate));

  exports.Set(
    Napi::String::New(env, "setAlarm"),
    Napi::Function::New(env, SetAlarm));

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitModule)
}  // namespace AlarmClock
