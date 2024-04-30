// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>

#include "Color.h"
#include "impl/ViewImpl.h"

namespace
{
constexpr char CHANGE_BRIGHTNESS = 'b';
constexpr char QUIT = 'q';
}

ViewImpl::ViewImpl(std::shared_ptr<IController> controller, std::shared_ptr<IModel> model)
	: _controller(std::move(controller))
	, _model(std::move(model))
{
}

void ViewImpl::showCurrentSettings()
{
	int lvl = _model->getBrightness();
	Color RGB = _model->getRGB();
	std::cout << "CURRENT SETTINGS" << '\n'
			  << "> Brightness: " << lvl << '%' << '\n'
			  << "> RGB: " << RGB.R << ' ' << RGB.B << ' ' << RGB.G << '\n';
}

static inline void printOptions()
{
	std::cout << "OPTIONS\n"
				 "> b     change brightness\n";
}

static inline void changeBrightness(const std::shared_ptr<IController>& c_ptr, int lvl)
{
	assert(0 <= lvl and lvl <= 100);
	try
	{
		c_ptr->setBrightness(lvl);
	}
	catch (std::runtime_error& e)
	{
		std::cout << e.what();
	}
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
			int lvl;
			std::cout << "value [0, 100]: ";
			std::cin >> lvl;
			//			memset(&optionBuf, 0, sizeof(optionBuf));
			if (0 <= lvl and lvl <= 100)
			{
				changeBrightness(_controller, lvl);
				break;
			}
			std::cout << "LEVEL OF BRIGHTNESS MUST BE FROM 0 TO 100 INCLUSIVELY\n";
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