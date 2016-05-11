#include "ctableview.h"
#include "csqlquerymodel.h"
#include <QDebug>
#include <QMouseEvent>
#include <QHeaderView>
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QPen>
#include <QPainter>

class CLineDelegate : public QStyledItemDelegate
{
public:
    CLineDelegate(QObject* parent = 0);
protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
private:
    QPen pen;
};

CLineDelegate::CLineDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
    QColor gridColor(0,0,0,50);
    pen = QPen(gridColor, 0, Qt::SolidLine);
}
void CLineDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,const QModelIndex& index)const
{
    QStyledItemDelegate::paint(painter, option, index);
    QPen oldPen = painter->pen();
    painter->setPen(pen);

    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
    painter->setPen(oldPen);
}

CTableView::CTableView(QWidget *parent) :
    QTableView(parent)
{
    m_hoverRow = -1;
    setupView();

    setMouseTracking(true);
    connect(this, SIGNAL(entered(const QModelIndex & )), this, SLOT(updateRow(const QModelIndex & )));
}

void CTableView::setupView()
{
    setFocusPolicy(Qt::NoFocus);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setTextElideMode(Qt::ElideMiddle);
    setWordWrap(false);
    setShowGrid(false);
    setItemDelegate(new CLineDelegate);



    verticalHeader()->setHidden(true);


    horizontalHeader()->setClickable(false);
}


void CTableView::updateRow(const QModelIndex &item)
{
    int row = item.row();
    if (row == m_hoverRow)
        return;

    emit hoverRowChanged(row);
    int columnCount = model()->columnCount();
    for (int i = columnCount - 1; i >= 0; i--)
    {
        update(model()->index(m_hoverRow, i));
        update(model()->index(row, i));
    }
    m_hoverRow = row;
}

void CTableView::leaveEvent(QEvent *event)
{
    emit hoverRowChanged(-1);
    if (model() != NULL)
    {
        int columnCount = model()->columnCount();
        for (int i = columnCount - 1; i >= 0; i--)
        {
            update(model()->index(m_hoverRow, i));
        }
    }
    m_hoverRow = -1;
}
