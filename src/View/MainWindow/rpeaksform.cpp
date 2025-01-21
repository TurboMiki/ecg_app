#include "rpeaksform.h"
#include "ui_rpeaksform.h"

RPeaksForm::RPeaksForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RPeaksForm)
{
    ui->setupUi(this);
}

RPeaksForm::~RPeaksForm()
{
    delete ui;
}

QString RPeaksForm::getCurrentMethod() const {
    return ui->AlgForm->currentWidget()->objectName();
}

QMap<QString, double> RPeaksForm::getCurrentParameters() const {
    QMap<QString, double> params;
    QString currentMethod = getCurrentMethod();
    
    if (currentMethod == "PT") {
        params["Window Length"] = ui->PT_WL_SpinBox->value();
        params["Threshold"] = ui->PT_T_SpinBox->value();
    }
    else if (currentMethod == "Hilbert") {
        params["Proximity"] = ui->H_P_SpinBox->value();
        params["Threshold"] = ui->H_T_SpinBox->value();
    }
    
    return params;
}

/*
Pan-Tompkins
    PT_WL_SpinBox
    PT_T_SpinBox

Hilbert
    H_P_SpinBox
    H_T_SpinBox
*/