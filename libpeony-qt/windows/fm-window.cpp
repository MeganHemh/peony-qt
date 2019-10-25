#include "fm-window.h"

#include "tab-page.h"
#include "side-bar.h"
#include "navigation-bar.h"
#include "tool-bar.h"
#include "search-bar.h"
#include "status-bar.h"
#include "path-completer.h"
#include "path-edit.h"

#include "search-vfs-uri-parser.h"

#include "directory-view-container.h"
#include "directory-view-plugin-iface.h"

#include "directory-view-menu.h"

#include "file-utils.h"
#include "file-info.h"

#include <QDockWidget>
#include <QStandardPaths>
#include <QDebug>

#include <QSplitter>
#include <QVBoxLayout>
#include <QFormLayout>

#include <QPainter>

#include <QMessageBox>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QStringListModel>
#include <QFileDialog>

using namespace Peony;

FMWindow::FMWindow(const QString &uri, QWidget *parent) : QMainWindow (parent)
{
    m_operation_minimum_interval.setSingleShot(true);

    setAttribute(Qt::WA_DeleteOnClose);
    setAnimated(false);

    auto location = uri;
    if (uri.isEmpty()) {
        location = "file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    }

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->setChildrenCollapsible(false);
    m_splitter->setLayoutDirection(Qt::LeftToRight);
    m_splitter->setHandleWidth(2);
    m_splitter->setStyleSheet("QSplitter"
                              "{"
                              "border: 0;"
                              "padding: 0;"
                              "margin: 0;"
                              "}");

    setCentralWidget(m_splitter);

    m_tab = new TabPage(this);
    m_tab->addPage(uri);

    m_side_bar = new SideBar(this);
    m_filter = new QWidget(this);

    m_splitter->addWidget(m_filter);
    m_splitter->addWidget(m_side_bar);
    m_splitter->addWidget(m_tab);
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 0);
    m_splitter->setStretchFactor(2, 1);

    m_tool_bar = new ToolBar(this, this);
    m_tool_bar->setContentsMargins(0, 0, 0, 0);

    m_search_bar = new SearchBar(this);
    QPushButton *m_advanced_button = new QPushButton(tr("advanced search"), nullptr);
    m_advanced_button->setFixedWidth(110);
    QPushButton *m_clear_record = new QPushButton(tr("clear record"), nullptr);
    m_clear_record->setFixedWidth(80);

    //put the tool bar and search bar into
    //a hobx-layout widget, and put the widget int a
    //new tool bar.
    QWidget *w1 = new QWidget(this);
    w1->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *l1 = new QHBoxLayout(w1);
    l1->setContentsMargins(0, 0, 0, 0);
    w1->setLayout(l1);
    l1->addWidget(m_tool_bar, Qt::AlignLeft);
    l1->addWidget(m_search_bar, Qt::AlignRight);
    l1->addWidget(m_advanced_button, Qt::AlignRight);
    l1->addWidget(m_clear_record, Qt::AlignRight);

    QToolBar *t1 = new QToolBar(this);
    t1->setContentsMargins(0, 0, 10, 0);
    t1->setMovable(false);
    t1->addWidget(w1);

    addToolBar(Qt::TopToolBarArea, t1);
    addToolBarBreak();

    //add mutiple filter page
    m_filter->setFixedWidth(140);
    m_filter->setContentsMargins(0, 0, 0, 0);
    QFormLayout *l2 = new QFormLayout(m_filter);
    l2->setContentsMargins(0, 10, 15, 10);
    m_filter->setLayout(l2);
    QLabel *keyLabel = new QLabel(tr("Key Words"), this);
    m_advanced_key = new QLineEdit(this);
    m_advanced_key->setFixedWidth(120);
    keyLabel->setBuddy(m_advanced_key);
    m_advanced_key->setPlaceholderText(tr("input key words..."));
    QLabel *searchLocation = new QLabel(tr("Search Location"), this);
    m_advance_bar = new AdvancedLocationBar(this);
    m_advance_bar->setFixedWidth(120);
    m_advance_bar->updateLocation(uri);
    m_advance_target_path = uri;

    QPushButton *m_browse_button = new QPushButton(tr("browse"), nullptr);
    m_browse_button->setFixedWidth(60);

    QLabel *fileType = new QLabel(tr("File Type"), this);
    fileType->setFixedWidth(120);
    typeViewCombox = new QComboBox(this);
    typeViewCombox->setToolTip(tr("Choose File Type"));
    typeViewCombox->setFixedWidth(120);
    auto model = new QStringListModel(this);
    model->setStringList(m_file_type_list);
    typeViewCombox->setModel(model);

    QLabel *modifyTime = new QLabel(tr("Modify Time"), this);
    modifyTime->setFixedWidth(120);
    timeViewCombox = new QComboBox(this);
    timeViewCombox->setToolTip(tr("Choose Modify Time"));
    timeViewCombox->setFixedWidth(120);
    auto time_model = new QStringListModel(this);
    time_model->setStringList(m_file_mtime_list);
    timeViewCombox->setModel(time_model);

    QLabel *fileSize = new QLabel(tr("File Size"), this);
    fileSize->setFixedWidth(120);
    sizeViewCombox = new QComboBox(this);
    sizeViewCombox->setToolTip(tr("Choose file size"));
    sizeViewCombox->setFixedWidth(120);
    auto size_model = new QStringListModel(this);
    size_model->setStringList(m_file_size_list);
    sizeViewCombox->setModel(size_model);

    QPushButton *m_filter_button = new QPushButton(tr("search"), nullptr);
    m_filter_button->setFixedWidth(60);
    m_filter_button->setToolTip("start search");

    l2->addWidget(keyLabel);
    l2->addWidget(m_advanced_key);
    l2->addWidget(searchLocation);
    l2->addWidget(m_advance_bar);
    l2->addWidget(m_browse_button);
    l2->addWidget(fileType);
    l2->addWidget(typeViewCombox);
    l2->addWidget(modifyTime);
    l2->addWidget(timeViewCombox);
    l2->addWidget(fileSize);
    l2->addWidget(sizeViewCombox);
    l2->addWidget(m_filter_button);

    m_filter_visible = false;            //default visible setting
    m_filter->setVisible(m_filter_visible);
    //end mutiple filter

    m_navigation_bar = new NavigationBar(this);
    m_navigation_bar->setMovable(false);
    m_navigation_bar->bindContainer(m_tab->getActivePage());
    m_navigation_bar->updateLocation(uri);

    QToolBar *t = new QToolBar(this);
    t->setMovable(false);
    t->addWidget(m_navigation_bar);
    t->setContentsMargins(0, 0, 0, 0);
    addToolBar(t);

    m_status_bar = new StatusBar(this, this);
    setStatusBar(m_status_bar);

    //connect signals
    connect(m_side_bar, &SideBar::updateWindowLocationRequest, this, &FMWindow::goToUri);
    connect(m_tab, &TabPage::updateWindowLocationRequest, this, &FMWindow::goToUri);
    connect(m_navigation_bar, &NavigationBar::updateWindowLocationRequest, this, &FMWindow::goToUri);
    connect(m_navigation_bar, &NavigationBar::refreshRequest, this, &FMWindow::refresh);
    connect(m_advanced_button, &QPushButton::clicked, this, &FMWindow::advanceSearch);
    connect(m_clear_record, &QPushButton::clicked, this, &FMWindow::clearRecord);
    connect(m_browse_button, &QPushButton::clicked, this, &FMWindow::browsePath);
    connect(m_filter_button, &QPushButton::clicked, this, &FMWindow::searchFilter);
    connect(typeViewCombox, QOverload<int>::of(&QComboBox::currentIndexChanged),this, &FMWindow::filterUpdate);
    connect(timeViewCombox, QOverload<int>::of(&QComboBox::currentIndexChanged),this, &FMWindow::filterUpdate);
    connect(sizeViewCombox, QOverload<int>::of(&QComboBox::currentIndexChanged),this, &FMWindow::filterUpdate);

    //tab changed
    connect(m_tab, &TabPage::currentActiveViewChanged, [=](){
        this->m_tool_bar->updateLocation(getCurrentUri());
        this->m_tool_bar->updateStates();
        this->m_navigation_bar->bindContainer(getCurrentPage());
        this->m_navigation_bar->updateLocation(getCurrentUri());
        this->m_status_bar->update();
        Q_EMIT this->tabPageChanged();
        if (m_filter_visible)
        {
            advanceSearch();
            filterUpdate();
        }
    });

    //location change
    connect(m_tab, &TabPage::currentLocationChanged,
            this, &FMWindow::locationChangeEnd);
    connect(this, &FMWindow::locationChangeStart, [=](){
        m_is_loading = true;
        m_side_bar->blockSignals(true);
        m_tool_bar->blockSignals(true);
        m_navigation_bar->setBlock(true);
    });
    connect(this, &FMWindow::locationChangeEnd, [=](){
        //qDebug()<<this->getCurrentAllFileUris();
        m_is_loading = false;
        m_side_bar->blockSignals(false);
        m_tool_bar->blockSignals(false);
        m_navigation_bar->setBlock(false);
        qDebug()<<this->getCurrentUri();
        m_navigation_bar->updateLocation(getCurrentUri());
        m_tool_bar->updateLocation(getCurrentUri());
    });

    //selection changed
    connect(m_tab, &TabPage::currentSelectionChanged, [=](){
        m_status_bar->update();
        m_tool_bar->updateStates();
        Q_EMIT this->windowSelectionChanged();
    });

    //location change
    connect(this, &FMWindow::locationChangeStart, [this](){
        QCursor c;
        c.setShape(Qt::WaitCursor);
        this->setCursor(c);
        m_status_bar->update(tr("Loaing... Press Esc to stop a loading."));
    });

    connect(this, &FMWindow::locationChangeEnd, [this](){
        QCursor c;
        c.setShape(Qt::ArrowCursor);
        this->setCursor(c);
        m_status_bar->update();
    });

    //view switched
    connect(m_tab, &TabPage::viewTypeChanged, [=](){
        m_tool_bar->updateStates();
    });

    //search
    connect(m_search_bar, &SearchBar::searchKeyChanged, [=](){
        //FIXME: filter the current directory
    });
    connect(m_search_bar, &SearchBar::searchRequest, [=](const QString &key){
        QString uri = this->getCurrentUri();
        if (uri.startsWith("search:///")) {
            uri = m_last_non_search_location;
        }
        m_update_condition = false; //common search, no filter
        auto targetUri = SearchVFSUriParser::parseSearchKey(uri, key);
        this->goToUri(targetUri, true);
    });

    //action
    QAction *stopLoadingAction = new QAction(this);
    stopLoadingAction->setShortcut(QKeySequence(Qt::Key_Escape));
    addAction(stopLoadingAction);
    connect(stopLoadingAction, &QAction::triggered, this, &FMWindow::forceStopLoading);

    //menu
    m_tab->connect(m_tab, &TabPage::menuRequest, [=](const QPoint &pos){
        if (m_is_loading)
            return;
        DirectoryViewMenu menu(this, nullptr);
        menu.exec(pos);
    });

    //advance search change Location
    connect(m_advance_bar, &AdvancedLocationBar::updateWindowLocationRequest, [=](const QString &uri){
        if (uri.contains("file://"))
            m_advance_target_path = uri;
        else
            m_advance_target_path = "file://" + uri;
    });
}

const QString FMWindow::getCurrentUri()
{
    if (m_tab->getActivePage()) {
        return m_tab->getActivePage()->getCurrentUri();
    }
    return nullptr;
}

const QStringList FMWindow::getCurrentAllFileUris()
{
    if (m_tab->getActivePage()) {
        return m_tab->getActivePage()->getAllFileUris();
    }
    return QStringList();
}

const QStringList FMWindow::getCurrentSelections()
{
    if (m_tab->getActivePage()) {
        return m_tab->getActivePage()->getCurrentSelections();
    }
    return QStringList();
}

const QList<std::shared_ptr<FileInfo>> FMWindow::getCurrentSelectionFileInfos()
{
    const QStringList uris = getCurrentSelections();
    QList<std::shared_ptr<FileInfo>> infos;
    for(auto uri : uris) {
        auto info = FileInfo::fromUri(uri);
        infos<<info;
    }
    return infos;
}

void FMWindow::addNewTabs(const QStringList &uris)
{
    for (auto uri : uris) {
        m_tab->addPage(uri);
    }
}

void FMWindow::goToUri(const QString &uri, bool addHistory, bool forceUpdate)
{
    if (forceUpdate)
        goto update;

    if (getCurrentUri() == uri)
        return;

update:
    if (!uri.startsWith("search://")) {
        m_last_non_search_location = uri;
    }
    Q_EMIT locationChangeStart();
    if (m_update_condition)
        filterUpdate();
    m_tab->getActivePage()->goToUri(uri, addHistory, forceUpdate);
    m_tab->refreshCurrentTabText();
    m_navigation_bar->updateLocation(uri);
    m_tool_bar->updateLocation(uri);
}

void FMWindow::beginSwitchView(const QString &viewId)
{
    m_tab->getActivePage()->switchViewType(viewId);
}

void FMWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
}

DirectoryViewContainer *FMWindow::getCurrentPage()
{
    return m_tab->getActivePage();
}

void FMWindow::refresh()
{
    if (m_operation_minimum_interval.isActive()) {
        return;
    }
    m_operation_minimum_interval.start(500);
    if (m_filter_visible)
    {
        advanceSearch();
        filterUpdate();
    }
    m_tab->getActivePage()->refresh();
}

void FMWindow::advanceSearch()
{
    qDebug()<<"advanceSearch clicked";
    if (! m_filter_visible)
    {
        //before show, update cur uri
        QString target_path = getCurrentUri();
        if (target_path.contains("file://"))
            m_advance_target_path = target_path;
        else
            m_advance_target_path = "file://" + target_path;
        m_advance_bar->updateLocation(target_path);
    }
    m_filter_visible = !m_filter_visible;
    m_filter->setVisible(m_filter_visible);
    //m_side_bar->setVisible(!m_filter_visible);
}

void FMWindow::clearRecord()
{
    qDebug()<<"clearRecord clicked";
    m_search_bar->clear_search_record();
}

void FMWindow::browsePath()
{
    QString target_path = QFileDialog::getExistingDirectory(this, "caption", getCurrentUri(), QFileDialog::ShowDirsOnly);
    qDebug()<<"browsePath Opened:"<<target_path;
    //add root prefix
    if (target_path.contains("file://"))
        m_advance_target_path = target_path;
    else
        m_advance_target_path = "file://" + target_path;
    m_advance_bar->updateLocation(target_path);
}

void FMWindow::searchFilter()
{
    qDebug()<<"searchFilter clicked"<<m_advanced_key->text()<<"path:"<<m_advance_target_path;
    if (m_advanced_key->text() == nullptr || m_advance_target_path == nullptr) //must have key words and target path
        return;
    if (typeViewCombox->currentIndex() > 0)
        qDebug()<<"Filter file type:"<<typeViewCombox->currentText();
    if (timeViewCombox->currentIndex() > 0)
        qDebug()<<"Filter modify time:"<<timeViewCombox->currentText();
    if (sizeViewCombox->currentIndex() > 0)
        qDebug()<<"Filter file size:"<<sizeViewCombox->currentText();

    auto targetUri = SearchVFSUriParser::parseSearchKey(m_advance_target_path, m_advanced_key->text());
    qDebug()<<"targeturi:"<<targetUri;
    m_update_condition = true;
    this->goToUri(targetUri, true);
}

void FMWindow::filterUpdate()
{
    qDebug()<<"filterUpdate:";
    m_tab->getActivePage()->setSortFilter(typeViewCombox->currentIndex(), timeViewCombox->currentIndex(), sizeViewCombox->currentIndex());
}

void FMWindow::forceStopLoading()
{
    m_tab->getActivePage()->stopLoading();
    m_is_loading = false;
}
