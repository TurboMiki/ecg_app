#include "baselineform.h"
#include "ui_baselineform.h"
#include <QFileDialog>

BaselineForm::BaselineForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BaselineForm)
{
    ui->setupUi(this);
}

BaselineForm::~BaselineForm()
{
    delete ui;
}

void BaselineForm::on_LMS_RF_ToolButton_clicked()
{
    QString fileFilter = "All Supported Files (*.dat *.csv *.xlsx);;Data Files (*.dat);;CSV Files (*.csv);;Excel Files (*.xlsx)";
    QString selectedFile = QFileDialog::getOpenFileName(this, "Choose file", QDir::homePath(), fileFilter);
    if (!selectedFile.isEmpty())
    {
        this->filePath = selectedFile;
        ui->LMS_RF_FilePath->setText(this->filePath);
    }
    fileReader.set_path(ui->LMS_RF_FilePath->text().toStdString());
    fileReader.read_file();
    fileReader.write_measured_time();
}
/*
Moving Mean:
    MM_WL_SpinBox

ButterWorth:
    Btw_FO_SpinBox
    Btw_UF_SpinBox
    Btw_LF_SpinBox

Savitzky-Golay
    SG_WL_SpinBox
    SG_FO_SpinBox

LMS
    LMS_RF_ToolButton
    LMS_RF_FilePath

*/
