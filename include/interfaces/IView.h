#pragma once

/*! \brief
 *
 * Interface for interacting with user
 */
class IView
{
public:
	virtual ~IView() = default;

	/*! \brief
	 * Shows chip's settings (for now prints in console)
	 */
	virtual void showCurrentSettings() const = 0;

	/*! \brief
	 * Runs a user interface that helps the user interact with the backlight
	 */
	virtual void runMenu() const = 0;

//	/*! \brief
//	 * Changes level of _brightness
//	 * @param lvl Level of _brightness. Must be set in [0; 100]
//	 */
//	virtual void changeBrightness(int lvl) = 0;

	using Ptr = std::unique_ptr<IView>;
};
