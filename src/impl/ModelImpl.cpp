// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <cassert>
#include <cstring>
#include <iostream>

#include "ChipException.h"
#include "impl/ModelImpl.h"
#include "operator_uc.h"

namespace
{
constexpr unsigned short kVID = 0x048d;
constexpr unsigned short kPID = 0x6006;

constexpr unsigned char kGetEffectByte = 0x88_uc;
constexpr unsigned char kGetMonocolorByte = 0x94_uc;

constexpr unsigned kReportLength = 9;

void logAndThrowRuntimeErr(hid_device* dev)
{
	std::wcerr << hid_error(dev) << '\n'; // todo: make error/log file
	throw ChipException("Can't handle the chip (check log)");
}

void checkReportError(int res, hid_device* dev)
{
	if (res == -1)
	{
		logAndThrowRuntimeErr(dev);
	}
}

void getReport(hid_device* dev, unsigned char controlByte, unsigned char* buf)
{
	buf[0] = controlByte;
	int res = hid_send_feature_report(dev, buf, kReportLength);
	checkReportError(res, dev);
	std::memset(buf, 0, kReportLength);
	buf[0] = 0;
	res = hid_get_feature_report(dev, buf, kReportLength);
	checkReportError(res, dev);
}

uint8_t collectBrightness(hid_device* dev)
{
	assert(dev);

	unsigned char buf[kReportLength];
	getReport(dev, kGetEffectByte, buf);
	return buf[5];
}

Color collectRGB(hid_device* dev)
{
	assert(dev);

	unsigned char hidBuf[kReportLength];
	getReport(dev, kGetMonocolorByte, hidBuf);

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

	auto dev = hid_open(kVID, kPID, nullptr);
	if (!dev)
	{
		logAndThrowRuntimeErr(nullptr);
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
