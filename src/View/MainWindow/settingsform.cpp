#include "settingsform.h"
#include "ui_settingsform.h"

SettingsForm::SettingsForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsForm)
{
    ui->setupUi(this);
}

SettingsForm::~SettingsForm()
{
    delete ui;
}

void SettingsForm::on_btnSave_clicked()
{
    QString parameter1 = ui->Param1->text();
    QString parameter2 = ui->Param2->text();
    QString parameter3 = ui->Param3->text();
    QString parameter4 = ui->Param4->text();
    QString parameter5 = ui->Param5->text();

    QStringList data;
    data << parameter1 << parameter2 << parameter3 << parameter4 << parameter5;

    emit pass_values(data); // Emitowanie sygnału z danymi
    this->close();
}

void SettingsForm::on_btnCancel_clicked()
{
    this->close(); // Zamknięcie okna ustawień
}
