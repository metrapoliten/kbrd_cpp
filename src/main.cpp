// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <iostream>

#include "ChipException.h"
#include "Controller.h"
#include "Model.h"
#include "View.h"

int main()
{
	try
	{
		auto const model = createModel();
		auto const controller = createController(model);
		auto const view = createView(controller, model);
		view->showCurrentSettings();
		view->runMenu();
	}
	catch (ChipException const& e)
	{
		std::cerr << "Chip Error: " << e.what() << '\n';
	}
	catch (std::runtime_error const& e)
	{
		std::cerr << "IO Error: " << e.what() << '\n';
	}

	return 0;
}
