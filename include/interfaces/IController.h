#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <string_view>

#include <hidapi.h>

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
	virtual void setBrightness(std::uint16_t lvl) = 0;

	using Ptr = std::shared_ptr<IController>;
};
