#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressBar_1->setMaximum(200);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_clickButton_1_clicked()
{
    for (int i=0; i<=ui->progressBar_1->maximum(); i++){
        QThread::msleep(20);
        ui->progressBar_1->setValue(i);
        qApp->processEvents(QEventLoop::AllEvents);
    }
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Message");
    msgBox.setText("Loading concluded. Please get to work!");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setStyleSheet("QMessageBox { border: 2px solid black; }"); // Add a border
    msgBox.exec();
}

