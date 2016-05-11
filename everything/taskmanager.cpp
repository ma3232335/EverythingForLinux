#include "taskmanager.h"
#include <QDebug>

TaskManager* TaskManager::m_taskManager;

TaskManager::TaskManager(QObject *parent) :
    QThread(parent), m_pProc(NULL)
{
}

TaskManager::~TaskManager()
{
    if (m_pProc != NULL)
    {
        delete m_pProc;
    }
}

bool TaskManager::sudoRunProcess(const QString &program, const QStringList &arguments)
{
    qDebug() << "sudo run process: " << m_passward << program << arguments;
    QStringList args;
    args << "-S" << program << arguments;
    m_pProc->start("sudo", args);
    m_pProc->write(m_passward.toUtf8() + "\n");
    if (!m_pProc->waitForFinished(1000 * 60))
    {
        qDebug() << m_pProc->errorString();
        m_pProc->terminate();
        m_pProc->waitForFinished(1000 * 5);
        return false;
    }

    if (m_pProc->exitCode() != 0)
    {
        return false;
    }

    return true;
}

void TaskManager::run()
{
    qDebug() << "taskmanager is running";

    if (!m_passward.isEmpty())
    {
        m_pProc = new QProcess();

        /* task No1 : run updatedb */
        emit taskFinished(TASKID_UPDATE_LOCATEDB, sudoRunProcess("updatedb"));

        /* task No2 : run everything-updatedb */
        emit taskFinished(TASKID_UPDATE_SEARCHDB, sudoRunProcess("./everything-updatedb"));

    }
    exit();
}
void TaskManager::setPassward(const QString &passward)
{
    m_passward = passward;
}

TaskManager* TaskManager::getTaskManager(QObject *parent)
{
    if (m_taskManager == NULL)
    {
        m_taskManager = new TaskManager(parent);
    }

    return m_taskManager;
}
