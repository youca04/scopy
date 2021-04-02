#ifndef DATASINK_HPP
#define DATASINK_HPP

#include <vector>

/**
 * @brief Namespace scopy
 */
namespace scopy {

/**
 * @brief Namespace core
 */
namespace core {

class DataSource;

/**
 * @class DataSink
 * @brief Class that represent a sink. This class receives data from data sources
 */
class DataSink
{
public:
	/**
	 * @private
	 */
	virtual ~DataSink() = default;

	/**
	 * @brief sets the data source for this sink
	 *
	 * @param the data source
	 */
	void setDataSource(DataSource* dataSource); // sources

	/**
	 * @brief Returns the data source associated to this sink
	 *
	 * @return DataSource
	 */
	DataSource* getDataSource() const;

	/**
	 * @brief Returns this sinks priority
	 *
	 * @return QWidget *
	 *
	 * @note The priority is used in the data source to figure out in which
	 * order the registered sinks will receive the data
	 *
	 */
	virtual int priority() const;

public: // interface that needs to be implemented
	/**
	 * @brief Method that is called from a data surce when data is received
	 *
	 * @param vector of pointers to the actual data
	 * @param vector of buffer sizes for each pointer to data
	 * @param vector of itemsizes for each pointer to data
	 *
	 * @return true if another sink can further receive this data, or false otherwise
	 *
	 */
	virtual bool receive(const std::vector<void*>& data, const std::vector<int> &bufferSize, const std::vector<int> &itemSize) = 0;

private:
	DataSource* m_dataSource{nullptr}; // std::vector
};
} // namespace core
} // namespace scopy

#endif // DATASINK_HPP
