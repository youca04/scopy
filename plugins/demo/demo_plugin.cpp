#include "demo_plugin.hpp"

DemoPlugin::DemoPlugin()
	: QObject()
{}

std::vector<ToolInterface*> DemoPlugin::getTools(/*iio_context*/) { return {}; }

bool DemoPlugin::compatible(/*iio_context*/) const { return true; }

std::vector<QWidget*> DemoPlugin::getControls(/*iio_context*/) { return {}; }
