#include "sudodialog.h"
#include "ui_sudodialog.h"
#include <QProcess>

SudoDialog::SudoDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint), m_attemptTimes(0)
{
    setupUi(this);
    incorrectLabel->hide();
    okBtn->setEnabled(false);
}

SudoDialog::~SudoDialog()
{

}

QString SudoDialog::getPassward()
{
    return m_passward;
}

void SudoDialog::setPassward(const QString &text)
{
    if (!text.isEmpty())
    {
        m_passward = text;
    }
}

#include <QDebug>
QString SudoDialog::getValidPassword(QWidget *parent)
{
    SudoDialog dlg(parent);
    if (dlg.exec() == QDialog::Accepted)
    {
        return dlg.getPassward();
    }
    return QString();
}


bool SudoDialog::attemptLogin()
{
    QProcess proc;
    proc.start("sudo -k");
    proc.waitForFinished(1000);

    QString commd = "sudo";
    QStringList arguments;
    arguments << "-S" << "true";
    proc.start(commd, arguments);
    QByteArray passward = pwdLineEdit->text().toUtf8();
    proc.write(passward + "\n");
    if (!proc.waitForFinished(1000))
    {
        qDebug() << proc.errorString();
        proc.kill();
        proc.waitForFinished(1000);
        return false;
    }
    return true;
}

void SudoDialog::on_cancelBtn_clicked()
{
    reject();
}

void SudoDialog::on_okBtn_clicked()
{
    if (attemptLogin())
    {
        setPassward(pwdLineEdit->text());
        accept();
    }
    else
    {
        if (++m_attemptTimes >= 3)
        {
            reject();
        }
        incorrectLabel->show();
        pwdLineEdit->clear();
    }
}

void SudoDialog::on_pwdLineEdit_textChanged(const QString &text)
{
    if (!text.isEmpty())
    {
        okBtn->setEnabled(true);
    }
    else
    {
        okBtn->setEnabled(false);
    }
}
