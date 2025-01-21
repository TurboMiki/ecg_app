#include "settingsform.h"
#include "ui_settingsform.h"
#include "baselineform.h"
#include "rpeaksform.h"

SettingsForm::SettingsForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsForm)
{
    ui->setupUi(this);
    
    // Display Baseline form by default when settings form is opened
    QLayout* layout = ui->toDisplay->layout();
    if (!layout) {
        layout = new QVBoxLayout(ui->toDisplay);
        ui->toDisplay->setLayout(layout);
    }
    
    // Create and add Baseline form as default
    BaselineForm* baselineWidget = new BaselineForm();
    layout->addWidget(baselineWidget);
    
    // Set combobox to Baseline
    ui->ModuleComboBox->setCurrentIndex(1);
}

SettingsForm::~SettingsForm()
{
    delete ui;
}

void SettingsForm::on_btnSave_clicked()
{
    this->close();
}

void SettingsForm::on_btnCancel_clicked()
{
    this->close();
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

    // Handle only Baseline and RPeaks options
    if(index == 0) { // Baseline
        BaselineForm* baselineWidget = new BaselineForm();
        layout->addWidget(baselineWidget);
    }
    else if(index == 1) { // RPeaks
        RPeaksForm* rpeaksWidget = new RPeaksForm();
        layout->addWidget(rpeaksWidget);
    }
}

/*
SettingsForm
    btnSave
    btnCancel
    ModuleComboBox
*/