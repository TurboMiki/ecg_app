#ifndef BASIC_PLOT_H
#define BASIC_PLOT_H

#include <QWidget>
#include "qcustomplot.h"
#include "Signal.h"
#include <QString>

class Basic_Plot : public QWidget
{
    Q_OBJECT  // This is essential!

public:
    explicit Basic_Plot(QWidget *parent = nullptr);
    virtual ~Basic_Plot();

    void setTitle(const QString& title);
    void updateBasicPlot(const Signal& signal, const QVector<int>& highlightIndices, 
                        const QString& legend, const QString& title,
                        const QString& xtitle, const QString& ytitle);

    QCustomPlot* getCustomPlot() const { return customPlot; }

private:
    QCustomPlot *customPlot;
};

#endif // BASIC_PLOT_H