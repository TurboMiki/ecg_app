#ifndef BASELINEFORM_H
#define BASELINEFORM_H

#include <QDialog>
#include "DataReader.h"

namespace Ui {
class BaselineForm;
}

class BaselineForm : public QDialog
{
    Q_OBJECT

public:
    explicit BaselineForm(QWidget *parent = nullptr);
    ~BaselineForm();

    QString getCurrentFilter() const;
    QMap<QString, double> getCurrentParameters() const;

private slots:
    void on_LMS_RF_ToolButton_clicked();

private:
    Ui::BaselineForm *ui;
    DataReader fileReader;
    QString filePath;
};

#endif // BASELINEFORM_H
