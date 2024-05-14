// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <iostream>

#include "Controller.h"
#include "ChipException.h"
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
		std::cerr << "Error" << e.what();
	}

	return 0;
}
