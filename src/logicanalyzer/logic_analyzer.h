/*
 * Copyright (c) 2020 Analog Devices Inc.
 *
 * This file is part of Scopy
 * (see http://www.github.com/analogdevicesinc/scopy).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef LOGIC_ANALYZER_H
#define LOGIC_ANALYZER_H

#include <thread>
#include <mutex>
#include <condition_variable>

#include <QList>
#include <QQueue>
#include <QScrollBar>
#include <QTimer>

#include "logic_tool.h"
#include "oscilloscope_plot.hpp"
#include "buffer_previewer.hpp"
#include "gui/spinbox_a.hpp"
#include "scroll_filter.hpp"
#include "saverestoretoolsettings.h"

#include "genericlogicplotcurve.h"

#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/digital/m2kdigital.hpp>
#include <libm2k/enums.hpp>

// TODO
using namespace libm2k;
using namespace libm2k::digital;
using namespace libm2k::context;

namespace Ui {
class LogicAnalyzer;
class CursorsSettings;
} // namespace Ui

namespace adiscope {

class Filter;
class BaseMenu;
class LogicAnalyzer_API;
class ExportSettings;
class StateUpdater;

namespace logic {

class LogicAnalyzer : public LogicTool {
	Q_OBJECT

	friend class LogicAnalyzer_API;
public:
	explicit LogicAnalyzer(struct iio_context *ctx, Filter *filt,
			 ToolMenuItem *toolMenuItem, QJSEngine *engine,
			 ToolLauncher *parent, bool offline_mode_ = 0);
	~LogicAnalyzer();
	void setNativeDialogs(bool nativeDialogs) override;

public: // Mixed Signal View Interface

	// enable mixed signal view
	// return a list with all the controls available for the Logic Analyzer
	std::vector<QWidget *> enableMixedSignalView(CapturePlot *osc, int oscAnalogChannels);

	// disable mixed signal view
	void disableMixedSignalView();

	// add the curve to the used plot (logic or osc)
	void addCurveToPlot(QwtPlotCurve *curve);

	// get the current plot in use (logic or osc)
	QwtPlot *getCurrentPlot();

	// connect signals and slots for the plot (logic or osc)
	void connectSignalsAndSlotsForPlot(CapturePlot *plot);

	void setData(const uint16_t * const data, int size);
Q_SIGNALS:
	void showTool();

private Q_SLOTS:

	void on_btnChannelSettings_toggled(bool);
	void on_btnCursors_toggled(bool);
	void on_btnTrigger_toggled(bool);

	void on_cursorsBox_toggled(bool on);

	void on_btnSettings_clicked(bool checked);
	void on_btnGeneralSettings_toggled(bool);
	void rightMenuFinished(bool opened);

	void onTimeTriggerValueChanged(double value);

	void onSampleRateValueChanged(double value);
	void onBufferSizeChanged(double value);

	void on_btnStreamOneShot_toggled(bool toggled);

	void on_btnGroupChannels_toggled(bool checked);
	void channelSelectedChanged(int chIdx, bool selected);

	void saveTriggerState();
	void restoreTriggerState();

	void readPreferences();

	void exportData();
	bool exportTabCsv(const QString &separator, const QString &fileName);
	bool exportVcd(const QString &fileName, const QString &startSep, const QString &endSep);

private:
	void setupUi();
	void connectSignalsAndSlots();
	void triggerRightMenuToggle(CustomPushButton *, bool checked);
	void toggleRightMenu(CustomPushButton *, bool checked);
	void settingsPanelUpdate(int id);
	void updateBufferPreviewer(int64_t min, int64_t max);
	void initBufferScrolling();

	void startStop(bool start);

	void setupDecoders();
	void updateStackDecoderButton();
	void updateChannelGroupWidget(bool visible);

	void setupTriggerMenu();

private:
	// TODO: consisten naming (m_ui, m_crUi)
	Ui::LogicAnalyzer *ui;
	Ui::CursorsSettings *cr_ui;
	CustomPlotPositionButton *cursorsPositionButton;

	QList<CustomPushButton *> m_menuOrder;
	QQueue<QPair<CustomPushButton *, bool>> m_menuButtonActions;

	CapturePlot m_plot;
	BufferPreviewer *m_bufferPreviewer;
	QScrollBar *m_plotScrollBar;

	ScaleSpinButton *m_sampleRateButton;
	ScaleSpinButton *m_bufferSizeButton;
	PositionSpinButton *m_timePositionButton;
	double m_sampleRate;
	uint64_t m_bufferSize;
	uint64_t m_lastCapturedSample;

	M2k* m_m2k_context;
	M2kDigital *m_m2kDigital;
	int m_nbChannels;

	QVector<GenericLogicPlotCurve *> m_plotCurves;

	double m_horizOffset;
	double m_timeTriggerOffset;
	bool m_resetHorizAxisOffset;

	// capture
	std::thread *m_captureThread;
	std::atomic<bool> m_stopRequested;
	bool m_acquisitionStarted;
	// prob not needed
	std::mutex m_acquisitionStartedMutex;
	std::condition_variable m_acquisitionStartedCv;

	bool m_started;

	int m_selectedChannel;

	MouseWheelWidgetGuard *m_wheelEventGuard;
	QWidget *m_decoderMenu;

	QVector<int> m_currentGroup;
	BaseMenu *m_currentGroupMenu;

	bool m_autoMode;
	QTimer *m_timer;
	double m_timerTimeout;
	QVector<M2K_TRIGGER_CONDITION_DIGITAL> m_triggerState;


	ExportSettings *m_exportSettings;
	QMap<int, bool> m_exportConfig;

	/* mixed signal view */
	std::unique_ptr<SaveRestoreToolSettings> m_saveRestoreSettings;
	CapturePlot *m_oscPlot;
	int m_oscAnalogChannels;
	int m_oscChannelSelected;
	QVector<GenericLogicPlotCurve *> m_oscPlotCurves;
	QWidget *m_oscDecoderMenu;
	QMetaObject::Connection m_oscChannelSelectedConnection;

	int m_currentKernelBuffers;

	StateUpdater *m_triggerUpdater;


};
} // namespace logic
} // namespace adiscope

#endif // LOGIC_ANALYZER_H
