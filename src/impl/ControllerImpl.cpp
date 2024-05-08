// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "impl/ControllerImpl.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

namespace ErrorMsg
{
constexpr const char* ChipError = "ErrorMsg: something is wrong with the chip";
}

namespace
{
/* special bytes used for create payload
 * use unsigned char because of C library, that uses this type
 */
constexpr unsigned char BRIGHTNESS_BYTE = static_cast<unsigned char>(0x09);
constexpr unsigned char ACTION_BYTE = static_cast<unsigned char>(0x02);

constexpr unsigned REPORT_LENGTH = 9;

void sendFeatureReport(hid_device* dev, unsigned char (&payload)[REPORT_LENGTH])
{
	int res = hid_send_feature_report(dev, payload, REPORT_LENGTH);
	if (res == -1)
	{
		std::wcerr << hid_error(dev) << '\n'; // todo: make error/log file
		(void)hid_exit(); //todo: should I delete the call because it calls in destructor of ModelImpl?
		throw std::runtime_error(ErrorMsg::ChipError);
	}
}
}

ControllerImpl::ControllerImpl(IModel::Ptr model)
	: _model(std::move(model))
{
}

void ControllerImpl::setBrightness(std::uint16_t lvl)
{
	assert(0 <= lvl and lvl <= 100);

	unsigned char payload[REPORT_LENGTH] {}; // use unsigned char because of C library, that uses this type
	payload[0] = BRIGHTNESS_BYTE;
	payload[1] = ACTION_BYTE;
	payload[2] = static_cast<unsigned char>(lvl);
	try
	{
		auto dev = _model->getChipHandler();
		sendFeatureReport(dev, payload);
	}
	catch (std::runtime_error& e)
	{
		std::cout << e.what();
	}
}
