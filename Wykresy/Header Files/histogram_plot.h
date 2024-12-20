#ifndef HISTOGRAM_PLOT_H
#define HISTOGRAM_PLOT_H

#include <QWidget>
#include <QCustomPlot.h>

class Histogram_Plot : public QWidget
{
    Q_OBJECT

public:
    explicit Histogram_Plot(QWidget *parent = nullptr);
    ~Histogram_Plot();

    void setTitle(const QString& title);
    void updateHistogramPlot(const Signal& signal, const QString& title);

private:
    QCustomPlot *customPlot;  // Obiekt QCustomPlot
    QCPBars *bars;  // Wskaźnik na QCPBars (słupki wykresu)
};

#endif // HISTOGRAM_PLOT_H
