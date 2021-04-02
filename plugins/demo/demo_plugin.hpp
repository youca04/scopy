#ifndef DEMOPLUGIN_HPP
#define DEMOPLUGIN_HPP

#include <QObject>
#include <QWidget>

#include <scopy/core/plugin_interface.hpp>

using namespace scopy::core;

class DemoPlugin : public QObject, public scopy::core::PluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Scopy.Core.PluginInterface" FILE "demo_plugin.json")
	Q_INTERFACES(scopy::core::PluginInterface)
public:
	DemoPlugin();

public: // PluginInterface
	std::vector<ToolInterface*> getTools(/*iio_context*/) override;
	bool compatible(/*iio_context*/) const override;
	std::vector<QWidget*> getControls(/*iio_context*/) override;
	int priority() const override { return 0; }
};

#endif // DEMOPLUGIN_HPP
