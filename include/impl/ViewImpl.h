#pragma once

#include <memory>

#include "interfaces/IView.h"

class ViewImpl : public IView
{
public:
	ViewImpl(std::shared_ptr<IController> controller, std::shared_ptr<IModel>);

	void showCurrentSettings() override;
	void runMenu() override;
//	void changeBrightness(int lvl) override;

private:
	std::shared_ptr<IController> _controller;
	std::shared_ptr<IModel> _model;
};
