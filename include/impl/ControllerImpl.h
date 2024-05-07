#pragma once

#include "interfaces/IController.h"
#include "interfaces/IModel.h"

class ControllerImpl : public IController
{
public:
	explicit ControllerImpl(std::shared_ptr<IModel>);

	void setBrightness(std::uint16_t lvl) override;
private:
	std::shared_ptr<IModel> _model;
};
