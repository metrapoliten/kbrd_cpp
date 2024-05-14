#pragma once

#include "interfaces/IController.h"
#include "interfaces/IModel.h"

class ControllerImpl : public IController
{
public:
	explicit ControllerImpl(IModel::Ptr model);

	void setBrightness(uint16_t lvl) const override;
	void setColor(Color Color) const override;

private:
	std::shared_ptr<IModel> _model;
};
