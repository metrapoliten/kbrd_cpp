// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "ChipException.h"
#include "impl/ControllerImpl.h"
#include "operator_uc.h"

namespace
{
/* special bytes used for create payload
 * use unsigned char because of C library, that uses this type
 */
constexpr unsigned char kActionByte = 0x02_uc;
constexpr unsigned char kBrightnessByte = 0x09_uc;
constexpr unsigned char kColorByte = 0x14_uc;

constexpr unsigned kReportLength = 9;

void sendFeatureReport(hid_device* dev, unsigned char (&payload)[kReportLength])
{
	int res = hid_send_feature_report(dev, payload, kReportLength);
	if (res == -1)
	{
		std::wcerr << hid_error(dev) << '\n'; // todo: make error/log file
		(void)hid_exit(); //todo: should I delete the call because it calls in destructor of ModelImpl?
		throw ChipException("Can't handle the chip (check log)");
	}
}
}

ControllerImpl::ControllerImpl(IModel::Ptr model)
	: _model(std::move(model))
{
}

void ControllerImpl::setBrightness(uint16_t lvl)
{
	assert(0 <= lvl and lvl <= 100);

	unsigned char payload[kReportLength] {}; // use unsigned char because of C library, that uses this type
	payload[0] = kBrightnessByte;
	payload[1] = kActionByte;
	payload[2] = static_cast<unsigned char>(lvl);
//	try
//	{
		auto dev = _model->getChipHandler();
		sendFeatureReport(dev, payload);
//	}
//	catch (std::runtime_error& e)
//	{
//		std::cout << e.what();
//	}
}

void ControllerImpl::setColor(Color Color)
{
	assert(0 <= Color.R and Color.R <= 255);
	assert(0 <= Color.B and Color.B <= 255);
	assert(0 <= Color.G and Color.G <= 255);
	unsigned char payload[kReportLength] {}; // use unsigned char because of C library, that uses this type
	payload[0] = kColorByte;
	payload[1] = 0x01_uc;
	payload[2] = 0x01_uc;
	payload[3] = static_cast<unsigned char>(Color.R);
	payload[4] = static_cast<unsigned char>(Color.G);
	payload[5] = static_cast<unsigned char>(Color.B);
//	try
//	{
		auto dev = _model->getChipHandler();
		sendFeatureReport(dev, payload);
//	}
//	catch (std::runtime_error& e)
//	{
//		std::cout << e.what();
//	}
}
