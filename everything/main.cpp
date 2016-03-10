#include "mainwindow.h"
#include <QApplication>
#include <updatedbsdialog.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    UpdateDbsDialog updateDbsDialog(&w);
    QObject::connect(&updateDbsDialog, SIGNAL(databaseUpdated()), &w, SLOT(reloadModel()));
    updateDbsDialog.exec();
    return a.exec();
}

