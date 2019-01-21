#include <node.h>

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#define NEW_STRING(str) \
  v8::String::NewFromUtf8(isolate, str, v8::NewStringType::kNormal) \
    .ToLocalChecked()

static void statSync(const v8::FunctionCallbackInfo<v8::Value>& args) {
  v8::Isolate* isolate = args.GetIsolate();

  if (args.Length() < 1) {
    args.GetReturnValue().Set(NEW_STRING("args.Length() < 1"));
    return;
  }

  if (!args[0]->IsString()) {
    args.GetReturnValue().Set(NEW_STRING("!args[0]->IsString()"));
    return;
  }
  v8::Local<v8::String> filepath = v8::Local<v8::String>::Cast(args[0]);
  v8::String::Utf8Value utf8_filepath(isolate, filepath);
  char* char_filepath = *utf8_filepath;

#ifdef _WIN32
  // TODO
#else  // _WIN32

  struct stat stat_struct;
  memset(&stat_struct, 0, sizeof(struct stat));
  if (stat(char_filepath, &stat_struct)) {
    std::string error_string =
      std::string("stat() failed. errno: ") + std::string(strerror(errno));
    args.GetReturnValue().Set(NEW_STRING(error_string.c_str()));
    return;
  }

  v8::Local<v8::Object> stat_object;
#ifdef __APPLE__
  stat_object->Set(NEW_STRING("mtimeNs"),
    v8::BigInt::New(isolate,
      stat_struct.st_mtimespec.tv_nsec));
  stat_object->Set(NEW_STRING("mtimeMs"),
    v8::BigInt::New(isolate,
      stat_struct.st_mtimespec.tv_sec +
      stat_struct.st_mtimespec.tv_nsec * 1000));
  stat_object->Set(NEW_STRING("atimeNs"),
    v8::BigInt::New(isolate,
      stat_struct.st_atimespec.tv_nsec));
  stat_object->Set(NEW_STRING("atimeMs"),
    v8::BigInt::New(isolate,
      stat_struct.st_atimespec.tv_sec +
      stat_struct.st_atimespec.tv_nsec * 1000));
  stat_object->Set(NEW_STRING("ctimeNs"),
    v8::BigInt::New(isolate,
      stat_struct.st_ctimespec.tv_nsec));
  stat_object->Set(NEW_STRING("ctimeMs"),
    v8::BigInt::New(isolate,
      stat_struct.st_ctimespec.tv_sec +
      stat_struct.st_ctimespec.tv_nsec * 1000));
  stat_object->Set(NEW_STRING("birthtimeNs"),
    v8::BigInt::New(isolate,
      stat_struct.st_birthtimespec.tv_nsec));
  stat_object->Set(NEW_STRING("birthtimeMs"),
    v8::BigInt::New(isolate,
      stat_struct.st_birthtimespec.tv_sec +
      stat_struct.st_birthtimespec.tv_nsec * 1000));
#else  // __APPLE__
  stat_object->Set(NEW_STRING("mtimeNs"),
    v8::BigInt::New(isolate,
      stat_struct.mtim.tv_nsec));
  stat_object->Set(NEW_STRING("mtimeMs"),
    v8::BigInt::New(isolate,
      stat_struct.mtim.tv_sec +
      stat_struct.mtim.tv_nsec * 1000));
  stat_object->Set(NEW_STRING("atimeNs"),
    v8::BigInt::New(isolate,
      stat_struct.atim.tv_nsec));
  stat_object->Set(NEW_STRING("atimeMs"),
    v8::BigInt::New(isolate,
      stat_struct.atim.tv_sec +
      stat_struct.atim.tv_nsec * 1000));
  stat_object->Set(NEW_STRING("ctimeNs"),
    v8::BigInt::New(isolate,
      stat_struct.ctim.tv_nsec));
  stat_object->Set(NEW_STRING("ctimeMs"),
    v8::BigInt::New(isolate,
      stat_struct.ctim.tv_sec +
      stat_struct.ctim.tv_nsec * 1000));
  stat_object->Set(NEW_STRING("birthtimeNs"),
    v8::BigInt::New(isolate,
      stat_struct.ctim.tv_nsec));
  stat_object->Set(NEW_STRING("birthtimeMs"),
    v8::BigInt::New(isolate,
      stat_struct.ctim.tv_sec +
      stat_struct.ctim.tv_nsec * 1000));
#endif
  //stat_object->Set(NEW_STRING("mtimeMs"), asdf);
  args.GetReturnValue().Set(stat_object);
#endif  // _WIN32
}

void Initialize(v8::Local<v8::Object> exports) {
  //NODE_SET_METHOD(exports, "hello", Method);
  NODE_SET_METHOD(exports, "statSync", statSync);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize);
