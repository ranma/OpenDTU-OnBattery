// SPDX-License-Identifier: GPL-2.0-or-later
#include <solarcharger/victron/Provider.h>
#include <battery/Controller.h>
#include "Configuration.h"
#include "PinMapping.h"
#include "MessageOutput.h"
#include "SerialPortManager.h"

namespace SolarChargers::Victron {

bool Provider::init(bool verboseLogging)
{
    const PinMapping_t& pin = PinMapping.get();
    auto controllerCount = 0;

    if (initController(pin.victron_rx, pin.victron_tx, verboseLogging, 1)) {
        controllerCount++;
    }

    if (initController(pin.victron_rx2, pin.victron_tx2, verboseLogging, 2)) {
        controllerCount++;
    }

    if (initController(pin.victron_rx3, pin.victron_tx3, verboseLogging, 3)) {
        controllerCount++;
    }

    return controllerCount > 0;
}

void Provider::deinit()
{
    std::lock_guard<std::mutex> lock(_mutex);

    _controllers.clear();
    for (auto const& o: _serialPortOwners) {
        SerialPortManager.freePort(o.c_str());
    }
    _serialPortOwners.clear();
}

bool Provider::initController(int8_t rx, int8_t tx, bool logging,
        uint8_t instance)
{
    MessageOutput.printf("[VictronMppt Instance %d] rx = %d, tx = %d\r\n",
            instance, rx, tx);

    if (rx < 0) {
        MessageOutput.printf("[VictronMppt Instance %d] invalid pin config\r\n", instance);
        return false;
    }

    String owner("Victron MPPT ");
    owner += String(instance);
    auto oHwSerialPort = SerialPortManager.allocatePort(owner.c_str());
    if (!oHwSerialPort) { return false; }

    _serialPortOwners.push_back(owner);

    auto upController = std::make_unique<VeDirectMpptController>();
    upController->init(rx, tx, &MessageOutput, logging, *oHwSerialPort);
    _controllers.push_back(std::move(upController));
    return true;
}

void Provider::loop()
{
    auto const& config = Configuration.get();
    auto forwardBatteryData = config.SolarCharger.ForwardBatteryData;
    std::lock_guard<std::mutex> lock(_mutex);

    for (auto const& upController : _controllers) {
        upController->loop();
#if 0
        upController->setRemoteMode(VeDirectNetworkMode::EXTERNAL_CONTROL);
        //upController->setRemoteVoltage(12.34);
        upController->setRemoteTemperature(21.34);
        upController->setRemoteChargeVoltageSetPoint(13.45);
        //upController->setRemoteCurrent(0.1);
        upController->setRemoteChargeCurrentLimit(0.5);
#endif
        if (forwardBatteryData) {
            auto batteryStats = Battery.getStats();
            if (batteryStats->isVoltageValid()) {
                upController->setRemoteVoltage(batteryStats->getVoltage());
            }
            if (batteryStats->getTemperature().has_value()) {
                upController->setRemoteTemperature(batteryStats->getTemperature().value());
            }
        }

        if (upController->isDataValid()) {
            _stats->update(upController->getLogId(), upController->getData(), upController->getLastUpdate());
        }
    }
}

} // namespace SolarChargers::Victron
