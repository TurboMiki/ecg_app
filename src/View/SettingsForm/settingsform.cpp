#include "settingsform.h"
#include "ui_settingsform.h"
//#include <QSettingsForm>
#include <QLayout>


QSettingsForm::QSettingsForm(QWidget *parent)
    : QPlainTextEdit(parent)
{
}

QSettingsForm::~QSettingsForm()
{
}

SettingsForm::SettingsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsForm)
{
    ui->setupUi(this);
    setWindowFlag(Qt::Popup);

    m_settingsform = nullptr;

    QStringList items;
    items << "1" << "2" << "3";
    ui->Param1->addItems(items);
    ui->Param2->addItems(items);
    ui->Param3->addItems(items);
    ui->Param4->addItems(items);
    ui->Param5->addItems(items);
    ui->Param6->addItems(items);

    auto signal1 = static_cast<void (QComboBox::*)(int)>(&QComboBox::activated);

    //layout()->setMargin(0);

    connect(ui->Param1, signal1, this, [&](int index){
        if (m_settingsform){
            m_settingsform->setPlainText(ui->Param1->itemText(index));
        }
    });

    connect(ui->Param2, signal1, this, [&](int index){
        if (m_settingsform){
            m_settingsform->setPlainText(ui->Param2->itemText(index));
        }
    });

    connect(ui->Param3, signal1, this, [&](int index){
        if (m_settingsform){
            m_settingsform->setPlainText(ui->Param3->itemText(index));
        }
    });

    connect(ui->Param4, signal1, this, [&](int index){
        if (m_settingsform){
            m_settingsform->setPlainText(ui->Param4->itemText(index));
        }
    });

    connect(ui->Param5, signal1, this, [&](int index){
        if (m_settingsform){
            m_settingsform->setPlainText(ui->Param5->itemText(index));
        }
    });

    connect(ui->Param6, signal1, this, [&](int index){
        if (m_settingsform){
            m_settingsform->setPlainText(ui->Param6->itemText(index));
        }
    });
}

SettingsForm::~SettingsForm()
{
    delete ui;
}

//void SettingsForm::on_OK_clicked()
//{
//    if (m_settingsform){

//    }
//}
