#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <string_view>

#include <hidapi.h>

#include "Color.h"

/*! \brief
 *
 * Interface for interacting with the chip that
 * allows to change the keyboard backlight settings
 */
class IController
{
public:
	virtual ~IController() = default;
	/*! \brief
	 * Sends data to chip to change _brightness of backlight
	 * \param lvl Level of _brightness [0; 100]
	 */
	virtual void setBrightness(uint16_t lvl) const = 0;
	//todo: add description
	virtual void setColor(Color Color) const = 0;

	using Ptr = std::shared_ptr<IController>;
};
