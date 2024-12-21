#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Icon
    ui->START->setIcon(QIcon("/Users/mateu/OneDrive/Desktop/play.png"));
    ui->Config->setIcon(QIcon("/Users/mateu/OneDrive/Desktop/gear.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->START->setIcon(QIcon("/Users/mateu/OneDrive/Desktop/play_push.png"));
}


void MainWindow::on_pushButton_pressed()
{
    ui->START->setIcon(QIcon("/Users/mateu/OneDrive/Desktop/play_push.png"));
}

void MainWindow::on_START_clicked(bool)
{

}
void MainWindow::on_START_clicked()
{

}

void MainWindow::on_Config_clicked()
{

}

void MainWindow::on_pushButton_6_clicked()
{

}

