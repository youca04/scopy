#include "m2k_tool_pack.hpp"

#include "src/oscilloscope.hpp"

#include <scopy/core/tool_interface.hpp>
#include <scopy/core/hardware_resource.hpp>

#include "src/tool.hpp"

using namespace scopy::core;

// Adapt to ToolInterface in order to use the plugin system
class ToolToToolInterfaceAdapter : public ToolInterface {

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

std::vector<ToolInterface *> M2kToolPack::getTools()
{

}

bool M2kToolPack::compatible() const
{
        return true;
}

std::vector<QWidget *> M2kToolPack::getControls()
{
        return {};
}
