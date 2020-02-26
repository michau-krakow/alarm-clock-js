#define NAPI_VERSION 4
#define NAPI_CPP_EXCEPTIONS
#include "napi.h"


namespace AlarmClock {

Napi::Value SetImmediate(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
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
