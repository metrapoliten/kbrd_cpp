#pragma once

#include <memory>
#include <string_view>

extern "C"
{
#include <hidapi.h>
}

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
	virtual int getBrightness() = 0;

	using Ptr = std::shared_ptr<IModel>;
};
