#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsform.h"
#include <QMenu>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Icon
    ui->START->setIcon(QIcon("/home/miki/vcs_projects/dadm_app/ecg_app/src/View/MainWindow/icons/play.png"));
    ui->Config->setIcon(QIcon("/home/miki/vcs_projects/dadm_app/ecg_app/src/View/MainWindow/icons/gear.png"));

    //Forms
    ui->Config->setContextMenuPolicy(Qt::CustomContextMenu);
    m_settingsedit_menu = new QMenu(this);
    m_settingsedit_popup_form = new SettingsForm(this);
    QVBoxLayout *config_layout = new QVBoxLayout;
    config_layout->addWidget(m_settingsedit_popup_form);
    m_settingsedit_menu->setLayout(config_layout);
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
    ui->Config->setContextMenuPolicy(Qt::CustomContextMenu);
    m_settingsedit_menu = new QMenu(this);
    m_settingsedit_popup_form = new SettingsForm(this);
    QVBoxLayout *config_layout = new QVBoxLayout;
    config_layout->addWidget(m_settingsedit_popup_form);
    m_settingsedit_menu->setLayout(config_layout);
}

void MainWindow::on_pushButton_6_clicked()
{

}

