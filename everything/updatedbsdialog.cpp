#include "updatedbsdialog.h"
#include "ui_updatedbsdialog.h"
#include "sudodialog.h"
#include <QDebug>
#include <QProcess>
#include <QThread>

UpdateDbsDialog::UpdateDbsDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint), m_passwardValid(false), m_pTaskMgr(NULL)
{
    setupUi(this);
    updateStat->hide();
    locateDbRes->hide();
    searchDbRes->hide();
}

UpdateDbsDialog::~UpdateDbsDialog()
{
    if (m_pTaskMgr != NULL)
    {
        delete m_pTaskMgr;
    }
}

void UpdateDbsDialog::on_cancelBtn_clicked()
{
    close();
}


void UpdateDbsDialog::on_unlockBtn_clicked()
{
    const QString password = SudoDialog::getValidPassword(this);
    if (password.isEmpty())
    {
        updateStat->setText("Authentication failed.");
        updateStat->show();
        return;
    }

    updateStat->setText("Updating database...");
    updateStat->show();
    cancelBtn->setDisabled(true);
    unlockBtn->setDisabled(true);

    m_pTaskMgr = TaskManager::getTaskManager(this);
    QObject::connect(m_pTaskMgr, SIGNAL(taskFinished(int,bool)), this, SLOT(taskFinish(int,bool)));
    m_pTaskMgr->setPassward(password);
    m_pTaskMgr->start();
}

void UpdateDbsDialog::onProcFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "on finish: " << exitCode << exitStatus;
}

void UpdateDbsDialog::taskFinish(int taskid, bool success)
{
    switch(taskid)
    {
    case TASKID_UPDATE_LOCATEDB:
        if (success)
        {
            locateDbRes->setText("Locate database updated successfully.");
        }
        else
        {
            locateDbRes->setText("Locate database updated failed.");
        }
        locateDbRes->show();
        break;

    case TASKID_UPDATE_SEARCHDB:
        if (success)
        {
            searchDbRes->setText("Search database updated successfully.");
        }
        else
        {
            searchDbRes->setText("Search database updated failed.");
        }
        searchDbRes->show();
        unlockBtn->hide();
        cancelBtn->setText("close");
        cancelBtn->setEnabled(true);
        updateStat->setDisabled(true);
        emit databaseUpdated();
        break;

    default:
        break;
    }
}
