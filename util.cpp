//
// Created by dy on 22-5-11.
//
#include "util.h"
#include <unistd.h>
namespace sylar {
    pid_t GetThreadId() {
        return syscall(SYS_gettid);
    }

    uint32_t GetFiberId() {
        return 0;   // 后面再补充
    }
}