#include "pulseview/pv/mainwindow.hpp"
#include "pulseview/pv/view/view.hpp"
#include "pulseview/pv/view/tracetreeitem.hpp"
#include "pg_channel_manager.hpp"
#include "pattern_generator.hpp"
#include "dynamicWidget.hpp"

#include "ui_pg_channel_group.h"
#include "ui_pg_channel_manager.h"
#include "ui_pg_channel_header.h"
#include "ui_pg_channel.h"


namespace Ui {
class PGChannelGroup;
class PGChannel;
class PGChannelManager;
class PGChannelManagerHeader;
}

namespace pv
{
class MainWindow;
namespace view
{
class View;
class TraceTreeItem;
}
}

namespace adiscope
{


//////////////////////// CHANNEL
PatternGeneratorChannel::PatternGeneratorChannel(uint16_t id_, std::string label_) : Channel(id_,label_), channel_role("None"),trigger("rising")
{

}
std::string PatternGeneratorChannel::getTrigger() const
{
    return trigger;
}

void PatternGeneratorChannel::setTrigger(const std::string &value)
{
    trigger = value;
}

std::string PatternGeneratorChannel::getChannel_role() const
{
    return channel_role;
}

void PatternGeneratorChannel::setChannel_role(const std::string &value)
{
    channel_role = value;
}
///////////////////////// CHANNEL UI
PatternGeneratorChannelUI::PatternGeneratorChannelUI(PatternGeneratorChannel* ch, PatternGeneratorChannelGroup* chg, PatternGeneratorChannelManagerUI* managerUi, QWidget *parent) : ChannelUI(ch,parent), managerUi(managerUi), chg(chg)
{
    this->ch = ch;
    qDebug()<<"PGChannelUI created";
}
PatternGeneratorChannelUI::~PatternGeneratorChannelUI(){
    qDebug()<<"PGChannelUI destroyed";
}

/////////////////////// CHANNEL GROUP
std::string PatternGeneratorChannelGroup::getDecoder() const
{
    return decoder;
}

void PatternGeneratorChannelGroup::setDecoder(const std::string &value)
{
    decoder = value;
}

PatternGeneratorChannelGroup::PatternGeneratorChannelGroup(PatternGeneratorChannel* ch) : ChannelGroup(ch)
{
    created_index = 0;
    collapsed = false;
    pattern=PatternFactory::create(0);
}

PatternGeneratorChannelGroup::~PatternGeneratorChannelGroup()
{
    qDebug()<<"pgchannelgroupdestroyed";
    if(pattern)
    {
            delete pattern;
            pattern = nullptr;
    }

}

bool PatternGeneratorChannelGroup::isCollapsed()
{
    return collapsed;
}

void PatternGeneratorChannelGroup::collapse(bool val)
{
    collapsed = val;
}

void PatternGeneratorChannelGroup::append(PatternGeneratorChannelGroup* tojoin)
{
    for(auto i=0;i<tojoin->channels.size();i++)
        channels.push_back(tojoin->channels[i]);
}


////////////////////////// CHANNEL GROUP UI
PatternGeneratorChannelManagerUI *PatternGeneratorChannelGroupUI::getManagerUi() const
{
    return managerUi;
}

PatternGeneratorChannelGroupUI::PatternGeneratorChannelGroupUI(PatternGeneratorChannelGroup* chg, PatternGeneratorChannelManagerUI* managerUi, QWidget *parent) : ChannelGroupUI(chg,parent), managerUi(managerUi)
{
    this->chg = chg;
    checked = false;
}

PatternGeneratorChannelGroup* PatternGeneratorChannelGroupUI::getChannelGroup()
{
    return static_cast<PatternGeneratorChannelGroup*>(this->chg);
}

void PatternGeneratorChannelGroupUI::set_decoder(std::string value)
{
    getChannelGroup()->setDecoder(value);
}

void PatternGeneratorChannelGroupUI::patternChanged(int index)
{
    getChannelGroup()->pattern->deinit();
    delete getChannelGroup()->pattern;
    getChannelGroup()->created_index=index;
    getChannelGroup()->pattern = PatternFactory::create(index);
    if(getChannelGroup()==getManagerUi()->getSelectedChannelGroup())
    {
        getManagerUi()->deleteSettingsWidget();
        getManagerUi()->createSettingsWidget();
    }
}

void PatternGeneratorChannelGroupUI::select(bool selected)
{
    ChannelGroupUI::select(selected);
    Q_EMIT channel_selected();
}

void PatternGeneratorChannelGroupUI::collapse()
{
    bool val = !getChannelGroup()->isCollapsed();
    getChannelGroup()->collapse(val);
    findChild<MenuAnim*>("subChannelWidget")->setVisible(!val);
    //findChild<MenuAnim*>("subChannelWidget")->toggleMenu(val);
}

void PatternGeneratorChannelUI::split()
{
    auto channelGroups = managerUi->chm->get_channel_groups();
    auto chgIter = std::find(channelGroups->begin(),channelGroups->end(),chg);
    if(chgIter == channelGroups->end())
    {
        return;
    }
    auto chgIndex = chgIter-channelGroups->begin();
    auto channels = *(managerUi->chm->get_channel_group(chgIndex)->get_channels());
    auto chIter = std::find(channels.begin(),channels.end(),ch);
    if(chIter==channels.end())
    {
        return;
    }
    auto chIndex = chIter-channels.begin();

    managerUi->chm->splitChannel(chgIndex,chIndex);
    managerUi->updateUi();

}

void PatternGeneratorChannelGroupUI::split()
{
    getManagerUi()->chm->deselect_all();
    select(true);
    getManagerUi()->groupSplitSelected();
    getManagerUi()->updateUi();
}

void PatternGeneratorChannelGroupUI::enableControls(bool enabled)
{
    findChild<QLabel*>("ChannelGroupLabel")->setEnabled(enabled);
    findChild<QLabel*>("DioLabel")->setEnabled(enabled);
    findChild<QComboBox*>("patternCombo")->setEnabled(enabled);
    findChild<QComboBox*>("outputCombo")->setEnabled(enabled);
    findChild<QWidget*>("subChannelWidget")->setEnabled(enabled);
}

void PatternGeneratorChannelGroupUI::enable(bool enabled)
{
    ChannelGroupUI::enable(enabled);
    enableControls(enabled);
    Q_EMIT channel_enabled();
}

void PatternGeneratorChannelGroupUI::settingsButtonHandler()
{
    getManagerUi()->deleteSettingsWidget();
    getManagerUi()->selectChannelGroup(this);
    getManagerUi()->createSettingsWidget();
}

int PatternGeneratorChannelGroupUI::isChecked()
{
    return checked;
}

void PatternGeneratorChannelGroupUI::check(int val)
{
    checked = val;
}

void PatternGeneratorChannelGroupUI::mousePressEvent(QMouseEvent*)
{
    bool checked = property("checked").toBool();
    setProperty("checked", !checked);

    setDynamicProperty(findChild<QWidget*>("widget_2"),"selected",checked);
    style()->unpolish(this);
    style()->polish(this);
    update();
}



/////////////////////////// CHANNEL MANAGER
PatternGeneratorChannelManager::PatternGeneratorChannelManager() : ChannelManager()
{
    for(auto i=0;i<16;i++)
    {
        std::string temp = "DIO" + std::to_string(i);
        channel.push_back(new PatternGeneratorChannel(i,temp));
    }
    auto temp = static_cast<PatternGeneratorChannel*>(channel.back());
    for(auto&& ch : channel)
    {
        channel_group.push_back(new PatternGeneratorChannelGroup(static_cast<PatternGeneratorChannel*>(ch)));
    }

}

PatternGeneratorChannelManager::~PatternGeneratorChannelManager()
{
    for(auto ch : channel_group)
    {
        delete ch;
    }
    for(auto ch : channel)
    {
        delete ch;
    }
}

PatternGeneratorChannelGroup* PatternGeneratorChannelManager::get_channel_group(int index)
{
    return static_cast<PatternGeneratorChannelGroup*>(channel_group[index]);
}

/*std::vector<PatternGeneratorChannelGroup*>* PatternGeneratorChannelManager::getChannelGroups()
{
    return static_cast<std::vector<PatternGeneratorChannelGroup*>*>(&channel_group);
}*/

void PatternGeneratorChannelManager::join(std::vector<int> index)
{
    for(auto i=1;i<index.size();i++){
        auto it = std::next(channel_group.begin(), index[i]);
        static_cast<PatternGeneratorChannelGroup*>(channel_group[index[0]])->append(static_cast<PatternGeneratorChannelGroup*>(channel_group[index[i]]));
        delete *it;
        channel_group.erase(it);
        for(auto j=0;j<index.size();j++)
        {
            if(index[i] < index[j]) index[j]--;
        }
    }
    channel_group[index[0]]->group(true);
    channel_group[index[0]]->set_label("Group name");
}

void PatternGeneratorChannelManager::split(int index)
{
    auto it = std::next(channel_group.begin(), index);
    it++;
    for(auto&& subch : *(channel_group[index]->get_channels()))
    {
        channel_group.insert(it,new PatternGeneratorChannelGroup(static_cast<PatternGeneratorChannel*>(subch)));
        it++;
    }
    it = std::next(channel_group.begin(), index);
    delete static_cast<PatternGeneratorChannelGroup*>(*it);
    channel_group.erase(it);
}

void PatternGeneratorChannelManager::splitChannel(int chgIndex, int chIndex)
{

    auto it = channel_group.begin()+chgIndex+1; // Use this to insert split channel after channelgroup
    // auto it = channel_group.begin()+chgIndex; // Use this to insert split channel before channelgroup
    auto subch = channel_group[chgIndex]->get_channel(chIndex);
    auto chIt = channel_group[chgIndex]->get_channels()->begin()+chIndex;
    channel_group.insert(it, new PatternGeneratorChannelGroup(static_cast<PatternGeneratorChannel*>(subch)));

    auto newChgIndex = chgIndex; // Use this to insert split channel after channelgroup
    // auto newChgIndex = chgIndex; // Use this to insert split channel before channelgroup
    channel_group[newChgIndex]->get_channels()->erase(chIt);

    if(channel_group[newChgIndex]->get_channel_count()==0)
    {
        it = channel_group.begin()+newChgIndex;
        delete static_cast<PatternGeneratorChannelGroup*>(*it);
        channel_group.erase(it);
    }

}

////////////////////////////////////// CHANNEL MANAGER UI
QWidget *PatternGeneratorChannelManagerUI::getSettingsWidget() const
{
    return settingsWidget;
}

PatternGeneratorChannelGroup *PatternGeneratorChannelManagerUI::getSelectedChannelGroup() const
{
    return selectedChannelGroup;
}

void PatternGeneratorChannelManagerUI::setSelectedChannelGroup(PatternGeneratorChannelGroup *value)
{
    selectedChannelGroup = value;
}

PatternGeneratorChannelManagerUI::PatternGeneratorChannelManagerUI(QWidget *parent, pv::MainWindow *main_win_, PatternGeneratorChannelManager *chm, QWidget *settingsWidget, PatternGenerator* pg)  : QWidget(parent),  ui(new Ui::PGChannelManager), settingsWidget(settingsWidget)
{
    ui->setupUi(this);
    main_win = main_win_;
    this->chm = chm;
    this->pg = pg;
    currentUI = nullptr;
    selectedChannelGroup = chm->get_channel_group(0);
    selectedChannelGroupUi = nullptr;
    channelButtonGroup = new QButtonGroup(this);
    disabledShown = true;
    detailsShown = true;
    channelManagerHeaderWiget = nullptr;

   // update_ui();
}
PatternGeneratorChannelManagerUI::~PatternGeneratorChannelManagerUI()
{

}

void PatternGeneratorChannelManagerUI::retainWidgetSizeWhenHidden(QWidget *w)
{
    QSizePolicy sp_retain = w->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    w->setSizePolicy(sp_retain);
}

void PatternGeneratorChannelManagerUI::setWidgetNrOfChars(QWidget *w, int minNrOfChars, int maxNrOfChars)
{
    QFontMetrics labelm(w->font());

    auto label_min_width = labelm.width(QString(minNrOfChars,'X'));
    w->setMinimumWidth(label_min_width);
    if(maxNrOfChars!=0)
    {
        auto label_max_width = labelm.width(QString(maxNrOfChars,'X'));
        w->setMaximumWidth(label_max_width);
    }
}

void PatternGeneratorChannelManagerUI::updateUi()
{
    static const int channelGroupLabelMaxLength = 10;
    static const int dioLabelMaxLength = 2;
    static const int channelComboMaxLength = 15;
    static const int outputComboMaxLength = 5;


    for(auto ch : chg_ui)
    {
        delete ch;
    }

    chg_ui.erase(chg_ui.begin(),chg_ui.end());

    if(channelManagerHeaderWiget != nullptr)
    {
        delete channelManagerHeaderWiget;
        channelManagerHeaderWiget = nullptr;
    }

    channelManagerHeaderWiget = new QWidget(ui->chmHeaderSlot);
    Ui::PGChannelManagerHeader *chmHeader = new Ui::PGChannelManagerHeader();
    chmHeader->setupUi(channelManagerHeaderWiget);
    ui->chmHeaderSlotLayout->addWidget(channelManagerHeaderWiget);

    auto offset = 0;
    ensurePolished();
    setWidgetNrOfChars(chmHeader->labelName, channelGroupLabelMaxLength);
    setWidgetNrOfChars(chmHeader->labelDIO, dioLabelMaxLength);
    setWidgetNrOfChars(chmHeader->labelType, channelComboMaxLength);
    setWidgetNrOfChars(chmHeader->labelOutput, outputComboMaxLength);
    chmHeader->labelView->setMinimumWidth(40);
    chmHeader->labelSelect->setMinimumWidth(40);

    if(!detailsShown)
    {
        chmHeader->wgHeaderEnableGroup->setVisible(false);
        chmHeader->wgHeaderSettingsGroup->setVisible(false);
        chmHeader->wgHeaderSelectionGroup->setVisible(false);
    }

    for(auto&& ch : *(chm->get_channel_groups()))
    {
        if(disabledShown==false && !ch->is_enabled()) continue;

        chg_ui.push_back(new PatternGeneratorChannelGroupUI(static_cast<PatternGeneratorChannelGroup*>(ch),this, 0)); // create widget for channelgroup
        PatternGeneratorChannelGroupUI* currentChannelGroupUI = chg_ui.back();

        Ui::PGChannelGroup *pgchannelgroupui = new Ui::PGChannelGroup();
        pgchannelgroupui->setupUi(chg_ui.back());
        ui->verticalLayout->insertWidget(chg_ui.size(),chg_ui.back());
        currentChannelGroupUI->ensurePolished();

        retainWidgetSizeWhenHidden(pgchannelgroupui->collapseBtn);
        retainWidgetSizeWhenHidden(pgchannelgroupui->line);
        retainWidgetSizeWhenHidden(pgchannelgroupui->line_2);

        if(!detailsShown)
        {
            pgchannelgroupui->wgChannelEnableGroup->setVisible(false);
            pgchannelgroupui->wgChannelSelectionGroup->setVisible(false);
            pgchannelgroupui->wgChannelSettingsGroup->setVisible(false);
        }
        else
        {
            retainWidgetSizeWhenHidden(pgchannelgroupui->splitBtn);
        }


        setWidgetNrOfChars(pgchannelgroupui->ChannelGroupLabel, channelGroupLabelMaxLength);

        QString channelGroupLabel = QString().fromStdString(ch->get_label());
        if(channelGroupLabel.length()>channelGroupLabelMaxLength)
        {
            channelGroupLabel.truncate(channelGroupLabelMaxLength-2);
            channelGroupLabel+="..";
        }
        pgchannelgroupui->ChannelGroupLabel->setText(channelGroupLabel);

        setWidgetNrOfChars(pgchannelgroupui->DioLabel, dioLabelMaxLength);
        setWidgetNrOfChars(pgchannelgroupui->patternCombo, channelComboMaxLength);
        setWidgetNrOfChars(pgchannelgroupui->outputCombo, outputComboMaxLength);

        int i = 0;
        for(auto var : PatternFactory::get_ui_list())
        {
            pgchannelgroupui->patternCombo->addItem(var);
            pgchannelgroupui->patternCombo->setItemData(i, (PatternFactory::get_description_list())[i],Qt::ToolTipRole);
            i++;
        }

        pgchannelgroupui->enableBox->setChecked(ch->is_enabled());
        currentChannelGroupUI->enableControls(ch->is_enabled());
//        channelButtonGroup->addButton(pgchannelgroupui->pushButton);
        channelButtonGroup->setExclusive(true);

        connect(pgchannelgroupui->enableBox,SIGNAL(toggled(bool)),chg_ui.back(),SLOT(enable(bool)));
        connect(static_cast<PatternGeneratorChannelGroupUI*>(chg_ui.back()),SIGNAL(channel_selected()),pg,SLOT(onChannelSelectedChanged())); // TEMP
        connect(static_cast<PatternGeneratorChannelGroupUI*>(chg_ui.back()),SIGNAL(channel_enabled()),pg,SLOT(onChannelEnabledChanged())); // TEMP
//        connect(pgchannelgroupui->pushButton,SIGNAL(clicked()),static_cast<PatternGeneratorChannelGroupUI*>(chg_ui.back()),SLOT(settingsButtonHandler()));
//        connect(pgchannelgroupui->pushButton,SIGNAL(clicked()),pg,SLOT(toggleRightMenu()));
        connect(pgchannelgroupui->selectBox,SIGNAL(toggled(bool)),static_cast<PatternGeneratorChannelGroupUI*>(chg_ui.back()),SLOT(select(bool)));
        connect(pgchannelgroupui->patternCombo,SIGNAL(currentIndexChanged(int)),chg_ui.back(),SLOT(patternChanged(int)));


        //chg_ui.back()->setStyleSheet("QWidget\n{\nborder-top:1px solid red;\nborder-bottom:1px solid red;\n }\n");

        offset+=(chg_ui.back()->geometry().bottomRight().y()-10);
        if(ch->is_grouped()) // create subwidgets
        {
            pgchannelgroupui->DioLabel->setText("");

            connect(pgchannelgroupui->collapseBtn,SIGNAL(clicked()),chg_ui.back(),SLOT(collapse()));
            connect(pgchannelgroupui->splitBtn,SIGNAL(clicked()),chg_ui.back(),SLOT(split()));

            for(auto i=0;i<ch->get_channel_count();i++)
                {
                    Ui::PGChannelGroup *pgchannelui = new Ui::PGChannelGroup();

                    currentChannelGroupUI->ch_ui.push_back(new PatternGeneratorChannelUI(static_cast<PatternGeneratorChannel*>(ch->get_channel(i)), static_cast<PatternGeneratorChannelGroup*>(ch), this, 0)); // create widget for channelgroup
                    PatternGeneratorChannelUI* currentChannelUI = currentChannelGroupUI->ch_ui.back();
                    //QWidget *p = new QWidget(chg_ui.back());
                    pgchannelui->setupUi(currentChannelUI);
                    pgchannelgroupui->subChannelLayout->insertWidget(i,currentChannelUI);
                    currentChannelGroupUI->ensurePolished();

                    retainWidgetSizeWhenHidden(pgchannelui->collapseBtn);
                    retainWidgetSizeWhenHidden(pgchannelui->line);
                    retainWidgetSizeWhenHidden(pgchannelui->line_2);

                    if(!detailsShown)
                    {
                        pgchannelui->wgChannelSelectionGroup->setVisible(false);
                    }
                    else
                    {
                        //retainWidgetSizeWhenHidden(pgchannelui->enableBox);
                        retainWidgetSizeWhenHidden(pgchannelui->wgChannelSettingsGroup);
                        retainWidgetSizeWhenHidden(pgchannelui->wgChannelEnableGroup);
                        retainWidgetSizeWhenHidden(pgchannelui->outputCombo);
                        retainWidgetSizeWhenHidden(pgchannelui->selectBox);
                    }

                    pgchannelui->line->setVisible(false);
                    pgchannelui->line_2->setVisible(false);

                    setWidgetNrOfChars(pgchannelui->ChannelGroupLabel, channelGroupLabelMaxLength);
                    setWidgetNrOfChars(pgchannelui->DioLabel, dioLabelMaxLength);
                    setWidgetNrOfChars(pgchannelui->patternCombo, channelComboMaxLength);
                    setWidgetNrOfChars(pgchannelui->outputCombo, outputComboMaxLength);

                    pgchannelui->wgChannelEnableGroup->setVisible(false);
                    pgchannelui->wgChannelSettingsGroup->setVisible(false);
                    pgchannelui->collapseBtn->setVisible(false);
                    pgchannelui->selectBox->setVisible(false);

                    connect(pgchannelui->splitBtn,SIGNAL(clicked()),currentChannelUI,SLOT(split()));

                    auto str = QString().fromStdString(ch->get_channel(i)->get_label());
                    pgchannelui->ChannelGroupLabel->setText(str);
                    str = QString().number(ch->get_channel(i)->get_id());
                    pgchannelui->DioLabel->setText(str);
                    auto x = pgchannelgroupui->ChannelGroupLabel->geometry().x();

                    //auto index = ch->get_channel(i)->get_id();
                    //offset+=p->geometry().bottomRight().y();

                }
            if(static_cast<PatternGeneratorChannelGroup*>(ch)->isCollapsed())
            {
                pgchannelgroupui->collapseBtn->setChecked(true);
                pgchannelgroupui->subChannelWidget->setVisible(false);
            }
        }
        else
        {
            auto index = ch->get_channel()->get_id();            
            pgchannelgroupui->DioLabel->setText(QString().number(ch->get_channel()->get_id()));            
            pgchannelgroupui->splitBtn->setVisible(false);
            pgchannelgroupui->collapseBtn->setVisible(false);
            pgchannelgroupui->line->setVisible(false);
            pgchannelgroupui->line_2->setVisible(false);

        }
    }

}

void PatternGeneratorChannelManagerUI::deleteSettingsWidget()
{
    if(currentUI!=nullptr)
    {
        currentUI->setVisible(false);
        currentUI->destroy_ui();
        delete currentUI;
        currentUI = nullptr;
    }
}


void PatternGeneratorChannelManagerUI::createSettingsWidget()
{    
    currentUI = PatternFactory::create_ui(selectedChannelGroup->pattern,selectedChannelGroup->created_index);
    currentUI->build_ui(settingsWidget);
    currentUI->get_pattern()->init();
    currentUI->post_load_ui();
    currentUI->setVisible(true);        
}


void PatternGeneratorChannelManagerUI::selectChannelGroup(PatternGeneratorChannelGroupUI* selected)
{
    // remove black outline from prev selected
    if(selectedChannelGroupUi)
        selectedChannelGroupUi->findChild<QWidget*>("widget")->setStyleSheet("");
    // select channel
    selectedChannelGroupUi = selected;
    selectedChannelGroup=selected->getChannelGroup();
    selectedChannelGroupUi->findChild<QWidget*>("widget")->setStyleSheet("QWidget {background-color: rgb(0, 0, 0);}");
    // apply black outline to currently selected channel
}

bool PatternGeneratorChannelManagerUI::isDisabledShown()
{
    return disabledShown;
}

void PatternGeneratorChannelManagerUI::showDisabled()
{
    disabledShown = true;
}

void PatternGeneratorChannelManagerUI::hideDisabled()
{
    disabledShown = false;
}

bool PatternGeneratorChannelManagerUI::areDetailsShown()
{
    return detailsShown;
}

void PatternGeneratorChannelManagerUI::showDetails()
{
    detailsShown = true;
}

void PatternGeneratorChannelManagerUI::hideDetails()
{
    detailsShown = false;
}


void PatternGeneratorChannelManagerUI::groupSplitSelected()
{
    std::vector<int> selection = chm->get_selected_indexes();
    if(selection.size() == 0) {return;}
    if(selection.size() == 1)
    {
        if(chm->get_channel_group(selection.back())->is_grouped())
        {
            chm->split(selection.back());
        }
        else
        {
            chm->join(selection);
        }
        chm->deselect_all();
    }
    else
    {
        chm->join(selection);
        chm->deselect_all();
    }
}
}



