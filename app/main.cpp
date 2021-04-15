#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QDir>

#include <scopy/core/plugin_manager.hpp>
#include <scopy/gui/theme_manager.hpp>
#include <scopy/gui/tool_launcher.hpp>

using namespace scopy::core;

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QString theme = "default-theme";

	scopy::gui::ThemeManager::getInstance().setApplication(&a);
	scopy::gui::ThemeManager::getInstance().setCurrentTheme(theme);

	//	qDebug() << QDir::current();

	// MainWindow w;
	// w.show();

	scopy::gui::ToolLauncher tl;
	tl.show();

	PluginManager::getInstance();

	return a.exec();
}
