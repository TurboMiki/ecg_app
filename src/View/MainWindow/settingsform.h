#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class SettingsForm;
}

class SettingsForm : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsForm(QWidget *parent = nullptr);
    ~SettingsForm();

signals:
    void pass_values(const QStringList &data);

private slots:
    void on_btnSave_clicked();
    void on_btnCancel_clicked();

private:
    Ui::SettingsForm *ui;
};

#endif // SETTINGSFORM_H
