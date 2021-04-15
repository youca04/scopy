#include "dynamic_widget.hpp"
#include "tool.hpp"

#include "ui_tool_launcher.h"

#include <scopy/gui/tool_launcher.hpp>
#include <scopy/gui/filter.hpp>
#include <iio.h>
#include <scopy/core/plugin_manager.hpp>

using namespace scopy::gui;
using namespace scopy::core;

ToolLauncher::ToolLauncher(QWidget* parent)
	: QMainWindow(parent)
	, m_ui(new Ui::ToolLauncher)
	, m_toolMenu(nullptr)
	, m_about(nullptr)
	, m_current(nullptr)
{
	m_ui->setupUi(this);

	setupUi();

	// Visualize all connected uris
	m_contextEnumerator = new scopy::core::ContextEnumerator();
	connect(m_contextEnumerator, &scopy::core::ContextEnumerator::printData, this,
		&scopy::gui::ToolLauncher::detectedUris);

	m_plugins = scopy::core::PluginManager::getInstance().getPlugins();
}

void ToolLauncher::swapMenu(QWidget* menu)
{
	Tool* tl = dynamic_cast<Tool*>(menu);

	if (m_current) {
		m_current->setVisible(false);
		m_ui->hLayoutCentral->removeWidget(m_current);
	}

	m_current = menu;

	m_ui->hLayoutCentral->addWidget(m_current);
	m_current->setVisible(true);
}

void ToolLauncher::setTestLbl(const QString& text) { m_ui->lblTestDevFound->setText(text); }

void ToolLauncher::setupUi()
{
	m_prefPanel = new Preferences(this);
	m_prefPanel->setVisible(false);

	m_notesPanel = new UserNotes(this);
	m_notesPanel->setVisible(false);

	connect(m_ui->prefBtn, &QPushButton::clicked, [=]() { swapMenu(static_cast<QWidget*>(m_prefPanel)); });
	connect(m_ui->btnNotes, &QPushButton::clicked, [=]() { swapMenu(static_cast<QWidget*>(m_notesPanel)); });
	connect(m_ui->btnHome, &QPushButton::clicked, [=]() { swapMenu(static_cast<QWidget*>(m_ui->widgetHome)); });

	m_about = new ScopyAboutDialog(this);
	connect(m_ui->btnAbout, &QPushButton::clicked, [=]() {
		if (!m_about)
			m_about = new ScopyAboutDialog(this);
		m_about->setModal(false);
		m_about->show();
		m_about->raise();
		m_about->activateWindow();
	});

	connect(m_ui->menuAnimMenu, &MenuAnim::finished, [=](bool opened) {
		m_ui->saveLbl->setVisible(opened);
		m_ui->loadLbl->setVisible(opened);
		m_ui->prefBtn->setText(opened ? tr("Preferences") : "");
		//		m_toolMenu->hideMenuText(!opened);
	});

	connect(m_ui->btnHome, &QPushButton::clicked, [=]() { swapMenu(static_cast<QWidget*>(m_ui->widgetHome)); });

	m_ui->saveBtn->parentWidget()->setEnabled(false);
	m_ui->loadBtn->parentWidget()->setEnabled(true);

	m_ui->btnHome->toggle();

	// TO DO: Remove temporary spaces
	// set home icon
	m_ui->btnHome->setText("   Home");
	m_ui->btnHome->setIcon(QIcon::fromTheme("house"));
	m_ui->btnHome->setIconSize(QSize(32, 32));

	m_current = m_ui->widgetHome;


	// setup tool menu
	//

	m_toolMenu = new ToolMenu(nullptr, m_ui->menuContainer);
	m_ui->menuContainerLayout->addWidget(m_toolMenu);
}

void ToolLauncher::detectedUris(const QStringList& uris)
{
	QString text = std::accumulate(uris.cbegin(), uris.cend(), QString{});
	this->setTestLbl(text);
	m_contextEnumerator->stop();
	// device clicked!
	auto tempCtx = iio_create_context_from_uri(uris[0].toStdString().c_str());
	if (tempCtx) {
		auto tempFilter = new Filter(tempCtx);
		m_toolMenu->loadToolsFromFilter(tempFilter);
		delete tempFilter;
//		iio_context_destroy(tempCtx);
	}

	std::vector<ToolInterface*> tools;

	// when connect is clicked
	// lets go through the plugins and see what we have compatible with this ctx
	for (PluginInterface *plugin : m_plugins) {
		tools = plugin->getTools(tempCtx, m_toolMenu);
//		for (ToolInterface *tl : tools) {
//			tl->getWidget()->show();
////			swapMenu(tl->getWidget());
//		}
//		swapMenu(tools[0]->getWidget());
//		tools[1]->getWidget()->show();
	}

	connect(m_toolMenu, &ToolMenu::toolSelected, [=](int tool) {
		qDebug() << "tool selected: " << tool;
		for (ToolInterface *tl : tools) {
			if (tl->getIdentifier() == tool) {
				swapMenu(tl->getWidget());
			}
		}
	});


}

void ToolLauncher::onBtnHomeClicked() { swapMenu(m_ui->widgetHome); }
