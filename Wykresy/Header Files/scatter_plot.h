#ifndef SCATTER_PLOT_H
#define SCATTER_PLOT_H

#include <QWidget>
#include <QCustomPlot.h>
#include <QString>
#include "Signal.h"

class Scatter_Plot : public QWidget
{
    Q_OBJECT

public:
    explicit Scatter_Plot(QWidget *parent = nullptr);
    ~Scatter_Plot();

    void setTitle(const QString& title);  // Ustawienie tytulu
    void updateScatterPlot(const Signal& signal,double x_elipse,double y_elipse,double smaller_rad, double greater_rad ,const QString& legend_rr,const QString& legend_sd_elipse , const QString& title, const QString& xtitle, const QString& ytitle);  // Update scatter plot data

    QCustomPlot* getCustomPlot() const { return customPlot; }

private:
    QCustomPlot *customPlot;  // Wskaznik do QCustomPlot object
};

#endif // SCATTER_PLOT_H
