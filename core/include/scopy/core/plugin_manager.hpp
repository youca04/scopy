#ifndef PLUGINMANAGER_HPP
#define PLUGINMANAGER_HPP

#include <scopy/core/plugin_interface.hpp>
#include <vector>

namespace scopy {
namespace core {
class PluginManager
{
public:
	static PluginManager &getInstance();

	PluginManager(const PluginManager&) = delete;
	PluginManager& operator=(const PluginManager&) = delete;

	std::vector<PluginInterface*> getPlugins() const;

private:
	PluginManager();

private:
	std::vector<PluginInterface*> m_plugins;
};
} // namespace core
} // namespace scopy
#endif // PLUGINMANAGER_HPP
