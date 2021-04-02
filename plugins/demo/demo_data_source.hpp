#ifndef DEMO_DATA_SOURCE_HPP
#define DEMO_DATA_SOURCE_HPP

#include <QObject>

#include <scopy/core/data_source.hpp>

using namespace scopy::core;

class DemoDataSource : public QObject, public DataSource
{
	Q_OBJECT
public:
	DemoDataSource(DataSink* dataSink);

Q_SIGNALS:
	void stopped();

public:
	void setSingle(bool);

	// DataSource interface
public:
	void start() override;
	void stop() override;
	bool isRunning() const override;
	HardwareResource hardwareResource() const override;

private:
	bool m_running{false};
};

#endif // DEMO_DATA_SOURCE_HPP
