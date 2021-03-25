#ifndef HARDWARERESOURCE_HPP
#define HARDWARERESOURCE_HPP

#include <string>
#include <vector>

/**
 * @brief Namespace scopy
 */
namespace scopy {

/**
 * @brief Namespace core
 */
namespace core {

/**
 * @struct HardwareResource
 * @brief Describes the hardware resources used by a tool / data source
 */
struct HardwareResource
{
	/**
	 * @enum ConnectionType
	 * @brief Types of connection for a board
	 */
	enum class ConnectionType
	{
		USB,
		NETWORK
	};

	/**
	 * @brief List of iio_devices used.
	 */
	std::vector<std::string> m_devices;

	/**
	 * @brief The serial number of the board
	 */
	std::string m_serialNumber;

	/**
	 * @brief The connection type;
	 */
	ConnectionType m_connectionType;
};
} // namespace core
} // namespace scopy

#endif // HARDWARERESOURCE_HPP