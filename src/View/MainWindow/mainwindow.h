#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QMenu;
class SettingsForm;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_pressed();

    void on_START_clicked(bool checked);

    void on_START_clicked();

    void on_pushButton_6_clicked();

    void on_Config_clicked();

private:
    Ui::MainWindow *ui;

    QMenu *m_settingsedit_menu;
    SettingsForm *m_settingsedit_popup_form;
};
#endif // MAINWINDOW_H
