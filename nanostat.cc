#include <node.h>

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <time.h>
#define stat _stat
#define lstat _stat
#endif  // _WIN32

#define NEW_STRING(str) \
  v8::String::NewFromUtf8(isolate, str, v8::NewStringType::kNormal) \
    .ToLocalChecked()

#ifdef _WIN32
struct nanotime {
  uint64_t tv_sec;
  uint64_t tv_nsec;
};
struct nanotime ftToTimespec(FILETIME ft) {
  ULARGE_INTEGER temp;
  temp.HighPart = ft.dwHighDateTime;
  temp.LowPart = ft.dwLowDateTime;
  uint64_t hunnaNs = temp.QuadPart;

  nanotime tv;
  tv.tv_nsec = (hunnaNs * 100) % 1000000000;
  tv.tv_sec = (hunnaNs / 10000000) - 11644473600;
  return tv;
}
#endif  // _WIN32

static uint64_t toMs(uint64_t sec, uint64_t nsec) {
  return (sec * 1000) + ((nsec / 1000000) % 1000);
}

static void statInternal(
    const v8::FunctionCallbackInfo<v8::Value>& args,
    bool follow_symlinks) {

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
  WIN32_FILE_ATTRIBUTE_DATA file_info;
  DWORD retval = GetFileAttributesEx(char_filepath, GetFileExInfoStandard, &file_info);
  if (!retval) {
    // https://docs.microsoft.com/en-us/windows/desktop/debug/retrieving-the-last-error-code
    LPTSTR lpszFunction = "SetFileTime";
    LPVOID lpMsgBuf, lpDisplayBuf;
    DWORD dw = GetLastError();
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    std::string error_string = std::string("GetFileAttributesEx(\"")
      + std::string(char_filepath)
      + std::string("\") failed with error ")
      + std::to_string(dw)
      + std::string(": ")
      + std::string((TCHAR*)lpMsgBuf);
    isolate->ThrowException(v8::Exception::Error(NEW_STRING(error_string.c_str())));
    return;
  }

  /*ULARGE_INTEGER temp;
  temp.HighPart = ftLastWriteTime.dwHighDateTime;
  temp.LowPart = ftLastWriteTime.dwLowDateTime;
  uint64_t hunna = temp.QuadPart;*/

  struct nanotime mtime = ftToTimespec(file_info.ftLastWriteTime);
  struct nanotime atime = ftToTimespec(file_info.ftLastAccessTime);
  struct nanotime ctime = ftToTimespec(file_info.ftCreationTime);

  /*uint64_t nanoseconds = (hunna * 100) % 1000000000;
  uint64_t seconds = (hunna / 10000000) - 11644473600;
  stat_object->Set(NEW_STRING("mtimeMs"),
      v8::BigInt::New(isolate, toMs(seconds, nanoseconds)));
  stat_object->Set(NEW_STRING("mtimeNs"),
      v8::BigInt::New(isolate, nanoseconds));*/
#else  // _WIN32

  struct stat stat_struct;
  memset(&stat_struct, 0, sizeof(struct stat));
  int stat_retval = -1;
  if (follow_symlinks) {
    stat_retval = lstat(char_filepath, &stat_struct);
  } else {
    stat_retval = stat(char_filepath, &stat_struct);
  }
  if (stat_retval) {
    std::string error_string =
      std::string("stat() failed. errno: ") + std::string(strerror(errno));
    isolate->ThrowException(v8::Exception::Error(
        NEW_STRING(error_string.c_str())));
    //args.GetReturnValue().Set(NEW_STRING(error_string.c_str()));
    return;
  }
#endif // _WIN32

  v8::Local<v8::Object> stat_object = v8::Object::New(isolate);
#ifdef __APPLE__
  auto mtimeS = stat_struct.st_mtimespec.tv_sec;
  auto mtimeNs = stat_struct.st_mtimespec.tv_nsec;
  auto atimeS = stat_struct.st_atimespec.tv_sec;
  auto atimeNs = stat_struct.st_atimespec.tv_nsec;
  auto ctimeS = stat_struct.st_ctimespec.tv_sec;
  auto ctimeNs = stat_struct.st_ctimespec.tv_nsec;
  auto birthtimeS = stat_struct.st_birthtimespec.tv_sec;
  auto birthtimeNs = stat_struct.st_birthtimespec.tv_nsec;
#elif _WIN32
  auto mtimeS = mtime.tv_sec;
  auto mtimeNs = mtime.tv_nsec;
  auto atimeS = atime.tv_sec;
  auto atimeNs = atime.tv_nsec;
  auto ctimeS = ctime.tv_sec;
  auto ctimeNs = ctime.tv_nsec;
  auto birthtimeS = ctime.tv_sec;
  auto birthtimeNs = ctime.tv_nsec;
#else
  auto mtimeS = stat_struct.st_mtim.tv_sec;
  auto mtimeNs = stat_struct.st_mtim.tv_nsec;
  auto atimeS = stat_struct.st_atim.tv_sec;
  auto atimeNs = stat_struct.st_atim.tv_nsec;
  auto ctimeS = stat_struct.st_ctim.tv_sec;
  auto ctimeNs = stat_struct.st_ctim.tv_nsec;
  auto birthtimeS = stat_struct.st_ctim.tv_sec;
  auto birthtimeNs = stat_struct.st_ctim.tv_nsec;
#endif  // __APPLE__
  auto mtimeMs = toMs(mtimeS, mtimeNs);
  auto atimeMs = toMs(atimeS, atimeNs);
  auto ctimeMs = toMs(ctimeS, ctimeNs);
  auto birthtimeMs = toMs(birthtimeS, birthtimeNs);
  stat_object->Set(NEW_STRING("mtimeS"),
      v8::BigInt::New(isolate, mtimeS));
  stat_object->Set(NEW_STRING("mtimeMs"),
      v8::BigInt::New(isolate, mtimeMs));
  stat_object->Set(NEW_STRING("mtimeNs"),
      v8::BigInt::New(isolate, mtimeNs));
  stat_object->Set(NEW_STRING("atimeS"),
      v8::BigInt::New(isolate, atimeS));
  stat_object->Set(NEW_STRING("atimeMs"),
      v8::BigInt::New(isolate, atimeMs));
  stat_object->Set(NEW_STRING("atimeNs"),
      v8::BigInt::New(isolate, atimeNs));
  stat_object->Set(NEW_STRING("ctimeS"),
      v8::BigInt::New(isolate, ctimeS));
  stat_object->Set(NEW_STRING("ctimeMs"),
      v8::BigInt::New(isolate, ctimeMs));
  stat_object->Set(NEW_STRING("ctimeNs"),
      v8::BigInt::New(isolate, ctimeNs));
  stat_object->Set(NEW_STRING("birthtimeMs"),
      v8::BigInt::New(isolate, birthtimeMs));
  stat_object->Set(NEW_STRING("birthtimeNs"),
      v8::BigInt::New(isolate, birthtimeNs));

#ifdef _WIN32
  // TODO
  // DWORD dwFileAttributes = file_info.dwFileAttributes;

  ULARGE_INTEGER file_size;
  file_size.HighPart = file_info.nFileSizeHigh;
  file_size.LowPart = file_info.nFileSizeLow;
  stat_object->Set(NEW_STRING("size"),
      v8::BigInt::New(isolate, file_size.QuadPart));

#else  // _WIN32
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
#endif  // _WIN32

  args.GetReturnValue().Set(stat_object);
}

static void lstatSync(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  statInternal(args, /* follow_symlinks */ true);
}

static void statSync(
    const v8::FunctionCallbackInfo<v8::Value>& args) {
  statInternal(args, /* follow_symlinks */ false);
}

void Initialize(v8::Local<v8::Object> exports) {
  NODE_SET_METHOD(exports, "statSync", statSync);
  NODE_SET_METHOD(exports, "lstatSync", lstatSync);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize);
