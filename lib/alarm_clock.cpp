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
