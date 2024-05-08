// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "impl/ModelImpl.h"

namespace ErrorMsg //todo: move to one file
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
	try
	{
		if (res == -1)
		{
			logAndThrowRuntimeErr(dev);
		}
	}
	catch (std::runtime_error &e)
	{
		std::cout << e.what();
	}
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

std::uint8_t collectBrightness(hid_device* dev)
{
	assert(dev);

	unsigned char buf[REPORT_LENGTH];
	getReport(dev, GET_EFFECT_BYTE, buf);
	return buf[5];
}

Color collectRGB(hid_device* dev)
{
	assert(dev);

	unsigned char hidBuf[REPORT_LENGTH];
	getReport(dev, GET_MONOCOLOR_BYTE, hidBuf);

	Color Color {
		hidBuf[4], // .R
		hidBuf[5], // .G
		hidBuf[6] // .B
	};
	return Color;
}

hid_device* openDevice()
{
	const int res = hid_init();
	checkReportError(res, nullptr);

	auto dev = hid_open(VENDOR_ID, PRODUCT_ID, nullptr);
	if (!dev)
	{
		try
		{
			logAndThrowRuntimeErr(nullptr);
		}
		catch (std::runtime_error &e)
		{
			std::cout << e.what();
		}
	}
	return dev;
}

}

ModelImpl::ModelImpl()
	: _dev{ openDevice() }
	, _brightness{ collectBrightness(_dev) }
	, _rgb{ collectRGB(_dev) }
{
}

hid_device* ModelImpl::getChipHandler()
{
	return ModelImpl::_dev;
}

unsigned char ModelImpl::getBrightness() //todo: make new try-catch system
{
	ModelImpl::_brightness = collectBrightness(_dev);
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
Color ModelImpl::getRGB() //todo: make new try-catch system
{
	ModelImpl::_rgb = collectRGB(_dev);
	return _rgb;
}
