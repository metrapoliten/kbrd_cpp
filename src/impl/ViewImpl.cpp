// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <cassert>
#include <charconv>
#include <cstring>
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
constexpr char CHANGE_BRIGHTNESS = 'b';
constexpr char CHANGE_MONOCOLOR = 'm';
constexpr char SHOW_CURRENT_SETTINGS = 'c';
constexpr char QUIT = 'q';

void printOptions()
{
	std::cout << "OPTIONS\n"
				 "> b     change brightness\n"
				 "> m     change monocolor\n"
				 "> c     show current settings\n"
				 "> q     quit\n";
}

void changeBrightness(const IController::Ptr& c_ptr, std::uint16_t lvl) //todo: should i use reference?
{
	assert(0 <= lvl and lvl <= 100);
	try
	{
		c_ptr->setBrightness(lvl);
	}
	catch (std::runtime_error& e) //todo: delete try-catch block
	{
		std::cout << e.what();
	}
}

std::variant<std::array<std::string, 3>, int> splitColor(const std::string& s)
{
	std::stringstream ss(s);
	std::string token;
	std::array<std::string, 3> colors;
	std::uint32_t tokensCount = 0;
	while(std::getline(ss, token, '.'))
	{
		if (tokensCount == 3)
		{
			return 1;
		}
		colors[tokensCount++] = token;
	}
	if (tokensCount != 3)
	{
		return 1;
	}
	return colors;
}

constexpr int COLOR_INVALID_ARGUMENT = 1;
constexpr int COLOR_OUT_OF_RANGE = 2;
std::variant<Color, int> colorsStringArrToStruct(std::array<std::string, 3> tokens)
{
	std::uint8_t value;
	std::array<std::uint8_t, 3> colors{};
	uint8_t colorCount = 0;
	for (auto &i : tokens)
	{
		auto [ptr, ec] = std::from_chars(i.data(), i.data() + i.size(), value);
		if (ec == std::errc())
		{
			colors[colorCount++] = value;
		}
		else if (ec == std::errc::invalid_argument)
		{
			return COLOR_INVALID_ARGUMENT;
		}
		else if (ec == std::errc::result_out_of_range)
		{
			return COLOR_OUT_OF_RANGE;
		}
	}
	Color Color{
		colors[0],
		colors[1],
		colors[2]
	};
	return Color;
}

void changeColor(const IController::Ptr& c_ptr) //todo: should i use reference?
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
		else if(std::get<int>(res2) == COLOR_INVALID_ARGUMENT)
		{
			std::cout << "INVALID ARGUMENT\n";
		}
		else if(std::get<int>(res2) == COLOR_OUT_OF_RANGE)
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

void ViewImpl::showCurrentSettings()
{
	auto lvl = _model->getBrightness();
	Color RGB = _model->getRGB();
	std::cout << "CURRENT SETTINGS" << '\n'
			  << "> Brightness: " << +lvl << '%' << '\n'
			  << "> RGB: " << +RGB.R << ' ' << +RGB.B << ' ' << +RGB.G << '\n';
}

void ViewImpl::runMenu()
{
	printOptions();
	char optionBuf[4] = { 0 };
	while (optionBuf[0] != QUIT)
	{
		std::cout << "Enter command: ";
		std::cin >> optionBuf;
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		switch (optionBuf[0])
		{
		case CHANGE_BRIGHTNESS:
			std::uint16_t lvl; // lvl is not uint8_t, because it reads as character, not number
			std::cout << "value [0, 100]: ";
			std::cin >> lvl; //todo: change to getline?
			if (lvl <= 100)
			{
				changeBrightness(_controller, lvl);
				break;
			}
			std::cout << "LEVEL OF BRIGHTNESS MUST BE FROM 0 TO 100 INCLUSIVELY\n";
			break;
		case CHANGE_MONOCOLOR:
			changeColor(_controller);
			break;
		case SHOW_CURRENT_SETTINGS:
			showCurrentSettings();
			break;
		case QUIT:
			break;
		default:
			std::cout << "WRONG OPTION\n";
			memset(&optionBuf, 0, sizeof(optionBuf));
			break;
		}
	}
}