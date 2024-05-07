#pragma once

#include <memory>

#include "interfaces/IView.h"
#include "interfaces/IController.h"
#include "interfaces/IModel.h"

class ViewImpl : public IView
{
public:
	ViewImpl(IController::Ptr controller, IModel::Ptr model);

	void showCurrentSettings() override;
	void runMenu() override;
//	void changeBrightness(int lvl) override;

private:
	IController::Ptr _controller;
	IModel::Ptr _model;
};
