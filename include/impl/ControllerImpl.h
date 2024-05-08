#pragma once

#include "interfaces/IController.h"
#include "interfaces/IModel.h"

class ControllerImpl : public IController
{
public:
	explicit ControllerImpl(IModel::Ptr model);

	void setBrightness(std::uint16_t lvl) override;
	void setColor(Color Color) override;
private:
	std::shared_ptr<IModel> _model;
};
