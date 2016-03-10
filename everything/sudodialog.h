#ifndef SUDODIALOG_H
#define SUDODIALOG_H

#include <QDialog>
#include "ui_sudodialog.h"

class SudoDialog : public QDialog, public Ui_SudoDialog
{
    Q_OBJECT
    
public:
    explicit SudoDialog(QWidget *parent = 0);
    ~SudoDialog();
    QString getPassward();

    static QString getValidPassword(QWidget *parent);

private:
    void setPassward(const QString &text);
    bool attemptLogin();

private slots:
    void on_cancelBtn_clicked();
    void on_okBtn_clicked();
    void on_pwdLineEdit_textChanged(const QString &text);

    
private:
    QString m_passward;
    int m_attemptTimes;

};

#endif // SUDODIALOG_H
