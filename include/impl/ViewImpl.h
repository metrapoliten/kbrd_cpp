#pragma once

#include <memory>

#include "interfaces/IView.h"
#include "interfaces/IController.h"
#include "interfaces/IModel.h"

class ViewImpl : public IView
{
public:
	ViewImpl(IController::Ptr controller, IModel::Ptr model);

	void showCurrentSettings() const override;
	void runMenu() const override;

private:
	IController::Ptr _controller;
	IModel::Ptr _model;
};
