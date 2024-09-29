// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022-2024 Thomas Basler and others
 */
#include <HardwareSerial.h>
#include "MessageOutput.h"
#include "SyslogLogger.h"

MessageOutputClass MessageOutput;

MessageOutputClass::MessageOutputClass()
    : _loopTask(1 * TASK_MILLISECOND, TASK_FOREVER, std::bind(&MessageOutputClass::loop, this))
{
}

void MessageOutputClass::init(Scheduler& scheduler)
{
    scheduler.addTask(_loopTask);
    _loopTask.enable();
}

void MessageOutputClass::register_ws_output(AsyncWebSocket* output)
{
    std::lock_guard<std::mutex> lock(_msgLock);

    _ws = output;
}

void MessageOutputClass::serialWrite(MessageOutputClass::message_t const& m)
{
    // operator bool() of HWCDC returns false if the device is not attached to
    // a USB host. in general it makes sense to skip writing entirely if the
    // default serial port is not ready.
    if (!Serial) { return; }

    size_t written = 0;
    while (written < m.size()) {
        written += Serial.write(m.data() + written, m.size() - written);
    }
}

size_t MessageOutputClass::write(uint8_t c)
{
    std::lock_guard<std::mutex> lock(_msgLock);

    auto res = _task_messages.emplace(xTaskGetCurrentTaskHandle(), message_t());
    auto iter = res.first;
    auto& message = iter->second;

    _condition.signal();
    message.push_back(c);

    if (c == '\n') {
        serialWrite(message);
        _lines.emplace(std::move(message));
        _task_messages.erase(iter);
    }

    return 1;
}

size_t MessageOutputClass::write(const uint8_t *buffer, size_t size)
{
    std::lock_guard<std::mutex> lock(_msgLock);

    auto res = _task_messages.emplace(xTaskGetCurrentTaskHandle(), message_t());
    auto iter = res.first;
    auto& message = iter->second;

    message.reserve(message.size() + size);

    for (size_t idx = 0; idx < size; ++idx) {
        uint8_t c = buffer[idx];

        _condition.signal();
        message.push_back(c);

        if (c == '\n') {
            serialWrite(message);
            _lines.emplace(std::move(message));
            message.clear();
            message.reserve(size - idx - 1);
        }
    }

    if (message.empty()) { _task_messages.erase(iter); }

    return size;
}

void MessageOutputClass::loop()
{
    _condition.setWaiting(1);
    _loopTask.waitFor(&_condition);
    std::lock_guard<std::mutex> lock(_msgLock);

    // clean up (possibly filled) buffers of deleted tasks
    auto map_iter = _task_messages.begin();
    while (map_iter != _task_messages.end()) {
        if (eTaskGetState(map_iter->first) == eDeleted) {
            map_iter = _task_messages.erase(map_iter);
            continue;
        }

        ++map_iter;
    }

    if (!_ws) {
        while (!_lines.empty()) {
            Syslog.write(_lines.front().data(), _lines.front().size());
            _lines.pop(); // do not hog memory
        }
        return;
    }

    while (!_lines.empty() && _ws->availableForWriteAll()) {
        Syslog.write(_lines.front().data(), _lines.front().size());
        _ws->textAll(std::make_shared<message_t>(std::move(_lines.front())));
        _lines.pop();
    }
}
