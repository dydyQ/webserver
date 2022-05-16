#include <iostream>
#include <thread>
#include <stdlib.h>
#include "Log.h"
#include "util.h"

int main() {
    sylar::Logger::ptr logger(new sylar::Logger);
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));


    sylar::LogEvent::ptr event(new sylar::LogEvent(logger, sylar::LogLevel::DEBUG,
                                                   __FILE__, __LINE__, 0,
                                                   sylar::GetThreadId(), sylar::GetFiberId(),
                                                   time(0)));

    event->getSS() << "hello world";

    logger->log(sylar::LogLevel::DEBUG, event);

    return 0;
}
