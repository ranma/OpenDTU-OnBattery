// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022-2024 Thomas Basler and others
 */
#include "WebApi_firmware.h"
#include "Configuration.h"
#include "RestartHelper.h"
#include "WebApi.h"
#include "helper.h"
#include <AsyncJson.h>
#include <Update.h>
#include "esp_partition.h"

void WebApiFirmwareClass::init(AsyncWebServer& server, Scheduler& scheduler)
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;
    using std::placeholders::_4;
    using std::placeholders::_5;
    using std::placeholders::_6;

    server.on("/api/firmware/update", HTTP_POST,
        std::bind(&WebApiFirmwareClass::onFirmwareUpdateFinish, this, _1),
        std::bind(&WebApiFirmwareClass::onFirmwareUpdateUpload, this, _1, _2, _3, _4, _5, _6));

    server.on("/api/firmware/status", HTTP_GET, std::bind(&WebApiFirmwareClass::onFirmwareStatus, this, _1));
}

bool WebApiFirmwareClass::otaSupported() const
{
    const esp_partition_t* pOtaPartition = esp_partition_find_first(
        ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_1, NULL);
    return (pOtaPartition != nullptr);
}

void WebApiFirmwareClass::onFirmwareUpdateFinish(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentials(request)) {
        return;
    }

    // the request handler is triggered after the upload has finished...
    // create the response, add header, and send response

    AsyncWebServerResponse* response = request->beginResponse((Update.hasError()) ? 500 : 200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    RestartHelper.triggerRestart();
}

void WebApiFirmwareClass::onFirmwareUpdateUpload(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final)
{
    if (!WebApi.checkCredentials(request)) {
        return;
    }

    if (!otaSupported()) {
        return request->send(500, "text/plain", "OTA updates not supported");
    }

    // Upload handler chunks in data
    if (!index) {
        if (!request->hasParam("MD5", true)) {
            return request->send(400, "text/plain", "MD5 parameter missing");
        }

        if (!Update.setMD5(request->getParam("MD5", true)->value().c_str())) {
            return request->send(400, "text/plain", "MD5 parameter invalid");
        }

        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) { // Start with max available size
            Update.printError(Serial);
            return request->send(400, "text/plain", "OTA could not begin");
        }
    }

    // Write chunked data to the free sketch space
    if (len) {
        if (Update.write(data, len) != len) {
            return request->send(400, "text/plain", "OTA could not begin");
        }
    }

    if (final) { // if the final flag is set then this is the last frame of data
        if (!Update.end(true)) { // true to set the size to the current progress
            Update.printError(Serial);
            return request->send(400, "text/plain", "Could not end OTA");
        }
    } else {
        return;
    }
}

void WebApiFirmwareClass::onFirmwareStatus(AsyncWebServerRequest* request)
{
    if (!WebApi.checkCredentialsReadonly(request)) {
        return;
    }

    AsyncJsonResponse* response = new AsyncJsonResponse();
    auto& root = response->getRoot();

    root["ota_supported"] = otaSupported();

    WebApi.sendJsonResponse(request, response, __FUNCTION__, __LINE__);
}
