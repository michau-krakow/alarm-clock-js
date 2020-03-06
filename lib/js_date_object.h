
#define NAPI_VERSION 4
#define NAPI_CPP_EXCEPTIONS
#include "napi.h"

#include <v8.h>

// This is a wrapper class to deal with Date objects passed in from Js code.
class MaybeDate : public Napi::Value
{
public:
  MaybeDate(const Napi::Value& v) : Napi::Value(v) {}

  bool IsDate() const {
    return toV8LocalValue()->IsDate();
  }

  std::chrono::milliseconds Milliseconds() const {
    double ms = toV8LocalValue().As<v8::Date>()->ValueOf();
    return std::chrono::milliseconds((long int)ms);
  }

private:
  inline const v8::Local<v8::Value> toV8LocalValue() const {
    v8::Local<v8::Value> val;
    memcpy((void*) &val, &_value, sizeof(_value));
    return val;
  }
};
