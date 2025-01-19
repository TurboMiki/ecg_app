#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <DataReader.h>
#include <Baseline.h>

#include "RPeaks.h"
#include "settingsform.h"
#include "basic_plot.h"

enum class PLOT_TYPE{
    RAW_PLOT
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
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
    void get_settings(const QStringList &data);
    void write_settings(const QStringList &data);
    void debug_settings();

    void on_btnPath_clicked();
    void on_btnRaw_clicked();
    void on_btnFECG_clicked();
    void on_checkBoxRP_stateChanged(int state);

    void on_pushButton_clicked();

    void createPlot(QLayout* layout,PLOT_TYPE plotType);

    void on_showTable_stateChanged(int state);
    void resizeLayout();

private:
    Ui::MainWindow *ui;
    SettingsForm *ptrSettingsForm;
    Basic_Plot *plotWidget;
    
    QString parameter1;
    QString parameter2;
    QString parameter3;
    QString parameter4;
    QString parameter5;
    QString filePath;

    DataReader fileReader;
    PLOT_TYPE cuurentPlot = PLOT_TYPE::RAW_PLOT;
    Baseline baseline;
    RPeaks rPeaks;
    QList<int> r_peak_positions;
    QWidget *tableWidget;
    QWidget *currentPlotWidget;

};

#endif // MAINWINDOW_H
