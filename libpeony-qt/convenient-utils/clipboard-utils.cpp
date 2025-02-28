#include "clipboard-utils.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>

#include "file-operation-manager.h"
#include "file-move-operation.h"
#include "file-copy-operation.h"

using namespace Peony;

static ClipboardUtils *global_instance = nullptr;

ClipboardUtils *ClipboardUtils::getInstance()
{
    if (!global_instance) {
        global_instance = new ClipboardUtils;
    }
    return global_instance;
}

ClipboardUtils::ClipboardUtils(QObject *parent) : QObject(parent)
{
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &ClipboardUtils::clipboardChanged);
}

ClipboardUtils::~ClipboardUtils()
{

}

void ClipboardUtils::release()
{
    delete global_instance;
}

void ClipboardUtils::setClipboardFiles(const QStringList &uris, bool isCut)
{
    if (!global_instance) {
        return;
    }

    auto data = new QMimeData;
    QVariant isCutData = QVariant(isCut);
    data->setData("peony-qt/is-cut", isCutData.toByteArray());
    QList<QUrl> urls;
    for (auto uri : uris) {
        urls<<uri;
    }
    data->setUrls(urls);
    if (isCut) {
        QApplication::clipboard()->setMimeData(data);
    }
}

bool ClipboardUtils::isClipboardHasFiles()
{
    return QApplication::clipboard()->mimeData()->hasUrls();
}

bool ClipboardUtils::isClipboardFilesBeCut()
{
    if (isClipboardHasFiles()) {
        auto data = QApplication::clipboard()->mimeData();
        if (data->hasFormat("peony-qt/is-cut")) {
            QVariant var(data->data("peony-qt/is-cut"));
            return var.toBool();
        }
    }
    return false;
}

QStringList ClipboardUtils::getClipboardFilesUris()
{
    QStringList l;
    if (!isClipboardHasFiles()) {
        return l;
    }
    auto urls = QApplication::clipboard()->mimeData()->urls();
    for (auto url : urls) {
        l<<url.toString();
    }
    return l;
}

void ClipboardUtils::pasteClipboardFiles(const QString &targetDirUri)
{
    if (!isClipboardHasFiles()) {
        return;
    }
    //auto uris = getClipboardFilesUris();
    auto fileOpMgr = FileOperationManager::getInstance();
    if (isClipboardFilesBeCut()) {
        auto moveOp = new FileMoveOperation(getClipboardFilesUris(), targetDirUri);
        fileOpMgr->startOperation(moveOp, true);
    } else {
        auto copyOp = new FileCopyOperation(getClipboardFilesUris(), targetDirUri);
        fileOpMgr->startOperation(copyOp, true);
    }
}

void ClipboardUtils::clearClipboard()
{
    QApplication::clipboard()->clear();
}
