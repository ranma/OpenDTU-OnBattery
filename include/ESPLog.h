// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <stdarg.h>
#include <TaskSchedulerDeclarations.h>
#include <mutex>

#define ESPLOG_BUFFER_SIZE 512

class ESPLogClass {
public:
    ESPLogClass();
    void init(Scheduler& scheduler);
    void putchar(char c);

private:
    void loop();

    Task _loopTask;

    char _buffer[ESPLOG_BUFFER_SIZE];
    uint16_t _buff_pos = 0;
    std::mutex _lock;
};

extern ESPLogClass ESPLog;

