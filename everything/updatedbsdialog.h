#ifndef UPDATEDBSDIALOG_H
#define UPDATEDBSDIALOG_H

#include <QDialog>
#include <QProcess>
#include <ui_updatedbsdialog.h>
#include "taskmanager.h"

class UpdateDbsDialog : public QDialog, public Ui_UpdateDbsDialog
{
    Q_OBJECT
    
public:
    explicit UpdateDbsDialog(QWidget *parent = 0);
    ~UpdateDbsDialog();

signals:
    void databaseUpdated();
    
private slots:
    void on_cancelBtn_clicked();
    void on_unlockBtn_clicked();
    void onProcFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void taskFinish(int taskid, bool success);

private:


private:
    bool m_passwardValid;
    TaskManager* m_pTaskMgr;
};

#endif // UPDATEDBSDIALOG_H
