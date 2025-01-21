#ifndef RPEAKSFORM_H
#define RPEAKSFORM_H

#include <QDialog>

namespace Ui {
class RPeaksForm;
}

class RPeaksForm : public QDialog
{
    Q_OBJECT

public:
    explicit RPeaksForm(QWidget *parent = nullptr);
    ~RPeaksForm();

    QString getCurrentMethod() const;
    QMap<QString, double> getCurrentParameters() const;

private:
    Ui::RPeaksForm *ui;
};

#endif // RPEAKSFORM_H
