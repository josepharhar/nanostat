#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("usage: jstat <filename>\n");
    return 1;
  }

  struct stat stat_struct;
  memset(&stat_struct, 0, sizeof(struct stat));
  if (stat(argv[1], &stat_struct)) {
    printf("stat() failed, strerror: %s\n", strerror(errno));
    return 1;
  }

#ifdef __APPLE__
  printf("st_mtimespec.tv_sec:      %ld\n", stat_struct.st_mtimespec.tv_sec);
  printf("st_mtimespec.tv_nsec:     %ld\n", stat_struct.st_mtimespec.tv_nsec);
  printf("st_atimespec.tv_sec:      %ld\n", stat_struct.st_atimespec.tv_sec);
  printf("st_atimespec.tv_nsec:     %ld\n", stat_struct.st_atimespec.tv_nsec);
  printf("st_ctimespec.tv_sec:      %ld\n", stat_struct.st_ctimespec.tv_sec);
  printf("st_ctimespec.tv_nsec:     %ld\n", stat_struct.st_ctimespec.tv_nsec);
  printf("st_birthtimespec.tv_sec:  %ld\n", stat_struct.st_birthtimespec.tv_sec);
  printf("st_birthtimespec.tv_nsec: %ld\n", stat_struct.st_birthtimespec.tv_nsec);
#else  // __APPLE__
  printf("st_mtim.tv_sec:           %ld\n", stat_struct.st_tim.tv_sec);
  printf("st_mtim.tv_nsec:          %ld\n", stat_struct.st_tim.tv_nsec);
  printf("st_atim.tv_sec:           %ld\n", stat_struct.st_tim.tv_sec);
  printf("st_atim.tv_nsec:          %ld\n", stat_struct.st_tim.tv_nsec);
  printf("st_ctim.tv_sec:           %ld\n", stat_struct.st_tim.tv_sec);
  printf("st_ctim.tv_nsec:          %ld\n", stat_struct.st_tim.tv_nsec);
  printf("st_birthtim.tv_sec:       %ld\n", stat_struct.st_tim.tv_sec);
  printf("st_birthtim.tv_nsec:      %ld\n", stat_struct.st_tim.tv_nsec);
#endif

  printf("dev:                      %d\n", stat_struct.st_dev);
  printf("ino:                      %llu\n", stat_struct.st_ino);
  printf("mode:                     %u\n", stat_struct.st_mode);
  printf("nlink:                    %u\n", stat_struct.st_nlink);
  printf("uid:                      %u\n", stat_struct.st_uid);
  printf("gid:                      %u\n", stat_struct.st_gid);
  printf("rdev:                     %d\n", stat_struct.st_rdev);
  printf("blksize:                  %d\n", stat_struct.st_blksize);
  printf("size:                     %lld\n", stat_struct.st_size);
  printf("blocks:                   %lld\n", stat_struct.st_blocks);

  return 0;
}
