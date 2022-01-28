#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QTranslator>

#include "mainwindow.hpp"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QTranslator translator;
    translator.load(":/translations/trans.qm");
    a.installTranslator(&translator);
    MainWindow w;
    w.show();
    return a.exec();
}
