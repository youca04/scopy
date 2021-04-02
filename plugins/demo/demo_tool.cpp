#include "demo_tool.hpp"

#include "demo_data_source.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>


DemoTool::DemoTool()
{
	// setup data source for "this" sink
	auto dataSource = new DemoDataSource(this);
	setDataSource(dataSource);

//	dataSource->registerDataSink(m_plot);
//	dataSource->registerDataSink(file);

	m_toolView = new QWidget();
	auto layout = new QHBoxLayout(m_toolView);
	m_toolView->setLayout(layout);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(10);

	layout->addWidget(new QLabel("Siggen: "));

	auto run_button = new QPushButton("run");
	layout->addWidget(run_button);

	m_statusLabel = new QLabel();
	layout->addWidget(m_statusLabel);
	m_statusLabel->setText("Waiting to be run");

	layout->insertSpacerItem(-1, new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

	connect(run_button, &QPushButton::clicked, [=](){
		if (getDataSource()->acquireOwnership(DataSource::AcquirePolicy::TrySteal)) {
			// We own the hardware resource here
			// 1. do hardware settings
			// ...
			// 2. start the data source
			getDataSource()->start();
		} else {
			m_statusLabel->setText("Failed to start!");
		}
	});

	connect(dataSource, &DemoDataSource::stopped, [=](){
		m_statusLabel->setText("Stopped by data source!");
	});

	m_toolView->setStyleSheet("background-color: #e27d60");
	m_toolView->setMaximumHeight(60);
	m_toolView->setMinimumHeight(60);
}

bool DemoTool::receive(const std::vector<void *> &data, const std::vector<int> &bufferSize, const std::vector<int> &itemSize)
{
	Q_UNUSED(bufferSize);

	int *array = static_cast<int *>(data[0]);
	QString labelText = "";
	for (int i = 0; i < 3; ++i) {
		labelText += " " + QString::number(array[i]);
	}
	m_statusLabel->setText(labelText);

	return true;
}

QWidget *DemoTool::getWidget()
{
	return m_toolView;
}

std::vector<QWidget *> DemoTool::getPreferences()
{
	return {};
}

HardwareResource DemoTool::getHardwareResource() const
{
	// This tool has a data source, the data source in this
	// case is "talking" to the hardware so we return its
	// hardware resource
	return getDataSource()->hardwareResource();
}
