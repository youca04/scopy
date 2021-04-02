
#include <QDebug>
#include <QString>

#include <algorithm>
#include <scopy/core/data_sink.hpp>
#include <scopy/core/data_source.hpp>

using namespace scopy::core;

DataSource::DataSource(DataSink* dataSink) { registerDataSink(dataSink); }

// TODO: unregister data sink on destructor ?

void DataSource::registerDataSink(DataSink* dataSink)
{
	// TODO: we can have data sinks stored in an ordered container
	m_sinks.push_back(dataSink);
	//	qDebug() << "Added sink for source" << this << " with priority: " << dataSink->priority();

	std::sort(m_sinks.begin(), m_sinks.end(),
		  [](DataSink* dsl, DataSink* dsr) { return dsl->priority() > dsr->priority(); });

	//	qDebug() << m_sinks[0]->priority() << " is the first sink";
}

void DataSource::unregisterDataSink(DataSink* dataSink)
{
	m_sinks.erase(std::remove(m_sinks.begin(), m_sinks.end(), dataSink), m_sinks.end());
}

void DataSource::notify(const std::vector<void*> &data, const std::vector<int> &bufferSize, const std::vector<int> &itemSize)
{
	for (DataSink* dataSink : m_sinks) {
		if (!dataSink->receive(data, bufferSize, itemSize)) {
			break;
		}
	}
}

void DataSource::addExternalSource(DataSource* dataSource) { m_externalSources.push_back(dataSource); }

void DataSource::removeExternalSource(DataSource* dataSource)
{
	m_externalSources.erase(std::remove(m_externalSources.begin(), m_externalSources.end(), dataSource),
				m_externalSources.end());
}

bool DataSource::acquireOwnership(DataSource::AcquirePolicy policy)
{
	for (DataSource* dataSource : m_externalSources) {
		const bool tryStop = dataSource->isRunning() && dataSource->hardwareResource() == hardwareResource();
		if (tryStop) {
			if (policy == AcquirePolicy::Steal) {
				dataSource->stop();
			} else {
				return false;
			}
		}
	}

	return true;
}
