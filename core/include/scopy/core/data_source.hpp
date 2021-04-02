#ifndef DATASOURCE_HPP
#define DATASOURCE_HPP

#include <algorithm>
#include <scopy/core/hardware_resource.hpp>
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

class DataSink;

/**
 * @class DataSource
 * @brief Class that represent a data source. This class is used to stream data from different sources
 */
class DataSource
{
public:
	/**
	 * @brief Constructor
	 */
	DataSource(DataSink* dataSink);

	/**
	 * @private
	 */
	virtual ~DataSource() = default;

	/**
	 * @brief register a new sink that will receive data from this data source
	 *
	 * @param the data sink
	 */
	void registerDataSink(DataSink* dataSink);

	/**
	 * @brief unregister a sink to stop receiving data from this data source
	 *
	 * @param the data sink
	 */
	void unregisterDataSink(DataSink* dataSink);

	/**
	 * @brief Method that is called to notify all the data sinks of existing new data
	 *
	 * @param vector of pointers to the actual data
	 * @param vector of buffer sizes for each pointer to data
	 * @param vector of itemsizes for each pointer to data
	 */
	virtual void notify(const std::vector<void*>& data, const std::vector<int>& bufferSize,
		    const std::vector<int>& itemSize);

	void addExternalSource(DataSource* dataSource);
	void removeExternalSource(DataSource* dataSource);

	enum class AcquirePolicy
	{
		TrySteal, // This data source will not be able to start because
			  // other data sources using the hardware resource are running
		Steal	  // This data source will force stop other data sources that use
			  // the same hardware resource
	};

	// acquire the ownership for using the hardware resource to capture data
	bool acquireOwnership(AcquirePolicy policy = AcquirePolicy::Steal);

public: // interface that needs to be implemented
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual bool isRunning() const = 0;
	virtual HardwareResource hardwareResource() const = 0; // iio_device libm2k::context -> 2 pluginuri
							       // read - write _> osc-ul read + write
private:
	std::vector<DataSink*> m_sinks;

	// data sources that are not part of the implementers plugin
	std::vector<DataSource*> m_externalSources;
};
} // namespace core
} // namespace scopy
#endif // DATASOURCE_HPP
