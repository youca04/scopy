#include <scopy/core/data_sink.hpp>
#include <scopy/core/data_source.hpp>

using namespace scopy::core;

void DataSink::setDataSource(DataSource* dataSource) { m_dataSource = dataSource; }

DataSource* DataSink::getDataSource() const { return m_dataSource; }

int DataSink::priority() const { return -1; }
