// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

#include "impl/ModelImpl.h"

namespace ErrorMsg
{
constexpr const char* ChipError = "ErrorMsg: something is wrong with the chip";
}

namespace
{
constexpr unsigned short VENDOR_ID = 0x048d;
constexpr unsigned short PRODUCT_ID = 0x6006;

constexpr unsigned char GET_EFFECT_BYTE = static_cast<unsigned char>(0x88);
constexpr unsigned char GET_MONOCOLOR_BYTE = static_cast<unsigned char>(148);

constexpr unsigned REPORT_LENGTH = 9;

void logAndThrowRuntimeErr(hid_device* dev)
{
	std::wcerr << hid_error(dev) << '\n'; // todo: make error/log file
	throw std::runtime_error(ErrorMsg::ChipError);
}

void checkReportError(int res, hid_device* dev)
{
	if (res == -1)
	{
		logAndThrowRuntimeErr(dev);
	}
}
void init()
{
	const int res = hid_init();
	checkReportError(res, nullptr);
}

void getReport(hid_device* dev, unsigned char controlByte, unsigned char* buf)
{
	buf[0] = controlByte;
	int res = hid_send_feature_report(dev, buf, REPORT_LENGTH);
	checkReportError(res, dev);
	std::memset(buf, 0, REPORT_LENGTH);
	buf[0] = 0;
	res = hid_get_feature_report(dev, buf, REPORT_LENGTH);
	checkReportError(res, dev);
}

//todo: ask -> collect
int askBrightness(hid_device* dev)
{
	assert(dev);

	unsigned char buf[REPORT_LENGTH];
	getReport(dev, GET_EFFECT_BYTE, buf);
	return buf[5];
}

void askRGB(hid_device* dev, Color &Color)
{
	assert(dev);

	unsigned char hidBuf[REPORT_LENGTH];
	getReport(dev, GET_MONOCOLOR_BYTE, hidBuf);

	Color.R = hidBuf[4];
	Color.B = hidBuf[5];
	Color.G = hidBuf[6];
}

hid_device* openDevice()
{
	auto dev = hid_open(VENDOR_ID, PRODUCT_ID, nullptr);
	if (!dev)
	{
		logAndThrowRuntimeErr(nullptr);
	}
	return dev;
}
}

ModelImpl::ModelImpl()
	: _dev { openDevice() }
	, _brightness {}
	, _rgb {}
{
	try
	{
		init();
		_brightness = askBrightness(_dev); //todo: make one function with init and add in init string
		askRGB(_dev, ModelImpl::_rgb);
	}
	catch (std::runtime_error& e)
	{
		throw;
	}
}

hid_device* ModelImpl::getChipHandler()
{
	return ModelImpl::_dev;
}

int ModelImpl::getBrightness()
{
	return ModelImpl::_brightness;
}

ModelImpl::~ModelImpl()
{
	hid_close(_dev);
	if (hid_exit() == -1)
	{
		std::wcerr << "hid_exit() returned error: the static data associated with HIDAPI weren't freed" << '\n'; // todo: make error/log file
	}
}
Color ModelImpl::getRGB()
{
	return _rgb;
}
