#include <algorithm>
#include <scopy/core/hardware_resource.hpp>

namespace scopy {
namespace core {

bool operator==(const HardwareResource& lhs, const HardwareResource& rhs)
{
	// if the serial number is the same
	if (lhs.m_serialNumber == rhs.m_serialNumber) {
		// check if they use the same iio_device
		// TODO: consider using std::set to store iio_devices
		for (auto it = lhs.m_devices.begin(); it != lhs.m_devices.end(); ++it) {
			if (std::find(rhs.m_devices.begin(), rhs.m_devices.end(), *it) != rhs.m_devices.end()) {
				return true;
			}
		}
	}

	// If the serial number is different, there is no way both hardware resource
	// share the same iio_device
	return false;
}

bool operator!=(const HardwareResource& lhs, const HardwareResource& rhs) { return !(lhs == rhs); }

} // namespace core
} // namespace scopy
