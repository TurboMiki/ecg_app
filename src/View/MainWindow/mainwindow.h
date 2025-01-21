#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <DataReader.h>
#include <Baseline.h>

#include "RPeaks.h"
#include "Waves.h"
#include "HRV_1.h"
#include "HRV_2.h"
#include "HRV_DFA.h"
#include "HeartClass.h"

#include "settingsform.h"

#include "basic_plot.h"
#include "waves_plot.h"
#include "scatter_plot.h"
#include "table.h"


enum class PLOT_TYPE{
    RAW_PLOT,
    FILTERED_PLOT,
    TABLE,
    POINCARE,
    HISTOGRAM,
    HRV_TABLE,
    NO_PLOT
};

enum class PLOT_FLAGS{
    FILTER_MM,
    FILTER_SG,
    FILTER_BUTTER,
    FILTER_LMS,
    FLAG_NONE
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
    void on_btnHRV_1_clicked();
    void on_btnHRV2_PC_clicked();
    void on_btnHRV2_hist_clicked();
    void on_btnHRV_DFA_clicked();
    void on_btnHeartClass_clicked();

    void on_checkBoxRP_stateChanged(int state);
    void on_checkBoxQRS_stateChanged(int state);

    void on_pushButton_clicked();

    void createPlot(QLayout* layout,PLOT_TYPE plotType);
    void on_showTable_stateChanged(int state);
    void resizeLayout();

private:
    Ui::MainWindow *ui;
    SettingsForm *ptrSettingsForm;
    Basic_Plot *plotWidget;
    bool isFileSelected = false;
    bool isSignalAnalyzed = false;

    QString parameter1;
    QString parameter2;
    QString parameter3;
    QString parameter4;
    QString parameter5;
    QString filePath;

    PLOT_TYPE currentPlot = PLOT_TYPE::NO_PLOT;
    PLOT_FLAGS currentFlag = PLOT_FLAGS::FLAG_NONE;

    DataReader fileReader;
    Baseline baseline;
    RPeaks rPeaks;
    HRV_2 hrv2;
    HeartClass heartClassifier;
    Waves waveDetector;
    HRV_DFA dfa;

    std::array<double,5> timeParams;
    std::array<double,6> freqParams;

    QList<int> r_peak_positions;
    QWidget *tableWidget;
    QWidget *currentPlotWidget;
    std::vector<int> qrs_onsets;
    std::vector<int> qrs_ends; 
    Table* hrvTable;
};

#endif // MAINWINDOW_H
