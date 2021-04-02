#include <QCoreApplication>
#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QPluginLoader>

#include <scopy/core/plugin_manager.hpp>

// /plugins -> build-uit, windows

using namespace scopy::core;

PluginManager::PluginManager()
{
	// TODO: set a path from cmake that points to where the installed plugins will be placed
	QStringList nameFilter{"*.dylib", "*.so", "*.dll"}; // threads?
	QDir dir(QDir::current().path() + "/plugins");
	for (const QString& pg : dir.entryList(nameFilter)) {
		QPluginLoader loader(dir.path() + "/" + pg);
		qDebug() << loader.metaData();
		QObject* plugin = loader.instance(); // check nullptr
		m_plugins.push_back(qobject_cast<PluginInterface*>(plugin));
	}

//	qDebug() << "Libraries path: " << QLibraryInfo::location(QLibraryInfo::LibrariesPath);
//	qDebug() << "Dynamic loading from: " << QCoreApplication::libraryPaths();
}

PluginManager &PluginManager::getInstance()
{
	static PluginManager instance;

	return instance;
}

std::vector<PluginInterface*> PluginManager::getPlugins(/*example: iio_context*/) const
{
	// return all plugins, should we filter?
	return m_plugins;
}
