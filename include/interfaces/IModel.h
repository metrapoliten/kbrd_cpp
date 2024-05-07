#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string_view>
#include "Color.h"

extern "C"
{
#include <hidapi.h>
}
// todo: update descriptions of methods
/*! \brief
 *
 * Interface for initialising the work with chip
 * and getting settings form the chip
 */
class IModel
{
public:
	virtual ~IModel() = default;

	/*! \brief
	 * Gets the device handler required for setting the settings by the IController
	 * The device must be inited firstly
	 * @return
	 * Returns the device handler in case of success or error description otherwise
	 */
	virtual hid_device* getChipHandler() = 0;

	/*! \brief
	 * Gets the level of _brightness
	 * The device must be inited firstly
	 * @return
	 * Returns the level of _brightness in case of success or error description otherwise
	 */
	virtual uint8_t getBrightness() = 0;

	/*! \brief
	 * Gets RGB values from chip
	 * The device must be inited firstly
	 * @return
	 * Returns the unordered_map with RGB, char for color name ('R', 'G', 'B') and int for color value
	 */
	virtual Color getRGB() = 0;

	using Ptr = std::shared_ptr<IModel>;
};
