#pragma once

#include "interfaces/IModel.h"

class ModelImpl : public IModel
{
public:
	/*! \brief
	 * throws std::runtime_error()
	 */
	ModelImpl();
	~ModelImpl() override;

	ModelImpl(const ModelImpl&) = delete;
	ModelImpl& operator=(const ModelImpl&) = delete;

	hid_device* getChipHandler() override;
	int getBrightness() override;
	Color getRGB() override;

private:
	hid_device* _dev; // todo: change to ref?
	int _brightness;
	Color _rgb;
};