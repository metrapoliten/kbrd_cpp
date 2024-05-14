#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string_view>

#include <hidapi.h>

#include "Color.h"

/*! \brief
 * Interface for initialising the work with chip
 * and getting settings from the chip
 */
class IModel
{
public:
	virtual ~IModel() = default;

	/*! \brief
	 * Gets the chip handler from HIDAPI required to change the settings by the Controller
	 * @return
	 * Returns the device handler in case, can't be nullptr
	 */
	[[nodiscard]] virtual hid_device* getChipHandler() const = 0;

	/*! \brief
	 * Requests from chip the level of brightness
	 * @return
	 * Returns the level of brightness [0; 100]
	 */
	virtual uint8_t getBrightness() = 0;

	/*! \brief
	 * Requests RGB values from the chip
	 * @return
	 * Returns the struct with .R, .G, .B fields
	 */
	virtual Color getRGB() = 0;

	using Ptr = std::shared_ptr<IModel>;
};
