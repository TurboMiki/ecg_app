#include "basic_plot.h"
#include <QVBoxLayout>
#include <algorithm>

Basic_Plot::Basic_Plot(QWidget *parent)
    : QWidget(parent),
    customPlot(new QCustomPlot(this))
{
    // Tworzenie layoutu
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(customPlot);
    setLayout(layout);

    // Ustawienia wykresu
    customPlot->addGraph();
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::blue, 1));
    customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);

    // Wlaczanie legendy
    customPlot->legend->setVisible(true);
    customPlot->legend->setFont(QFont("Helvetica", 9));
    customPlot->legend->setBrush(QBrush(Qt::white));
    customPlot->legend->setBorderPen(QPen(Qt::black));

    // Interaktywnosc
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}

Basic_Plot::~Basic_Plot()
{
    // Destruktor
}

void Basic_Plot::setTitle(const QString& title)
{
    if (customPlot->plotLayout()->elementCount() > 0)
    {
        if (qobject_cast<QCPTextElement*>(customPlot->plotLayout()->element(0, 0)))
            customPlot->plotLayout()->remove(customPlot->plotLayout()->element(0, 0));
    }

    QCPTextElement *titleElement = new QCPTextElement(customPlot, title, QFont("Helvetica", 12, QFont::Bold));
    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->addElement(0, 0, titleElement);
}

void Basic_Plot::updateBasicPlot(const Signal& signal, const QVector<int>& highlightIndices,
                                 const QString& legend,const QString& indexlegend ,const QString& title,
                                 const QString& xtitle, const QString& ytitle)
{
    // zmioana elementow klasy signal na QT tak by dalo sie je wyplotowac
    QVector<double> x(signal.getX().begin(), signal.getX().end());
    QVector<double> y(signal.getY().begin(), signal.getY().end());

    customPlot->clearGraphs();
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    customPlot->graph(0)->setName(legend);

    // Nazwy osi
    customPlot->xAxis->setLabel(xtitle);
    customPlot->yAxis->setLabel(ytitle);

    // Calculate y axis zakresu z marginesem
    auto [minIt, maxIt] = std::minmax_element(y.begin(), y.end());
    double yMin = *minIt;
    double yMax = *maxIt;
    double margin = (yMax - yMin) * 0.1; // 10% marginesu
    customPlot->yAxis->setRange(yMin - margin, yMax + margin);

    // ustawienie zasiegu osi
    if (!x.empty()) {
        customPlot->xAxis->setRange(x.front(), x.back());
    }

    // Dodanie wyroznionych elemntow jezeli takie sa
    if (!highlightIndices.isEmpty()) {
        customPlot->addGraph();
        customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, 10));
        customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);

        QVector<double> highlightX, highlightY;
        for (int index : highlightIndices) {
            if (index >= 0 && index < x.size()) {
                highlightX.append(x[index]);
                highlightY.append(y[index]);
            }
        }

        customPlot->graph(1)->setData(highlightX, highlightY);
        customPlot->graph(1)->setName(indexlegend);
    }

    setTitle(title);
    customPlot->replot();
}
