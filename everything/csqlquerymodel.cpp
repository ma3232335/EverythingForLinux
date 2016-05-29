#include "csqlquerymodel.h"
#include <QDebug>
#include <QBrush>
#include <QColor>
#include <sys/types.h>
#include <sys/stat.h>

#include <QByteArray>
#include <QSqlRecord>

CSqlQueryModel::CSqlQueryModel(QObject *parent) :
    QSqlQueryModel(parent)
{
    m_hoverRow = -1;
}

void CSqlQueryModel::setHoverRow(int row)
{
    m_hoverRow = row;
}

QVariant CSqlQueryModel::data (const QModelIndex & item, int role) const
{
    /* Set background color of hover row  RGBA(51,153,255,100)*/
    if (item.row() == m_hoverRow && Qt::BackgroundRole == role)
    {
        return QBrush(QColor(51, 153, 255, 100));
    }

    static time_t s_mtime;

    /* Set 'size' column alignment right|vcenter, other  left|vcenter*/
    if (item.column() == COLUMN_SIZE)
    {
        if (role == Qt::TextAlignmentRole)
        {
            return QVariant(Qt::AlignRight | Qt::AlignVCenter);
        }
        else if (role == Qt::DisplayRole)
        {
            struct stat fileStat;
            const QByteArray baFilePath = (record(item.row()).value("path").toString() + "/" + record(item.row()).value("name").toString()).toLatin1();
            stat(baFilePath.data(), &fileStat);
            s_mtime = fileStat.st_mtime;

            return sizeValue(fileStat.st_size);
        }
    }

    if (item.column() == COLUMN_MTIME && role == Qt::DisplayRole)
    {
        return timeValue(s_mtime);
    }


    return QSqlQueryModel::data(item, role);
}

QVariant CSqlQueryModel::timeValue(const time_t timep) const
{
    struct tm *stTime = NULL;
    stTime = localtime(&timep);
    return QString("%1/%2/%3 %4:%5")
        .arg(stTime->tm_year+1900)
        .arg(stTime->tm_mon+1)
        .arg(stTime->tm_mday)
        .arg(stTime->tm_hour)
        .arg(stTime->tm_min);
}

QVariant CSqlQueryModel::sizeValue(const size_t size) const
{
    return QString("%1 KB").arg(size/1024.0, 0, 'f', 2);
}

void CSqlQueryModel::setQuery ( const QString & query, const QSqlDatabase & db)
{
    qDebug() << query;
    QSqlQueryModel::setQuery(query, db);
    insertColumn(3);
}
