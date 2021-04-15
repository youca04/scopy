#include "demo_plugin.hpp"

#include <scopy/gui/tool_menu.hpp>

DemoPlugin::DemoPlugin()
	: QObject()
{}

std::vector<ToolInterface*> DemoPlugin::getTools(iio_context *ctx, scopy::gui::ToolMenu *menu) { return {}; }

bool DemoPlugin::compatible(/*iio_context*/) const { return false; }

std::vector<QWidget*> DemoPlugin::getControls(/*iio_context*/) { return {}; }
