#include "csqlquerymodel.h"
#include <QDebug>
#include <QBrush>
#include <QColor>

CSqlQueryModel::CSqlQueryModel(QObject *parent) :
    QSqlQueryModel(parent)
{
    m_hoverRow = -1;
}

void CSqlQueryModel::setHoverRow(int row)
{
    m_hoverRow = row;
}


QVariant CSqlQueryModel::data ( const QModelIndex & item, int role) const
{
    /* Set background color of hover row  RGBA(51,153,255,100)*/
    if (item.row() == m_hoverRow && Qt::BackgroundRole == role)
    {
        return QBrush(QColor(51, 153, 255, 100));
    }


    /* Set 'size' column alignment right|vcenter, other  left|vcenter*/
    if (item.column() == COLUMN_SIZE && role == Qt::TextAlignmentRole)
    {
        return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    }


    if (item.column() == COLUMN_MTIME && role == Qt::DisplayRole)
        return 1;


    return QSqlQueryModel::data(item, role);
}

void CSqlQueryModel::setQuery ( const QString & query, const QSqlDatabase & db)
{
    qDebug() << query;
    QSqlQueryModel::setQuery(query, db);
    insertColumn(3);
}
