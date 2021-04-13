#include "m2k_tool_pack.hpp"

#include "src/logicanalyzer/logic_analyzer.h"

#include <scopy/core/tool_interface.hpp>
#include <scopy/core/hardware_resource.hpp>

#include "src/tool.hpp"

#include "libm2k/m2k.hpp"


using namespace scopy::core;

// Adapt to ToolInterface in order to use the plugin system
class ToolToToolInterfaceAdapter : public ToolInterface {

public:
	ToolToToolInterfaceAdapter(adiscope::Tool *tl)
		: m_tl(tl) {}

	// ToolInterface interface
public:
	QWidget *getWidget() override {
		return m_tl;
	}

	std::vector<QWidget *> getPreferences() override {
		return {};
	}

	HardwareResource getHardwareResource() const override {
		return {{""}, "", HardwareResource::ConnectionType::USB};
	}

private:
	adiscope::Tool *m_tl;
};

// Plugin implementation

M2kToolPack::M2kToolPack() : QObject()
{

}

std::vector<ToolInterface *> M2kToolPack::getTools(iio_context *ctx)
{

	adiscope::Tool *tl = new adiscope::logic::LogicAnalyzer(ctx,
								nullptr,
								nullptr,
								nullptr,
								nullptr);

	return {new ToolToToolInterfaceAdapter(tl)};
}


bool M2kToolPack::compatible() const
{
	return true;
}

std::vector<QWidget *> M2kToolPack::getControls()
{
	return {};
}
