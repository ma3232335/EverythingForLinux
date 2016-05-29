#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <sudodialog.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), m_sourceModel()
{
    setupUi();
    setWindowIcon(QIcon(QPixmap("://windowIcon.png")));
    m_sourceModel = new CSqlQueryModel;
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(strDbName);
    if (connectDb())
    {
        loadSettings(1);
        initTable();
    }
}

MainWindow::~MainWindow()
{

}

void MainWindow::setupUi()
{
    resize(700, 432);
    centralWidget = new QWidget(this);

    verticalLayout = new QVBoxLayout(centralWidget);
    verticalLayout->setSpacing(3);
    verticalLayout->setContentsMargins(1, 1, 1, 1);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(3);

    keywordEdit = new QLineEdit(centralWidget);
    keywordEdit->setObjectName(QString::fromUtf8("keywordEdit"));

    horizontalLayout->addWidget(keywordEdit);

    verticalLayout->addLayout(horizontalLayout);

    tableView = new CTableView(centralWidget);

    verticalLayout->addWidget(tableView);

    setCentralWidget(centralWidget);

    menuBar = new QMenuBar(this);
    menuBar->setGeometry(QRect(0, 0, 700, 25));
    setMenuBar(menuBar);

    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);

    /* set title and texts */
    retranslateUi();

    /* connect slots by OBJECT name */
    QMetaObject::connectSlotsByName(this);
}

void MainWindow::retranslateUi()
{
    setWindowTitle(QApplication::translate("MainWindow", "Everything", 0, QApplication::UnicodeUTF8));
}

bool MainWindow::reConnectDb()
{
    if (!m_db.isOpen())
    {
        m_db.close();
    }

    connectDb();

    return true;
}

bool MainWindow::connectDb()
{
    if (!m_db.open())
    {
        QMessageBox::warning(0, "Connect Database Error", m_db.lastError().text());
        return false;
    }
    return true;
}

bool MainWindow::initTable()
{
    m_sourceModel->setQuery(strSelectSQL + strOrderByNm);
    m_sourceModel->setHeaderData(0, Qt::Horizontal, "Name");
    m_sourceModel->setHeaderData(1, Qt::Horizontal, "Path");
    m_sourceModel->setHeaderData(2, Qt::Horizontal, "Size");
    m_sourceModel->setHeaderData(3, Qt::Horizontal, "Data Modified");

    tableView->setModel(m_sourceModel);
    tableView->setColumnWidth(0, 150);
    tableView->setColumnWidth(1, 250);
    tableView->setColumnWidth(2, 70);
    tableView->setColumnWidth(3, 110);
    tableView->setStyleSheet("QTableView{border-style: none}"
                             "QTableView::item:selected{background: rgb(51,153,255)}");

    connect(tableView, SIGNAL(hoverRowChanged(int)), m_sourceModel, SLOT(setHoverRow(int)));
    return true;
}

bool MainWindow::loadSettings(bool loadDefault)
{
    if (loadDefault)
    {
        m_enableMatchCase = false;
        m_enableRegex = false;
    }
    else
    {
        //load settings from ini file
    }
}

void MainWindow::setStatus(const QString& text)
{
    statusBar->showMessage("1234");
}

void MainWindow::setTitle(const QString& text)
{
    if (text.isEmpty())
    {
        setWindowTitle("Everything");
    }
    else
    {
        setWindowTitle(text + " - Everything");
    }
}

void MainWindow::setFilter(const QString& text)
{
    //    m_sourceModel->setQuery(strSelectSQL+" WHERE name LIKE '%" + text + "%' ORDER BY name");
    //    m_proxyModel->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive));
    if (text.contains('*'))
    {
        if (m_enableMatchCase)
        {
            m_sourceModel->setQuery(strSelectSQL + strWhereGlob.arg(text) + strOrderByNm);
        }
        else
        {
            QString keyword = text;
            m_sourceModel->setQuery(strSelectSQL + strWhereLike.arg(keyword.replace('*', '%')) + strOrderByNm);
        }
    }
    else
    {
        if (m_enableMatchCase)
        {
            m_sourceModel->setQuery(strSelectSQL + strWhereGlobWildcard.arg(text) + strOrderByNm);
        }
        else
        {
            m_sourceModel->setQuery(strSelectSQL + strWhereLikeWildcard.arg(text) + strOrderByNm);
        }
    }
}

void MainWindow::on_keywordEdit_textChanged()
{
    setTitle(keywordEdit->text());
    setStatus(keywordEdit->text());
    setFilter(keywordEdit->text());
}

void MainWindow::reloadModel()
{
    qDebug() << "reloadModel";
    reConnectDb();
    loadSettings(1);
    initTable();

    m_sourceModel->setQuery(strSelectSQL + strOrderByNm);
}
