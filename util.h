//
// Created by dy on 22-5-11.
//

#ifndef SYLARWEBSERVER_UTIL_H
#define SYLARWEBSERVER_UTIL_H

#include <cxxabi.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>

namespace sylar {
    /**
     * @brief 返回当前线程 ID
     */
    pid_t GetThreadId();

    /**
     * @brief 返回当前协程 Id
     */
    uint32_t GetFiberId();

}

#endif //SYLARWEBSERVER_UTIL_H
