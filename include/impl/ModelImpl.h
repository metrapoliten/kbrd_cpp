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

	[[nodiscard]] hid_device* getChipHandler() const noexcept override;
	unsigned char getBrightness() override;
	Color getRGB() override;

private:
	hid_device* _dev; // todo: change to ref?
	uint8_t _brightness;
	Color _rgb;
};