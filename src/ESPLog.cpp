// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022-2024 Thomas Basler and others
 */
#include "ESPLog.h"
#include "MessageOutput.h"

#include <Arduino.h>

ESPLogClass ESPLog;

namespace {

extern "C" {

static void wrap_putchar(char c)
{
    return ESPLog.putchar(c);
}

}; // extern "C"

}; // namespace

ESPLogClass::ESPLogClass()
    : _loopTask(TASK_IMMEDIATE, TASK_FOREVER, std::bind(&ESPLogClass::loop, this))
{
}

void ESPLogClass::init(Scheduler& scheduler)
{
    scheduler.addTask(_loopTask);
    _loopTask.enable();
    ets_install_putc1(wrap_putchar);
}

void ESPLogClass::loop()
{
    _lock.lock();
    int front = 0;
    int used = _buff_pos;
    while (used > front) {
        // Unlock while we call MessageOutput in case something triggers a ESP framework print during that call.
        _lock.unlock();
        MessageOutput.write(reinterpret_cast<uint8_t*>(&_buffer[front]), used);
        _lock.lock();

        front = used;
        used = _buff_pos;
    };
    _buff_pos = 0;
    _lock.unlock();
}

void ESPLogClass::putchar(char c)
{
    std::lock_guard<std::mutex> lock(_lock);
    if (_buff_pos >= sizeof(_buffer)) {
        return;
    }
    _buffer[_buff_pos++] = c;
}
