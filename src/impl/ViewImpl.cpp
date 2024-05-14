// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <charconv>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <variant>

#include "Color.h"
#include "impl/ViewImpl.h"

namespace
{
constexpr char kChangeBrightness = 'b';
constexpr char kChangeMonocolor = 'm';
constexpr char kShowCurrentSettings = 'c';
constexpr char kQuit = 'q';

void printOptions()
{
	std::cout << "OPTIONS\n"
				 "> b     change brightness\n"
				 "> m     change monocolor\n"
				 "> c     show current settings\n"
				 "> q     quit\n";
}

void changeBrightness(const IController::Ptr& _controller)
{
	uint16_t lvl; // lvl is not uint8_t, because it reads as character, not number
	std::cout << "value [0, 100]: ";
	std::cin >> lvl;
	if (!std::cin)
	{
		if (std::cin.eof())
		{
			std::terminate();
		}
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout << "INVALID ARGUMENT\n";
		return;
	}
	if (lvl <= 100)
	{
		_controller->setBrightness(lvl);
		return;
	}
	std::cout << "LEVEL OF BRIGHTNESS MUST BE FROM 0 TO 100 INCLUSIVELY\n";
}

enum class ColorErrorCode
{
	InvalidArgument,
	OutOfRange,
	NotThreeTokens
};

std::variant<std::array<std::string, 3>, ColorErrorCode> splitColor(std::string const& s)
{
	std::stringstream ss(s);
	std::string token;
	std::array<std::string, 3> colors;
	uint32_t tokensCount = 0;
	while(std::getline(ss, token, '.'))
	{
		if (tokensCount == 3)
		{
			return ColorErrorCode::NotThreeTokens;
		}
		colors[tokensCount++] = token;
	}
	if (tokensCount != 3)
	{
		return ColorErrorCode::NotThreeTokens;
	}
	return colors;
}

std::variant<Color, ColorErrorCode> colorsStringArrToStruct(std::array<std::string, 3> const& tokens)
{
	uint8_t value;
	std::array<uint8_t, 3> colors{};
	uint8_t colorCount = 0;
	for (auto const &i : tokens)
	{
		auto [ptr, ec] = std::from_chars(i.data(), i.data() + i.size(), value);
		if (ec == std::errc())
		{
			colors[colorCount++] = value;
		}
		else if (ec == std::errc::invalid_argument)
		{
			return ColorErrorCode::InvalidArgument;
		}
		else if (ec == std::errc::result_out_of_range)
		{
			return ColorErrorCode::OutOfRange;
		}
	}
	Color Color{
		colors[0],
		colors[1],
		colors[2]
	};
	return Color;
}

void changeColor(const IController::Ptr& c_ptr)
{
	std::string input;
	std::cout << "[R.G.B]: ";
	std::getline(std::cin, input);

	auto const res = splitColor(input);
	if (std::holds_alternative<std::array<std::string, 3>>(res))
	{
		auto const res2 = colorsStringArrToStruct(std::get<std::array<std::string, 3>>(res));
		if (std::holds_alternative<Color>(res2))
		{
			c_ptr->setColor(std::get<Color>(res2));
		}
		else if(std::get<ColorErrorCode>(res2) == ColorErrorCode::InvalidArgument)
		{
			std::cout << "INVALID ARGUMENT\n";
		}
		else if(std::get<ColorErrorCode>(res2) == ColorErrorCode::OutOfRange)
		{
			std::cout << "PRIMARY COLOR VALUE MUST BE FROM 0 TO 255 INCLUSIVELY\n";
		}
	}
	else
	{
		std::cout << "UNABLE TO GET THREE COLORS: EXPECTED THREE TOKENS SEPARATED BY DOT\n";
	}
}

}

ViewImpl::ViewImpl(IController::Ptr controller, IModel::Ptr model)
	: _controller(std::move(controller))
	, _model(std::move(model))
{
}

void ViewImpl::showCurrentSettings() const
{
	auto const lvl = _model->getBrightness();
	Color const RGB = _model->getRGB();
	std::cout << "CURRENT SETTINGS" << '\n'
			  << "> Brightness: " << +lvl << '%' << '\n'
			  << "> RGB: " << +RGB.R << ' ' << +RGB.B << ' ' << +RGB.G << '\n';
}

void ViewImpl::runMenu() const
{
	printOptions();
	char optionBuf[4] = { 0 }; //todo: write why size = 4
	while (optionBuf[0] != kQuit)
	{
		std::cout << "Enter command: ";
		std::cin >> optionBuf;
		if (!std::cin)
		{
			if (std::cin.eof())
			{
				std::terminate();
			}
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		switch (optionBuf[0])
		{
		case kChangeBrightness:
			changeBrightness(_controller);
			break;
		case kChangeMonocolor:
			changeColor(_controller);
			break;
		case kShowCurrentSettings:
			showCurrentSettings();
			break;
		case kQuit:
			break;
		default:
			std::cout << "WRONG OPTION\n";
			break;
		}
	}
}