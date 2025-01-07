#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>

#include <QPlainTextEdit>


namespace Ui {
class QSettingsForm;
class SettingsForm;
}

class QSettingsForm : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit QSettingsForm(QWidget *parent = nullptr);
    ~QSettingsForm();
};

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
