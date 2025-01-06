#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>

class QSettingsForm;

namespace Ui {
class SettingsForm;
}

class SettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsForm(QWidget *parent = nullptr);
    ~SettingsForm();

    QSettingsForm *settingsform() const
        {
        return m_settingsform;
        }

private:
    Ui::SettingsForm *ui;
    QSettingsForm *m_settingsform;
};

#endif // SETTINGSFORM_H
