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

/*
Pan-Tompkins
    PT_WL_SpinBox
    PT_T_SpinBox

Hilbert
    H_P_SpinBox
    H_T_SpinBox
*/