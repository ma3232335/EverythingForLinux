#ifndef CTABLEVIEW_H
#define CTABLEVIEW_H

#include <QTableView>

class CTableView : public QTableView
{
    Q_OBJECT
public:
    explicit CTableView(QWidget *parent = 0);

signals:
    void hoverRowChanged(int row);

public slots:
    void updateRow(const QModelIndex & index);

protected:
    void leaveEvent(QEvent *event);


private:
    void setupView();

private:
    int m_hoverRow;
};

#endif // CTABLEVIEW_H
