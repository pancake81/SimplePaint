#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    try
    {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        return a.exec();
    } catch (std::bad_alloc&)
    {
        QMessageBox msg;
        msg.setText("Not enough memory!");
        msg.setWindowTitle("Critical error");
        msg.setIcon(QMessageBox::Critical);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.open();
    }

    return 0;
}
