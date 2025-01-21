#include "settingsform.h"
#include "ui_settingsform.h"
#include "baselineform.h"
#include "rpeaksform.h"
#include <QMessageBox>

SettingsForm::SettingsForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsForm)
{
    ui->setupUi(this);
    
    // Setup layout if it doesn't exist
    QLayout* layout = ui->toDisplay->layout();
    if (!layout) {
        layout = new QVBoxLayout(ui->toDisplay);
        ui->toDisplay->setLayout(layout);
    }
    
    // Create both forms
    BaselineForm* baselineWidget = new BaselineForm(this);
    RPeaksForm* rpeaksWidget = new RPeaksForm(this);
    
    // Store pointers to forms
    ptrBaselineForm = baselineWidget;
    ptrRPeaksForm = rpeaksWidget;
    
    // Add initial widget (Baseline)
    layout->addWidget(baselineWidget);
    layout->addWidget(rpeaksWidget);
    rpeaksWidget->hide();  // Hide RPeaks initially
    
    // Set initial selection
    ui->ModuleComboBox->setCurrentIndex(0);  // Set to Baseline
}
SettingsForm::~SettingsForm()
{
    delete ui;
}

void SettingsForm::on_btnSave_clicked()
{
    // Get parameters from both forms
    QString debugMsg = "Current Settings:\n\n";
    
    // Baseline parameters
    QString baselineMethod = ptrBaselineForm->getCurrentFilter();
    QMap<QString, double> baselineParams = ptrBaselineForm->getCurrentParameters();
    
    debugMsg += "BASELINE\n";
    debugMsg += QString("Method: %1\n").arg(baselineMethod);
    debugMsg += "Parameters:\n";
    for (auto it = baselineParams.constBegin(); it != baselineParams.constEnd(); ++it) {
        debugMsg += QString("  %1: %2\n").arg(it.key()).arg(it.value());
    }
    
    // RPeaks parameters
    QString rpeaksMethod = ptrRPeaksForm->getCurrentMethod();
    QMap<QString, double> rpeaksParams = ptrRPeaksForm->getCurrentParameters();
    
    debugMsg += "\nRPEAKS\n";
    debugMsg += QString("Method: %1\n").arg(rpeaksMethod);
    debugMsg += "Parameters:\n";
    for (auto it = rpeaksParams.constBegin(); it != rpeaksParams.constEnd(); ++it) {
        debugMsg += QString("  %1: %2\n").arg(it.key()).arg(it.value());
    }

    // Show debug message with all parameters
    QMessageBox::information(this, "Selected Parameters", debugMsg);

    // Emit signal with both sets of parameters
    emit settingsChanged("Baseline", baselineMethod, baselineParams);
    emit settingsChanged("RPeaks", rpeaksMethod, rpeaksParams);

    this->close();
}

void SettingsForm::on_btnCancel_clicked()
{
    this->close();
}

void SettingsForm::on_ModuleComboBox_activated(int index)
{
    // Show/hide appropriate widget based on selection
    if (index == 0) { // Baseline
        ptrBaselineForm->show();
        ptrRPeaksForm->hide();
    }
    else if (index == 1) { // RPeaks
        ptrBaselineForm->hide();
        ptrRPeaksForm->show();
    }
}

/*
SettingsForm
    btnSave
    btnCancel
    ModuleComboBox
*/