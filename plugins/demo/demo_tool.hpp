#ifndef DEMO_TOOL_HPP
#define DEMO_TOOL_HPP

#include <QObject>
#include <QWidget>
#include <QLabel>

#include <scopy/core/data_sink.hpp>
#include <scopy/core/tool_interface.hpp>

using namespace scopy::core;

class DemoTool : public QObject, public ToolInterface, public DataSink
{
	Q_OBJECT
public:
	DemoTool();

	// DataSink interface
public:
	bool receive(const std::vector<void*>& data, const std::vector<int> &bufferSize, const std::vector<int> &itemSize) override;

	// ToolInterface interface
public:
	QWidget *getWidget() override;
	std::vector<QWidget *> getPreferences() override;
	HardwareResource getHardwareResource() const override;

private:
	QWidget *m_toolView{nullptr};
	QLabel *m_statusLabel{nullptr};
};

#endif // DEMO_TOOL_HPP
