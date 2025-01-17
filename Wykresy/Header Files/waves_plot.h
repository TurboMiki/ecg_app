#ifndef WAVES_PLOT_H
#define WAVES_PLOT_H

#include <QWidget>
#include "qcustomplot.h"
#include "Signal.h"
#include <QString>
#include <QVector>

class Waves_Plot : public QWidget
{
    Q_OBJECT
public:
    explicit Waves_Plot(QWidget *parent = nullptr);
    ~Waves_Plot();

    void setTitle(const QString& title);

    void updateWavesPlot(const Signal& signal,const QString& legend,
                                     const QVector<int>& highlightIndices, const QString& legend0,
                                     const QVector<int>& highlightIndices1, const QString& legend1,
                                     const QVector<int>& highlightIndices2, const QString& legend2,
                                     const QVector<int>& highlightIndices3, const QString& legend3,
                                     const QVector<int>& highlightIndices4, const QString& legend4,
                                     const QString& title, const QString& xtitle, const QString& ytitle);

    QCustomPlot* getCustomPlot() const { return customPlot; }

private:
    QCustomPlot *customPlot;
};

#endif // WAVES_PLOT_H
