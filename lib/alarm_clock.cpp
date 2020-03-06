#include <functional>
#include "debug.h"
#include "timer.h"

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

  Napi::Value value = info[1];
  int32_t milliseconds;
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

  // TODO: memory leak
  AlarmTimer *alarm = new AlarmTimer(expirationFn);
  alarm->set(std::chrono::milliseconds(milliseconds));
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
