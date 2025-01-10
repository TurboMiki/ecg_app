#ifndef HISTOGRAM_PLOT_H
#define HISTOGRAM_PLOT_H

#include <QWidget>
#include "qcustomplot.h"
#include "Signal.h"

class Histogram_Plot : public QWidget
{
    Q_OBJECT

public:
    explicit Histogram_Plot(QWidget *parent = nullptr);
    ~Histogram_Plot();

    void setTitle(const QString& title);
    void updateHistogramPlot(const Signal& signal, const QString& title,const QString& legend ,const QString& xtitle, const QString& ytitle);

private:
    QCustomPlot *customPlot;  // Obiekt QCustomPlot
    QCPBars *bars;  // Wskaźnik na QCPBars (słupki wykresu)
};

#endif // HISTOGRAM_PLOT_H
