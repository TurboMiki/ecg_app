#include "waves_plot.h"
#include "signal.h"  // Zakładam, że masz odpowiedni plik do klasy Signal
#include <algorithm>

Waves_Plot::Waves_Plot(QWidget *parent)
    : QWidget(parent),
    customPlot(new QCustomPlot(this))
{
    customPlot->setGeometry(10, 10, 800, 600);  // Ustawienie rozmiaru wykresu w widgetzie

    customPlot->addGraph();
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::blue, 1));
    customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);


    // Włączenie legendy
    customPlot->legend->setVisible(true);
    customPlot->legend->setFont(QFont("Helvetica", 9));
    customPlot->legend->setBrush(QBrush(Qt::white));
    customPlot->legend->setBorderPen(QPen(Qt::black));

    // Ustawienia interakcji z wykresem
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}

Waves_Plot::~Waves_Plot() { delete customPlot; }

void Waves_Plot::setTitle(const QString& title) {
    if (customPlot) {
        customPlot->plotLayout()->insertRow(0);
        QCPTextElement *titleElement = new QCPTextElement(customPlot, title, QFont("sans", 12, QFont::Bold));
        customPlot->plotLayout()->addElement(0, 0, titleElement);
    }
}

void Waves_Plot::updateWavesPlot(const Signal& signal,const QString& legend,
                                 const QVector<int>& highlightIndices, const QString& legend0,
                                 const QVector<int>& highlightIndices1, const QString& legend1,
                                 const QVector<int>& highlightIndices2, const QString& legend2,
                                 const QVector<int>& highlightIndices3, const QString& legend3,
                                 const QVector<int>& highlightIndices4, const QString& legend4,
                                 const QString& title, const QString& xtitle, const QString& ytitle) {
    QVector<double> x, y;
    for (double value : signal.getX()) x.append(value);
    for (double value : signal.getY()) y.append(value);

    customPlot->clearGraphs();
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    customPlot->graph(0)->setName(legend);
    customPlot->xAxis->setLabel(xtitle);
    customPlot->yAxis->setLabel(ytitle);

    double minY = *std::min_element(y.begin(), y.end());
    double maxY = *std::max_element(y.begin(), y.end());
    customPlot->yAxis->setRange(minY - 0.1, maxY + 0.1);

    // Kolory i rozmiary dla punktów wyróżnionych
    QVector<QColor> colors = {Qt::red, Qt::blue, Qt::green, Qt::magenta, Qt::cyan};
    QVector<int> sizes = {10, 12, 14, 16, 18};  // Rozmiary punktów

    auto addHighlightGraph = [&](const QVector<int>& indices, const QString& legend, int graphIndex, QColor color, int size) {
        if (!indices.isEmpty()) {
            if (customPlot->graphCount() <= graphIndex) {
                customPlot->addGraph();
                customPlot->graph(graphIndex)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, color, size));
                customPlot->graph(graphIndex)->setLineStyle(QCPGraph::lsNone);  // Brak linii łączącej punkty
            }
            QVector<double> highlightX, highlightY;
            for (int index : indices) {
                if (index >= 0 && index < x.size()) {
                    highlightX.append(x[index]);
                    highlightY.append(y[index]);
                }
            }
            customPlot->graph(graphIndex)->setData(highlightX, highlightY);
            customPlot->graph(graphIndex)->setName(legend);
        }
    };

    // Dodanie punktów wyróżnionych
    addHighlightGraph(highlightIndices, legend, 1, colors[0], sizes[0]);
    addHighlightGraph(highlightIndices1, legend1, 2, colors[1], sizes[1]);
    addHighlightGraph(highlightIndices2, legend2, 3, colors[2], sizes[2]);
    addHighlightGraph(highlightIndices3, legend3, 4, colors[3], sizes[3]);
    addHighlightGraph(highlightIndices4, legend4, 5, colors[4], sizes[4]);

    setTitle(title);
    customPlot->replot();
}
