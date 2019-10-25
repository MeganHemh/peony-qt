#ifndef FMWINDOW_H
#define FMWINDOW_H

#include <QMainWindow>
#include "peony-core_global.h"
#include "advanced-location-bar.h"
#include <memory>

#include <QTimer>
#include <QLineEdit>
#include <QComboBox>

class QSplitter;

namespace Peony {

class TabPage;
class SideBar;
class NavigationBar;
class ToolBar;
class SearchBar;
class StatusBar;

class DirectoryViewProxyIface;
class DirectoryViewContainer;

class FileInfo;

/*!
 * \brief The FMWindow class, the normal window of peony-qt's file manager.
 * \details
 * This class show the directory as the common window which used by peony-qt.
 * It contains a tool bar, a navigation bar, a side bar, a preview page and
 * a status bar, and a tab widget container which may conatin over one
 * directory views.
 *
 * The tab view's design is improving and refactoring from peony's window-pane-slot
 * window framework. There is no concept pane in peony-qt (stand by preview page).
 * The window can hold over one 'slot' represent a directory view at a FMWindow
 * instance, but there should be only and just one slot is active in this window.
 *
 * \note
 * The tab view's design is not necessary for a file manager, and it might increased
 * the design difficulty. If you plan to develop a file manager application.
 * You should consider wether it is needed.
 */
class PEONYCORESHARED_EXPORT FMWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit FMWindow(const QString &uri, QWidget *parent = nullptr);

    const QString getCurrentUri();
    const QStringList getCurrentSelections();
    const QStringList getCurrentAllFileUris();
    const QList<std::shared_ptr<FileInfo>> getCurrentSelectionFileInfos();
    DirectoryViewContainer *getCurrentPage();

    QSize sizeHint() const override {return QSize(800, 600);}

Q_SIGNALS:
    void activeViewChanged(const DirectoryViewProxyIface *view);
    void tabPageChanged();

    /*!
     * \brief locationChangeStart
     * \details
     * This signal is used to tell the window doing a location change.
     * When a window is excuting a location change, it should not excute another
     * one util the location change finished.
     */
    void locationChangeStart();
    /*!
     * \brief endLocationChange
     * \details
     * This signal is used to tell the window that a location change finished.
     * Once a location change finished, we can start a new location change.
     */
    void locationChangeEnd();

    void windowSelectionChanged();

public Q_SLOTS:
    void goToUri(const QString &uri, bool addHistory, bool forceUpdate = false);
    void addNewTabs(const QStringList &uris);

    void beginSwitchView(const QString &viewId);

    void refresh();
    void forceStopLoading();
    void advanceSearch();
    void clearRecord();
    void browsePath();
    void searchFilter();
    void filterUpdate();


protected:
    void resizeEvent(QResizeEvent *e) override;

public:
    //advance search filter options
    QStringList m_file_type_list = {"all", "file folder", "image", "video", "text file", "audio", "others"};
    QStringList m_file_mtime_list = {"all", "today", "this week", "this month", "this year", "year ago"};
    QStringList m_file_size_list = {"all", "tiny(0-16K)", "small(16k-1M)", "medium(1M-100M)", "big(100M-1G)","large(>1G)"};

private:
    QSplitter *m_splitter;

    TabPage *m_tab;
    SideBar *m_side_bar;
    NavigationBar *m_navigation_bar;
    ToolBar *m_tool_bar;
    SearchBar *m_search_bar;
    StatusBar *m_status_bar;

    QWidget *m_filter;
    QLineEdit *m_advanced_key;
    QComboBox *typeViewCombox, *timeViewCombox, *sizeViewCombox;
    AdvancedLocationBar *m_advance_bar;

    QTimer m_operation_minimum_interval;
    bool m_is_loading = false;
    bool m_filter_visible;
    bool m_update_condition = false;

    QString m_last_non_search_location;
    QString m_advance_target_path;
};

}

#endif // FMWINDOW_H
