#include "src/View/MainWindow/mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow M;
    M.show();
    return app.exec();
}