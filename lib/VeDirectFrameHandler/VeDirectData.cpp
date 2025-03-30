#include "VeDirectData.h"

template<typename T, size_t L>
static frozen::string const& getAsString(frozen::map<T, frozen::string, L> const& values, T val)
{
	auto pos = values.find(val);
	if (pos == values.end()) {
		static constexpr frozen::string dummy("???");
		return dummy;
	}
	return pos->second;
}

/*
 * This function returns the product id (PID) as readable text.
 */
frozen::string const& veStruct::getPidAsString() const
{
	/**
	 * this map is rendered from [1], which is more recent than [2]. Phoenix
	 * inverters are not included in the map. unfortunately, the documents do
	 * not fully align. PID 0xA07F is only present in [1]. PIDs 0xA048, 0xA110,
	 * and 0xA111 are only present in [2]. PIDs 0xA06D and 0xA078 are rev3 in
	 * [1] but rev2 in [2].
	 *
	 * [1] https://www.victronenergy.com/upload/documents/VE.Direct-Protocol-3.33.pdf
	 * [2] https://www.victronenergy.com/upload/documents/BlueSolar-HEX-protocol.pdf
	 */
	static constexpr frozen::map<uint16_t, frozen::string, 105> values = {
		{ 0x0203, "BMV-700" },
		{ 0x0204, "BMV-702" },
		{ 0x0205, "BMV-700H" },
		{ 0x0300, "BlueSolar MPPT 70|15" },
		{ 0xA040, "BlueSolar MPPT 75|50" },
		{ 0xA041, "BlueSolar MPPT 150|35" },
		{ 0xA042, "BlueSolar MPPT 75|15" },
		{ 0xA043, "BlueSolar MPPT 100|15" },
		{ 0xA044, "BlueSolar MPPT 100|30" },
		{ 0xA045, "BlueSolar MPPT 100|50" },
		{ 0xA046, "BlueSolar MPPT 150|70" },
		{ 0xA047, "BlueSolar MPPT 150|100" },
		{ 0xA048, "BlueSolar MPPT 75|50 rev2" },
		{ 0xA049, "BlueSolar MPPT 100|50 rev2" },
		{ 0xA04A, "BlueSolar MPPT 100|30 rev2" },
		{ 0xA04B, "BlueSolar MPPT 150|35 rev2" },
		{ 0xA04C, "BlueSolar MPPT 75|10" },
		{ 0xA04D, "BlueSolar MPPT 150|45" },
		{ 0xA04E, "BlueSolar MPPT 150|60" },
		{ 0xA04F, "BlueSolar MPPT 150|85" },
		{ 0xA050, "SmartSolar MPPT 250|100" },
		{ 0xA051, "SmartSolar MPPT 150|100" },
		{ 0xA052, "SmartSolar MPPT 150|85" },
		{ 0xA053, "SmartSolar MPPT 75|15" },
		{ 0xA054, "SmartSolar MPPT 75|10" },
		{ 0xA055, "SmartSolar MPPT 100|15" },
		{ 0xA056, "SmartSolar MPPT 100|30" },
		{ 0xA057, "SmartSolar MPPT 100|50" },
		{ 0xA058, "SmartSolar MPPT 150|35" },
		{ 0xA059, "SmartSolar MPPT 150|100 rev2" },
		{ 0xA05A, "SmartSolar MPPT 150|85 rev2" },
		{ 0xA05B, "SmartSolar MPPT 250|70" },
		{ 0xA05C, "SmartSolar MPPT 250|85" },
		{ 0xA05D, "SmartSolar MPPT 250|60" },
		{ 0xA05E, "SmartSolar MPPT 250|45" },
		{ 0xA05F, "SmartSolar MPPT 100|20" },
		{ 0xA060, "SmartSolar MPPT 100|20 48V" },
		{ 0xA061, "SmartSolar MPPT 150|45" },
		{ 0xA062, "SmartSolar MPPT 150|60" },
		{ 0xA063, "SmartSolar MPPT 150|70" },
		{ 0xA064, "SmartSolar MPPT 250|85 rev2" },
		{ 0xA065, "SmartSolar MPPT 250|100 rev2" },
		{ 0xA066, "BlueSolar MPPT 100|20" },
		{ 0xA067, "BlueSolar MPPT 100|20 48V" },
		{ 0xA068, "SmartSolar MPPT 250|60 rev2" },
		{ 0xA069, "SmartSolar MPPT 250|70 rev2" },
		{ 0xA06A, "SmartSolar MPPT 150|45 rev2" },
		{ 0xA06B, "SmartSolar MPPT 150|60 rev2" },
		{ 0xA06C, "SmartSolar MPPT 150|70 rev2" },
		{ 0xA06D, "SmartSolar MPPT 150|85 rev3" },
		{ 0xA06E, "SmartSolar MPPT 150|100 rev3" },
		{ 0xA06F, "BlueSolar MPPT 150|45 rev2" },
		{ 0xA070, "BlueSolar MPPT 150|60 rev2" },
		{ 0xA071, "BlueSolar MPPT 150|70 rev2" },
		{ 0xA072, "BlueSolar MPPT 150|45 rev3" },
		{ 0xA073, "SmartSolar MPPT 150|45 rev3" },
		{ 0xA074, "SmartSolar MPPT 75|10 rev2" },
		{ 0xA075, "SmartSolar MPPT 75|15 rev2" },
		{ 0xA076, "BlueSolar MPPT 100|30 rev3" },
		{ 0xA077, "BlueSolar MPPT 100|50 rev3" },
		{ 0xA078, "BlueSolar MPPT 150|35 rev3" },
		{ 0xA079, "BlueSolar MPPT 75|10 rev2" },
		{ 0xA07A, "BlueSolar MPPT 75|15 rev2" },
		{ 0xA07B, "BlueSolar MPPT 100|15 rev2" },
		{ 0xA07C, "BlueSolar MPPT 75|10 rev3" },
		{ 0xA07D, "BlueSolar MPPT 75|15 rev3" },
		{ 0xA07E, "SmartSolar MPPT 100|30 12V" },
		{ 0xA07F, "All-In-1 SmartSolar MPPT 75|15 12V" },
		{ 0xA102, "SmartSolar MPPT VE.Can 150|70" },
		{ 0xA103, "SmartSolar MPPT VE.Can 150|45" },
		{ 0xA104, "SmartSolar MPPT VE.Can 150|60" },
		{ 0xA105, "SmartSolar MPPT VE.Can 150|85" },
		{ 0xA106, "SmartSolar MPPT VE.Can 150|100" },
		{ 0xA107, "SmartSolar MPPT VE.Can 250|45" },
		{ 0xA108, "SmartSolar MPPT VE.Can 250|60" },
		{ 0xA109, "SmartSolar MPPT VE.Can 250|70" },
		{ 0xA10A, "SmartSolar MPPT VE.Can 250|85" },
		{ 0xA10B, "SmartSolar MPPT VE.Can 250|100" },
		{ 0xA10C, "SmartSolar MPPT VE.Can 150|70 rev2" },
		{ 0xA10D, "SmartSolar MPPT VE.Can 150|85 rev2" },
		{ 0xA10E, "SmartSolar MPPT VE.Can 150|100 rev2" },
		{ 0xA10F, "BlueSolar MPPT VE.Can 150|100" },
		{ 0xA110, "SmartSolar MPPT RS 450|100" },
		{ 0xA111, "SmartSolar MPPT RS 450|200" },
		{ 0xA112, "BlueSolar MPPT VE.Can 250|70" },
		{ 0xA113, "BlueSolar MPPT VE.Can 250|100" },
		{ 0xA114, "SmartSolar MPPT VE.Can 250|70 rev2" },
		{ 0xA115, "SmartSolar MPPT VE.Can 250|100 rev2" },
		{ 0xA116, "SmartSolar MPPT VE.Can 250|85 rev2" },
		{ 0xA117, "BlueSolar MPPT VE.Can 150|100 rev2" },
		{ 0xA340, "Phoenix Smart IP43 Charger 12|50 (1+1)" },
		{ 0xA341, "Phoenix Smart IP43 Charger 12|50 (3)" },
		{ 0xA342, "Phoenix Smart IP43 Charger 24|25 (1+1)" },
		{ 0xA343, "Phoenix Smart IP43 Charger 24|25 (3)" },
		{ 0xA344, "Phoenix Smart IP43 Charger 12|30 (1+1)" },
		{ 0xA345, "Phoenix Smart IP43 Charger 12|30 (3)" },
		{ 0xA346, "Phoenix Smart IP43 Charger 24|16 (1+1)" },
		{ 0xA347, "Phoenix Smart IP43 Charger 24|16 (3)" },
		{ 0xA381, "BMV-712 Smart" },
		{ 0xA382, "BMV-710H Smart" },
		{ 0xA383, "BMV-712 Smart Rev2" },
		{ 0xA389, "SmartShunt 500A/50mV" },
		{ 0xA38A, "SmartShunt 1000A/50mV" },
		{ 0xA38B, "SmartShunt 2000A/50mV" },
		{ 0xA3F0, "Smart BuckBoost 12V/12V-50A" },
	};

	return getAsString(values, productID_PID);
}

/*
 * This function returns the firmware version as an integer, disregarding
 * release candidate marks.
 */
uint32_t veStruct::getFwVersionAsInteger() const
{
	if (strlen(firmwareVer_FW) == 0) {
		if (strlen(firmwareVer_FWE) == 0) { return 0; }

		// the firmware version from the FWE field may be preceeded by a zero
		// for padding as per VE.Direct protocol, which is fine for strtoul()
		// when we use a fixed base. however, the postfix (2 chars) might be
		// numeric as well to indicate a beta release, which we must not parse.
		std::string strVer(firmwareVer_FWE, strlen(firmwareVer_FWE) - 2);
		return static_cast<uint32_t>(strtoul(strVer.c_str(), nullptr, 10));
	}

	char const* strVersion = firmwareVer_FW;

	// VE.Direct protocol manual states that the first char can be a non-digit,
	// in which case that char represents a release candidate version
	if (strVersion[0] < '0' || strVersion[0] > '9') { ++strVersion; }

	return static_cast<uint32_t>(strtoul(strVersion, nullptr, 10));
}

/*
 * This function returns the firmware version as readable text.
 */
String veStruct::getFwVersionFormatted() const
{
	if (strlen(firmwareVer_FW) == 0 && strlen(firmwareVer_FWE) == 0) {
		return "n/a";
	}

	if (strlen(firmwareVer_FWE) > 0) {
		char const* strVersion = firmwareVer_FWE;

		// the firmware version from the FWE field may be preceeded by a zero
		// for padding as per VE.Direct protocol.
		while (strVersion[0] == '0') { ++strVersion; }

		String res(strVersion[0]);
		strVersion++;
		res += ".";
		res += strVersion[0];
		strVersion++;
		res += strVersion[0];
		strVersion++;

		String suffix(strVersion);
		suffix.toUpperCase();
		if (suffix == "FF") { return res; }

		res += "-beta-";
		res += suffix;
		return res;
	}

	char const* strVersion = firmwareVer_FW;
	char rc = 0;

	// VE.Direct protocol manual states that the first char can be a non-digit,
	// in which case that char represents a release candidate version
	if (strVersion[0] < '0' || strVersion[0] > '9') {
		rc = strVersion[0];
		++strVersion;
	}

	// SmartShunt firmware version is transmitted with leading zero(es)
	while (strVersion[0] == '0') { ++strVersion; }

	String res(strVersion[0]);
	res += ".";
	res += strVersion + 1;

	if (rc != 0) {
		res += "-rc-";
		res += rc;
	}

	return res;
}

/*
 * This function returns the state of operations (CS) as readable text.
 */
frozen::string const& veMpptStruct::getCsAsString() const
{
	static constexpr frozen::map<uint8_t, frozen::string, 9> values = {
		{ 0,   "OFF" },
		{ 2,   "Fault" },
		{ 3,   "Bulk" },
		{ 4,   "Absorption" },
		{ 5,   "Float" },
		{ 7,   "Equalize (manual)" },
		{ 245, "Starting-up" },
		{ 247, "Auto equalize / Recondition" },
		{ 252, "External Control" }
	};

	return getAsString(values, currentState_CS);
}

/*
 * This function returns the state of MPPT (MPPT) as readable text.
 */
frozen::string const& veMpptStruct::getMpptAsString() const
{
	static constexpr frozen::map<uint8_t, frozen::string, 3> values = {
		{ 0, "OFF" },
		{ 1, "Voltage or current limited" },
		{ 2, "MPP Tracker active" }
	};

	return getAsString(values, stateOfTracker_MPPT);
}

/*
 * This function returns error state (ERR) as readable text.
 */
frozen::string const& veMpptStruct::getErrAsString() const
{
	static constexpr frozen::map<uint8_t, frozen::string, 20> values = {
		{ 0,   "No error" },
		{ 2,   "Battery voltage too high" },
		{ 17,  "Charger temperature too high" },
		{ 18,  "Charger over current" },
		{ 19,  "Charger current reversed" },
		{ 20,  "Bulk time limit exceeded" },
		{ 21,  "Current sensor issue(sensor bias/sensor broken)" },
		{ 26,  "Terminals overheated" },
		{ 28,  "Converter issue (dual converter models only)" },
		{ 33,  "Input voltage too high (solar panel)" },
		{ 34,  "Input current too high (solar panel)" },
		{ 38,  "Input shutdown (due to excessive battery voltage)" },
		{ 39,  "Input shutdown (due to current flow during off mode)" },
		{ 40,  "Input" },
		{ 65,  "Lost communication with one of devices" },
		{ 67,  "Synchronised charging device configuration issue" },
		{ 68,  "BMS connection lost" },
		{ 116, "Factory calibration data lost" },
		{ 117, "Invalid/incompatible firmware" },
		{ 118, "User settings invalid" }
	};

	return getAsString(values, errorCode_ERR);
}

/*
 * This function returns the off reason (OR) as readable text.
 */
frozen::string const& veMpptStruct::getOrAsString() const
{
	static constexpr frozen::map<uint32_t, frozen::string, 10> values = {
		{ 0x00000000, "Not off" },
		{ 0x00000001, "No input power" },
		{ 0x00000002, "Switched off (power switch)" },
		{ 0x00000004, "Switched off (device moderegister)" },
		{ 0x00000008, "Remote input" },
		{ 0x00000010, "Protection active" },
		{ 0x00000020, "Paygo" },
		{ 0x00000040, "BMS" },
		{ 0x00000080, "Engine shutdown detection" },
		{ 0x00000100, "Analysing input voltage" }
	};

	return getAsString(values, offReason_OR);
}

/*
 * This function returns the network status as readable text.
 */
frozen::string const& veMpptStruct::getNetworkStatusAsString() const
{
	static constexpr frozen::map<uint8_t, frozen::string, 5> values = {
		{ 0, "BMS or ESS controlled" },
		{ 1, "Group primary" },
		{ 2, "Instance primary" },
		{ 3, "Group and instance primary" },
		{ 4, "Stand-alone" },
	};

	// Clear high nibble, which has sense status bits.
	uint8_t status = NetworkStatus.second & 0x0f;
	return getAsString(values, status);
}

frozen::string const& VeDirectHexData::getResponseAsString() const
{
	using Response = VeDirectHexResponse;
	static constexpr frozen::map<Response, frozen::string, 7> values = {
		{ Response::DONE, "Done" },
		{ Response::UNKNOWN, "Unknown" },
		{ Response::ERROR, "Error" },
		{ Response::PING, "Ping" },
		{ Response::GET, "Get" },
		{ Response::SET, "Set" },
		{ Response::ASYNC, "Async" }
	};

	return getAsString(values, rsp);
}

frozen::string const& VeDirectHexData::getRegisterAsString() const
{
	using Register = VeDirectHexRegister;
	static constexpr frozen::map<Register, frozen::string, 28> values = {
		{ Register::DeviceCapabilities, "Device Capabilities" },
		{ Register::DeviceMode, "Device Mode" },
		{ Register::DeviceState, "Device State" },
		{ Register::RemoteControlUsed, "Remote Control Used" },
		{ Register::PanelVoltage, "Panel Voltage" },
		{ Register::PanelPower, "Panel Power" },
		{ Register::PanelCurrent, "Panel Current" },
		{ Register::ChargerVoltage, "Charger Voltage" },
		{ Register::ChargerCurrent, "Charger Current" },
		{ Register::NetworkTotalDcInputPower, "Network Total DC Input Power" },
		{ Register::ChargeControllerTemperature, "Charger Controller Temperature" },
		{ Register::SmartBatterySenseTemperature, "Smart Battery Sense Temperature" },
		{ Register::NetworkInfo, "Network Info" },
		{ Register::NetworkMode, "Network Mode" },
		{ Register::NetworkStatus, "Network Status" },
		{ Register::BatteryVoltage, "Battery Voltage" },
		{ Register::BatteryVoltageSetting, "Battery Voltage Setting" },
		{ Register::BatteryAbsorptionVoltage, "Battery Absorption Voltage" },
		{ Register::BatteryFloatVoltage, "Battery Float Voltage" },
		{ Register::TotalChargeCurrent, "Total Charge Current" },
		{ Register::ChargeStateElapsedTime, "Charge State Elapsed Time" },
		{ Register::BatteryVoltageSense, "Battery Voltage Sense" },
		{ Register::BatteryTemperatureSense, "Battery Temperature Sense" },
		{ Register::BatteryChargeCurrent, "Battery Charge Current" },
		{ Register::ChargeCurrentLimit, "Charge Current Limit" },
		{ Register::ChargeVoltageSetPoint, "Charge Voltage Set Point" },
		{ Register::LoadCurrent, "Load current" },
		{ Register::LoadOutputVoltage, "Load Output Voltage" }
	};

	return getAsString(values, addr);
}
