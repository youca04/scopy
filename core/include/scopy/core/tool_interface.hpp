#ifndef TOOLINTERFACE_HPP
#define TOOLINTERFACE_HPP

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
class ToolInterface
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

	/**
	 * @brief Returns the list of preferences that are supported by this tool
	 *
	 * @return std::vector<QWidget *>
	 */
	virtual std::vector<QWidget*> getPreferences() = 0;
	// start stop
};
} // namespace core
} // namespace scopy
#endif // TOOLINTERFACE_HPP
