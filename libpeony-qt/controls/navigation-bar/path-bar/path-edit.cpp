#include "path-edit.h"
#include "path-bar-model.h"
#include "path-completer.h"

#include <QKeyEvent>
#include <QAction>

#include <QDebug>

using namespace Peony;

PathEdit::PathEdit(QWidget *parent) : QLineEdit(parent)
{
    setFocusPolicy(Qt::ClickFocus);

    m_model = new PathBarModel(this);
    m_completer = new PathCompleter(this);
    m_completer->setModel(m_model);

    setLayoutDirection(Qt::LeftToRight);

    QAction *goToAction = new QAction(QIcon::fromTheme("forward"), tr("Go To"), this);
    addAction(goToAction, QLineEdit::TrailingPosition);

    connect(goToAction, &QAction::triggered, this, &QLineEdit::returnPressed);

    setCompleter(m_completer);

    connect(this, &QLineEdit::returnPressed, [=]{
        if (this->text().isEmpty()) {
            this->setText(m_last_uri);
            return;
        } else {
            qDebug()<<"change dir request"<<this->text();
            Q_EMIT this->uriChangeRequest(this->text());
        }
    });
}

void PathEdit::setUri(const QString &uri)
{
    m_last_uri = uri;
    setText(uri);
}

void PathEdit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    Q_EMIT editCancelled();
}

void PathEdit::keyPressEvent(QKeyEvent *e)
{
    QLineEdit::keyPressEvent(e);
    if (e->key() == Qt::Key_Escape) {
        Q_EMIT editCancelled();
    }
}
