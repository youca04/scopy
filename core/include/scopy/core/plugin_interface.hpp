#ifndef PLUGININTERFACE_HPP
#define PLUGININTERFACE_HPP

#include "tool_interface.hpp"

#include <QtPlugin>

#include <vector>
#include <iio.h>

namespace scopy {
namespace gui {
class ToolMenu;
}
}

class QWidget;

/**
 * @brief Namespace scopy
 */
namespace scopy {

/**
 * @brief Namespace core
 */
namespace core {

/**
 * @class PluginInterface
 * @brief Interface for plugins to implement
 */
class PluginInterface
{
public:
	/**
	 * @private
	 */
	virtual ~PluginInterface() = default;

public:
	/**
	 * @brief Returns the list of tools for the given iio_context
	 *
	 * @param The iio_context ?
	 * @return std::vector<ToolInterface *>
	 *
	 * @note: the tool menu is used to get the corresponding tool menu item
	 * @note Will return an empty list in case the iio_context is not compatible with the plugin
	 */
	virtual std::vector<ToolInterface*> getTools(struct iio_context *ctx/*, scopy::gui::ToolMenu *menu*/) = 0;

	/**
	 * @brief Returns if the iio_context is compatible with the plugin
	 *
	 * @param The iio_context ?
	 * @return true if compatible, false otherwise
	 */
	virtual bool compatible(/*iio_context*/) const = 0;

	/**
	 * @brief Returns the list of extra controls for the given iio_context that this plugin implements
	 *
	 * @param The iio_context ?
	 * @return std::vector<QWidget *>
	 *
	 * @note An example of controls is: the "calibrate" button and the "identify" for the adalm2000
	 */
	virtual std::vector<QWidget*> getControls(/*iio_context*/) = 0;

	/**
	 * @brief Returns the priority of this plugin against other plugins
	 *
	 * @return int value
	 */
	virtual int priority() const { return 0; }

public:
};

} // namespace core
} // namespace scopy

#define PluginInterface_iid "org.qt-project.Scopy.Core.PluginInterface/1.0"
Q_DECLARE_INTERFACE(scopy::core::PluginInterface, PluginInterface_iid)

#endif // PLUGININTERFACE_HPP
