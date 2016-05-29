#ifndef CSQLQUERYMODEL_H
#define CSQLQUERYMODEL_H

#include <QtSql/QSqlQueryModel>
#include <QFileIconProvider>

enum {
    COLUMN_NAME,
    COLUMN_PATH,
    COLUMN_SIZE,
    COLUMN_MTIME,
    COLUMN_NUM
};

class CSqlQueryModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit CSqlQueryModel(QObject *parent = 0);
    QVariant data ( const QModelIndex & item, int role = Qt::DisplayRole ) const ;
    void setQuery ( const QString & query, const QSqlDatabase & db = QSqlDatabase());

private:
    QVariant sizeValue(const size_t size) const;
    QVariant timeValue(const time_t timep) const;

signals:

public slots:
    void setHoverRow(int row);

private:
    int m_hoverRow;
    QFileIconProvider m_iconProvider;
};

#endif // CSQLQUERYMODEL_H
