// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include "Controller.h"
#include "Model.h"
#include "View.h"

#include <stdexcept>

int main()
{
	auto model = createModel();
	auto controller = createController(model);
	auto view = createView(controller, model);
	view->showCurrentSettings();
	view->runMenu();

	return 0;
}
