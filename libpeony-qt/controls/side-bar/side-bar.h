#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QTreeView>
#include "peony-core_global.h"

namespace Peony {

class SideBarDelegate;

class PEONYCORESHARED_EXPORT SideBar : public QTreeView
{
    friend class SideBarDelegate;
    Q_OBJECT
public:
    explicit SideBar(QWidget *parent = nullptr);

Q_SIGNALS:
    void updateWindowLocationRequest(const QString &uri, bool addHistory = true);

protected:
    void paintEvent(QPaintEvent *e) override;
    QRect visualRect(const QModelIndex &index) const override;
    //int horizontalOffset() const override {return 100;}
};

}

#endif // SIDEBAR_H
