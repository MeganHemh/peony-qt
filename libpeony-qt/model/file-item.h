#ifndef FILEITEM_H
#define FILEITEM_H

#include "peony-core_global.h"
#include <memory>

#include <QObject>
#include <QVector>

namespace Peony {

class FileInfo;
class FileInfoManager;
class FileItemModel;
class FileWatcher;
class FileItemProxyFilterSortModel;

/*!
 * \brief The FileItem class
 * <br>
 * FileItem is the absctract item class contract with FileItemModel.
 * The different from FileInfo to FileItem is that FileItem has concept of
 * children and parent. This makes FileItem instance has a tree struction and
 * can represent a tree item in a view(non-tree as well). Other different is
 * that FileItem instance is not shared. You can hold many FileItem instances
 * crosponding to the same FileInfo, but they are allocated in their own memory
 * space. Every FileItem instance which has children will aslo support
 * monitoring. When find the children of the item, it will start a monitor for
 * this directory.
 * </br>
 * \note
 * Actually, every FileItem instance should bind with an model instance,
 * otherwise it will be useless.
 */
class PEONYCORESHARED_EXPORT FileItem : public QObject
{
    friend class FileItemProxyFilterSortModel;
    friend class FileItemModel;
    Q_OBJECT
public:
    explicit FileItem(std::shared_ptr<Peony::FileInfo> info,
                      FileItem *parentItem = nullptr,
                      FileItemModel *model = nullptr,
                      QObject *parent = nullptr);
    ~FileItem();

    const QString uri();
    const std::shared_ptr<FileInfo> info() {return m_info;}

    bool operator == (const FileItem &item);

    QVector<FileItem*> *findChildrenSync();
    void findChildrenAsync();

    /*!
     * \brief firstColumnIndex
     * \return first column index of item in model.
     * \see FileItemModel::firstColumnIndex().
     */
    QModelIndex firstColumnIndex();
    /*!
     * \brief lastColumnIndex
     * \return last column index of item in model.
     * \see FileItemModel::lastColumnIndex()
     */
    QModelIndex lastColumnIndex();

    bool hasChildren();

Q_SIGNALS:
    void cancelFindChildren();
    void childAdded(const QString &uri);
    void childRemoved(const QString &uri);
    void deleted(const QString &thisUri);
    void renamed(const QString &oldUri, const QString &newUri);

public Q_SLOTS:
    void onChildAdded(const QString &uri);
    void onChildRemoved(const QString &uri);
    void onDeleted(const QString &thisUri);
    void onRenamed(const QString &oldUri, const QString &newUri);

    void clearChildren();

protected:
    /*!
     * \brief getChildFromUri
     * \param uri
     * \return child item
     * \note
     * This is ususally used when fileCreated() and fileDeleted() happend,
     * and item must has parent item.
     */
    FileItem *getChildFromUri(QString uri);

    /*!
     * \brief updateInfoSync
     * <br>
     * Update the item info synchously.
     * </br>
     * \note
     * This is ususally used when fileCreated() and fileDeleted() happend,
     * and item must has parent item.
     */
    void updateInfoSync();
    /*!
     * \brief updateInfoAsync
     * <br>
     * Update the item info asynchously.
     * </br>
     * This is ususally used when fileCreated() and fileDeleted() happend,
     * and item must has parent item.
     */
    void updateInfoAsync();

private:
    FileItem *m_parent = nullptr;
    std::shared_ptr<Peony::FileInfo> m_info;
    QVector<FileItem*> *m_children = nullptr;

    FileItemModel *m_model = nullptr;

    bool m_expanded = false;

    FileWatcher *m_watcher = nullptr;

    /*!
     * \brief m_async_count
     * <br>
     * when enumerate children finished, we start a async job for update children info.
     * this count is record the current last un-updated children count.
     * while all job finished, the count will clear, and we can insert the rows to model.
     * </br>
     */
    int m_async_count = 0;
};

}

#endif // FILEITEM_H
