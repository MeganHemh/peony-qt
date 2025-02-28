#include "status-bar.h"
#include "fm-window.h"
#include "file-info.h"

#include <QLabel>
#include <QPainter>
#include <QApplication>
#include <QStyle>

using namespace Peony;

StatusBar::StatusBar(FMWindow *window, QWidget *parent) : QStatusBar(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setStyleSheet("padding: 0;");
    setSizeGripEnabled(false);
    setMinimumHeight(30);

    m_window = window;
    m_label = new QLabel(this);
    m_label->setContentsMargins(0, 0, 0, 0);
    m_label->setWordWrap(false);
    m_label->setAlignment(Qt::AlignCenter);
    addWidget(m_label, 1);

    //setStyleSheet("align: center;");
    //showMessage(tr("Status Bar"));
}

void StatusBar::update()
{
    if (!m_window)
        return;

    auto selections = m_window->getCurrentSelectionFileInfos();
    if (!selections.isEmpty()) {
        QString directoriesString;
        int directoryCount = 0;
        QString filesString;
        int fileCount = 0;
        goffset size = 0;
        for (auto selection : selections) {
            if(selection->isDir()) {
                directoryCount++;
            } else if (!selection->isVolume()) {
                fileCount++;
                size += selection->size();
            }
        }
        auto format_size = g_format_size(size);
        if (selections.count() == 1) {
            if (directoryCount == 1)
                directoriesString = QString(", %1").arg(selections.first()->displayName());
            if (fileCount == 1)
                filesString = QString(", %1, %2").arg(selections.first()->displayName()).arg(format_size);
        } else if (directoryCount > 1 && (fileCount > 1)){
            directoriesString = tr("; %1 folders").arg(directoryCount);
            filesString = tr("; %1 files, %2 total").arg(fileCount).arg(format_size);
        } else if (directoryCount > 1 && (fileCount > 1)) {
            directoriesString = tr("; %1 folder").arg(directoryCount);
            filesString = tr("; %1 file, %2").arg(fileCount).arg(format_size);
        } else if (fileCount == 0){
            directoriesString = tr("; %1 folders").arg(directoryCount);
        } else {
            filesString = tr("; %1 files, %2 total").arg(fileCount).arg(format_size);
        }

        m_label->setText(tr("%1 selected").arg(selections.count()) + directoriesString + filesString);
        //showMessage(tr("%1 files selected ").arg(selections.count()));
        g_free(format_size);
    } else {
        m_label->setText(m_window->getCurrentUri());
        //showMessage(m_window->getCurrentUri());
    }
}

void StatusBar::paintEvent(QPaintEvent *e)
{
    //I do not want status bar draw the inserted widget's
    //'border', so I use painter overwrite it.
    QStatusBar::paintEvent(e);
    QPainter p(this);
    QPalette palette;
    auto rect = this->rect();
    rect.adjust(1, 2, 1, 1);
    p.fillRect(rect, palette.brush(QPalette::Background));
}
