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
