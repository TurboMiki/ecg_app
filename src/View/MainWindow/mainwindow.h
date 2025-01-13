#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include "settingsform.h"
#include <DataReader.h>
#include "basic_plot.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void requestData(const QStringList &data);

private slots:
    void on_START_clicked();
    void on_Config_clicked();
    void get_settings(const QStringList &data); // get parameters from settings form
    void write_settings(const QStringList &data); // writes private data
    void debug_settings(); // only for debug use

    void on_btnPath_clicked();

    void on_btnRaw_clicked();

private:
    Ui::MainWindow *ui;
    SettingsForm *ptrSettingsForm;
    // Basic_Plot *plotWidget;
    QString parameter1;
    QString parameter2;
    QString parameter3;
    QString parameter4;
    QString parameter5;
    QString filePath;
    DataReader fileReader = DataReader();
    // Basic_Plot rawPlot = Basic_Plot();
};

#endif // MAINWINDOW_H
