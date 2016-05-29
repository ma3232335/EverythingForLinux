#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>
#include <QSqlQueryModel>
#include "csqlquerymodel.h"
#include "ctableview.h"

#include <QMessageBox>
#include <QSqlError>

const QString strSelectSQL = "SELECT name, path, type FROM everything";
const QString strWhereGlobWildcard = " WHERE name GLOB '*%1*'"; /* match case */
const QString strWhereLikeWildcard = " WHERE name LIKE '%%1%'";
const QString strWhereGlob = " WHERE name GLOB '%1'"; /* match case */
const QString strWhereLike = " WHERE name LIKE '%1'";
const QString strOrderByNm = " ORDER BY name";
const QString strDbName = "/var/lib/everything/everything.db";

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

public slots:
    void on_keywordEdit_textChanged();
    void reloadModel();

private:
    void setupUi();
    void retranslateUi();
    bool initTable();
    bool connectDb();
    bool reConnectDb();
    bool loadSettings(bool loadDefault = false);
    void setFilter(const QString& text);
    void setTitle(const QString& text);
    void setStatus(const QString& text);

public:
    CSqlQueryModel *m_sourceModel;
    QSqlDatabase m_db;

    /* settings */
    bool m_enableMatchCase;
    bool m_enableRegex;

    /* GUI widgets */
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLineEdit *keywordEdit;
    CTableView *tableView;
    QMenuBar *menuBar;
    QStatusBar *statusBar;

private:
};

#endif // MAINWINDOW_H

