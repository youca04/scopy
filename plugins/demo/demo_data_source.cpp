#include "demo_data_source.hpp"

DemoDataSource::DemoDataSource(DataSink *dataSink)
	: DataSource(dataSink)
{

}

void DemoDataSource::start()
{
	m_running = true;

	int *a = new int[3];
	a[0] = 3;
	a[1] = 7;
	a[2] = 8;

	notify({a}, {3}, {sizeof(int)});
}

void DemoDataSource::stop()
{
	m_running = false;
	Q_EMIT stopped();
}

bool DemoDataSource::isRunning() const
{
	return m_running;
}

HardwareResource DemoDataSource::hardwareResource() const
{
	return {{"adc"}, "serial_number", HardwareResource::ConnectionType::USB};
}
