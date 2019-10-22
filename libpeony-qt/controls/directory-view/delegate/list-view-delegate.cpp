#include "list-view-delegate.h"
#include "file-operation-utils.h"
#include "file-item-model.h"
#include <QTextEdit>

#include <QTimer>
#include <QPushButton>

#include <QPainter>

using namespace Peony;

ListViewDelegate::ListViewDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    m_styled_button = new QPushButton;
}

ListViewDelegate::~ListViewDelegate()
{
    m_styled_button->deleteLater();
}

void ListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.displayAlignment = Qt::Alignment(Qt::AlignLeft|Qt::AlignVCenter);

    auto widget = qobject_cast<QWidget*>(parent());
    auto style = widget->style();
    painter->save();
    if (opt.state.testFlag(QStyle::State_MouseOver)) {
        if (index.column() == 0) {
            if (!opt.state.testFlag(QStyle::State_Selected)) {
                auto rect = opt.rect;
                rect.setX(0);
                rect.setRight(9999);
                QColor color = m_styled_button->palette().highlight().color();
                color.setAlpha(127);//half transparent
                painter->fillRect(rect, color);
            } else {
                auto rect = opt.rect;
                rect.setX(0);
                rect.setRight(9999);
                QColor color = m_styled_button->palette().highlight().color();
                painter->fillRect(rect, color);
            }
        }
    }
    painter->restore();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

QWidget *ListViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QTextEdit *edit = new QTextEdit(parent);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setWordWrapMode(QTextOption::NoWrap);

    QTimer::singleShot(1, [=](){
        this->updateEditorGeometry(edit, option, index);
    });

    connect(edit, &QTextEdit::textChanged, [=](){
        updateEditorGeometry(edit, option, index);
    });

    return edit;
}

void ListViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QTextEdit *edit = qobject_cast<QTextEdit *>(editor);
    if (!edit)
        return;

    edit->setText(index.data(Qt::DisplayRole).toString());
    auto cursor = edit->textCursor();
    cursor.setPosition(0, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    //qDebug()<<cursor.position();
    if (edit->toPlainText().contains(".") && !edit->toPlainText().startsWith(".")) {
        cursor.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor, 2);
        //qDebug()<<cursor.position();
    }
    //qDebug()<<cursor.anchor();
    edit->setTextCursor(cursor);
}

void ListViewDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
    QTextEdit *edit = qobject_cast<QTextEdit*>(editor);
    edit->setFixedHeight(editor->height());
    edit->resize(edit->document()->size().width(), -1);
}

void ListViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QTextEdit *edit = qobject_cast<QTextEdit*>(editor);

    auto text = edit->toPlainText();
    if (text.isEmpty())
        return;
    if (text == index.data(Qt::DisplayRole).toString())
        return;

    FileOperationUtils::rename(index.data(FileItemModel::UriRole).toString(), text, true);
}
