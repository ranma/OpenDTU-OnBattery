#pragma once

#include <Arduino.h>
#include "VeDirectData.h"
#include "VeDirectFrameHandler.h"

template<typename T, size_t WINDOW_SIZE>
class MovingAverage {
public:
    MovingAverage()
      : _sum(0)
      , _index(0)
      , _count(0) { }

    void addNumber(T num) {
        if (_count < WINDOW_SIZE) {
            _count++;
        } else {
            _sum -= _window[_index];
        }

        _window[_index] = num;
        _sum += num;
        _index = (_index + 1) % WINDOW_SIZE;
    }

    float getAverage() const {
        if (_count == 0) { return 0.0; }
        return static_cast<float>(_sum) / _count;
    }

private:
    std::array<T, WINDOW_SIZE> _window;
    T _sum;
    size_t _index;
    size_t _count;
};

struct VeDirectHexQueue {
    VeDirectHexRegister _hexRegister;   // hex register
    uint8_t _readPeriod;                // time period in sec until we send the command again
    uint32_t _lastSendTime;             // time stamp in milli sec of last send
    uint8_t _writeSize = 0;
    std::optional<uint32_t> _writeData = std::nullopt;
};

class VeDirectMpptController : public VeDirectFrameHandler<veMpptStruct> {
public:
    VeDirectMpptController() = default;

    void init(int8_t rx, int8_t tx, Print* msgOut,
        bool verboseLogging, uint8_t hwSerialPort);

    using data_t = veMpptStruct;

    void loop() final;

    void setRemoteMode(VeDirectNetworkMode mode);
    void setRemoteChargeVoltageSetPoint(float volt);
    void setRemoteVoltage(float volt);
    void setRemoteTemperature(float degreeCelsius);
    void setRemoteCurrent(float ampere);
    void setRemoteChargeCurrentLimit(float ampere);

private:
    bool hexDataHandler(VeDirectHexData const &data) final;
    bool processTextDataDerived(std::string const& name, std::string const& value) final;
    void frameValidEvent() final;
    void sendNextHexCommandFromQueue(void);
    bool isHexCommandPossible(void);
    MovingAverage<float, 5> _efficiency;

    uint32_t _sendTimeout = 0;          // timeout until we send the next command from the queue
    size_t _sendQueueNr = 0;            // actual queue position;

    // for slow changing values we use a send time period of 4 sec
    #define HIGH_PRIO_COMMAND 1
    #define WRITE_ONLY_COMMAND 0
    std::array<VeDirectHexQueue, 14> _hexQueue {{
         { VeDirectHexRegister::NetworkTotalDcInputPower, HIGH_PRIO_COMMAND, 0 },
         { VeDirectHexRegister::NetworkStatus, 4, 0 },
         { VeDirectHexRegister::DeviceCapabilities, 4, 0 },
         { VeDirectHexRegister::ChargeControllerTemperature, 4, 0 },
         { VeDirectHexRegister::SmartBatterySenseTemperature, 4, 0 },
         { VeDirectHexRegister::BatteryVoltageSetting, 4, 0 },
         { VeDirectHexRegister::BatteryFloatVoltage, 4, 0 },
         { VeDirectHexRegister::BatteryAbsorptionVoltage, 4, 0 },
         { VeDirectHexRegister::ChargeCurrentLimit, 4, 0, 16 },
         { VeDirectHexRegister::NetworkMode, WRITE_ONLY_COMMAND, 0, 8 },
         { VeDirectHexRegister::ChargeVoltageSetPoint, WRITE_ONLY_COMMAND, 0, 16 },
         { VeDirectHexRegister::BatteryChargeCurrent, WRITE_ONLY_COMMAND, 0, 32 },
         { VeDirectHexRegister::BatteryVoltageSense, WRITE_ONLY_COMMAND, 0, 16 },
         { VeDirectHexRegister::BatteryTemperatureSense, WRITE_ONLY_COMMAND, 0, 16 },
    }};
};
