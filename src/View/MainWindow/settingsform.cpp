#include "settingsform.h"
#include "ui_settingsform.h"
#include "baselineform.h"
#include "rpeaksform.h"

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
    /*
    QString parameter1 = ui->Param1->text();
    QString parameter2 = ui->Param2->text();
    QString parameter3 = ui->Param3->text();
    QString parameter4 = ui->Param4->text();
    QString parameter5 = ui->Param5->text();

    QStringList data;
    data << parameter1 << parameter2 << parameter3 << parameter4 << parameter5;

    emit pass_values(data); // Emitowanie sygnału z danymi
    this->close();
    */
}

void SettingsForm::on_btnCancel_clicked()
{
    this->close(); // Zamknięcie okna ustawień
}

void SettingsForm::on_ModuleComboBox_activated(int index)
{
    QLayout* layout = ui->toDisplay->layout();
    if (!layout) {
        layout = new QVBoxLayout(ui->toDisplay);
        ui->toDisplay->setLayout(layout);
    }

    // Clear any existing widgets in the layout
    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // Dummy (Blank Page) -> 0
    // Baseline -> 1
    // RPeaks -> 2

    if(index==1) {
        // Create and add form
        BaselineForm* baselineWidget = new BaselineForm();
        layout->addWidget(baselineWidget);
    }

    if(index==2) {
        // Create and add form
        RPeaksForm* rpeaksWidget = new RPeaksForm();
        layout->addWidget(rpeaksWidget);
    }

}

