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

  v8::Local<v8::Object> stat_object = v8::Object::New(isolate);
#ifdef __APPLE__
  auto mtimeMs = stat_struct.st_mtimespec.tv_sec;
  auto mtimeNs = stat_struct.st_mtimespec.tv_nsec;
  auto atimeMs = stat_struct.st_atimespec.tv_sec;
  auto atimeNs = stat_struct.st_atimespec.tv_nsec;
  auto ctimeMs = stat_struct.st_ctimespec.tv_sec;
  auto ctimeNs = stat_struct.st_ctimespec.tv_nsec;
  auto birthtimeMs = stat_struct.st_birthtimespec.tv_sec;
  auto birthtimeNs = stat_struct.st_birthtimespec.tv_nsec;
#else  // __APPLE__
  auto mtimeMs = stat_struct.st_mtim.tv_sec;
  auto mtimeNs = stat_struct.st_mtim.tv_nsec;
  auto atimeMs = stat_struct.st_atim.tv_sec;
  auto atimeNs = stat_struct.st_atim.tv_nsec;
  auto ctimeMs = stat_struct.st_ctim.tv_sec;
  auto ctimeNs = stat_struct.st_ctim.tv_nsec;
  auto birthtimeMs = stat_struct.st_ctim.tv_sec;
  auto birthtimeNs = stat_struct.st_ctim.tv_nsec;
#endif  // __APPLE__
  mtimeMs = (mtimeMs * 1000) + (mtimeNs / 1000);
  atimeMs = (atimeMs * 1000) + (atimeNs / 1000);
  ctimeMs = (ctimeMs * 1000) + (ctimeNs / 1000);
  birthtimeMs = (birthtimeMs * 1000) + (birthtimeNs / 1000);
  stat_object->Set(NEW_STRING("mtimeMs"),
      v8::BigInt::New(isolate, mtimeMs));
  stat_object->Set(NEW_STRING("mtimeNs"),
      v8::BigInt::New(isolate, mtimeNs));
  stat_object->Set(NEW_STRING("atimeMs"),
      v8::BigInt::New(isolate, atimeMs));
  stat_object->Set(NEW_STRING("atimeNs"),
      v8::BigInt::New(isolate, atimeNs));
  stat_object->Set(NEW_STRING("ctimeMs"),
      v8::BigInt::New(isolate, ctimeMs));
  stat_object->Set(NEW_STRING("ctimeNs"),
      v8::BigInt::New(isolate, ctimeNs));
  stat_object->Set(NEW_STRING("birthtimeMs"),
      v8::BigInt::New(isolate, birthtimeMs));
  stat_object->Set(NEW_STRING("birthtimeNs"),
      v8::BigInt::New(isolate, birthtimeNs));

  stat_object->Set(NEW_STRING("dev"),
      v8::BigInt::New(isolate, stat_struct.st_dev));
  stat_object->Set(NEW_STRING("ino"),
      v8::BigInt::New(isolate, stat_struct.st_ino));
  stat_object->Set(NEW_STRING("mode"),
      v8::BigInt::New(isolate, stat_struct.st_mode));
  stat_object->Set(NEW_STRING("nlink"),
      v8::BigInt::New(isolate, stat_struct.st_nlink));
  stat_object->Set(NEW_STRING("uid"),
      v8::BigInt::New(isolate, stat_struct.st_uid));
  stat_object->Set(NEW_STRING("gid"),
      v8::BigInt::New(isolate, stat_struct.st_gid));
  stat_object->Set(NEW_STRING("rdev"),
      v8::BigInt::New(isolate, stat_struct.st_rdev));
  stat_object->Set(NEW_STRING("blksize"),
      v8::BigInt::New(isolate, stat_struct.st_blksize));
  stat_object->Set(NEW_STRING("size"),
      v8::BigInt::New(isolate, stat_struct.st_size));
  stat_object->Set(NEW_STRING("blocks"),
      v8::BigInt::New(isolate, stat_struct.st_blocks));

  args.GetReturnValue().Set(stat_object);
#endif  // _WIN32
}

void Initialize(v8::Local<v8::Object> exports) {
  //NODE_SET_METHOD(exports, "hello", Method);
  NODE_SET_METHOD(exports, "statSync", statSync);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize);
