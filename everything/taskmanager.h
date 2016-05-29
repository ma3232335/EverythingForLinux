#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QThread>
#include <QProcess>

enum 
{
    TASKID_UPDATE_LOCATEDB = 1,
    TASKID_UPDATE_SEARCHDB,
};

class TaskManager : public QThread
{
    Q_OBJECT
public:
    ~TaskManager();
    void run();
    void setPassward(const QString &passward);

    static TaskManager *getTaskManager(QObject *parent = 0);
    static TaskManager *m_taskManager;

private:
    explicit TaskManager(QObject *parent = 0);
    bool sudoRunProcess(const QString &program, const QStringList &arguments = QStringList());

signals:
    void taskFinished(int taskid, bool success);

public slots:

private:
    QProcess* m_pProc;
    QString m_passward;
};



#endif // TASKMANAGER_H
