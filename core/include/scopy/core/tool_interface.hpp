#ifndef TOOLINTERFACE_HPP
#define TOOLINTERFACE_HPP

#include "hardware_resource.hpp"

#include <vector>

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
 * @class ToolInterface
 * @brief Interface for tools to implement
 */
class ToolInterface // RunableToolInterface -> start / stop/ isRunning/
{
public:
	/**
	 * @private
	 */
	virtual ~ToolInterface() = default;

public:
	/**
	 * @brief Returns the tool as a QWidget
	 *
	 * @return QWidget *
	 */
	virtual QWidget* getWidget() = 0;

	// start / stop tool interface forwards data soure start stop (debugger manual calibration)

	// ToolDetails -> {name, icon, } getToolDetails static

	// std::vector<sDataSource> getDataSources() { return {nullptr}; }

//	virtual std::string name() const = 0;

//	static virtual QWidget * getToolMenuButtonBeforeConnect() { return nullptr; }

	/**
	 * @brief Returns the list of preferences that are supported by this tool
	 *
	 * @return std::vector<QWidget *>
	 */
	virtual std::vector<QWidget*> getPreferences() = 0;

	/**
	 * @brief Returns the hardware resource that is used by this tool
	 *
	 * @return HardwareResource
	 */
	virtual HardwareResource getHardwareResource() const = 0;

};
} // namespace core
} // namespace scopy
#endif // TOOLINTERFACE_HPP
