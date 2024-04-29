// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "Controller.h"
#include "Model.h"
#include "View.h"

#include <iostream>
#include <stdexcept>

int main()
{
	try
	{
		auto model = createModel();
		auto controller = createController(model);
		auto view = createView(controller, model);
		view->showCurrentSettings();
		view->runMenu();
	}
	catch (std::runtime_error& e)
	{
		std::cout << e.what();
	}
	return 0;
}
