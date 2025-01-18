#include "waves_plot.h"
#include <QVBoxLayout>
#include <algorithm>

Waves_Plot::Waves_Plot(QWidget *parent)
    : QWidget(parent),
    customPlot(new QCustomPlot(this))
{
    // Create layout and add QCustomPlot to it
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(customPlot);
    setLayout(layout);

    // Setup the plot
    customPlot->addGraph();
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::blue, 1));
    customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);

    // Enable legend
    customPlot->legend->setVisible(true);
    customPlot->legend->setFont(QFont("Helvetica", 9));
    customPlot->legend->setBrush(QBrush(Qt::white));
    customPlot->legend->setBorderPen(QPen(Qt::black));

    // Setup interactions
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}

Waves_Plot::~Waves_Plot()
{
    // QCustomPlot will be automatically deleted by Qt parent-child mechanism
}

void Waves_Plot::setTitle(const QString& title)
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

void Waves_Plot::updateWavesPlot(const Signal& signal, const QString& legend,
                               const QVector<int>& highlightIndices, const QString& legend0,
                               const QVector<int>& highlightIndices1, const QString& legend1,
                               const QVector<int>& highlightIndices2, const QString& legend2,
                               const QVector<int>& highlightIndices3, const QString& legend3,
                               const QVector<int>& highlightIndices4, const QString& legend4,
                               const QString& title, const QString& xtitle, const QString& ytitle)
{
    // Convert std::vector to QVector
    QVector<double> x(signal.getX().begin(), signal.getX().end());
    QVector<double> y(signal.getY().begin(), signal.getY().end());

    customPlot->clearGraphs();
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    customPlot->graph(0)->setName(legend);
    
    // Set axis labels
    customPlot->xAxis->setLabel(xtitle);
    customPlot->yAxis->setLabel(ytitle);

    // Calculate y axis range with some margin
    auto [minIt, maxIt] = std::minmax_element(y.begin(), y.end());
    double yMin = *minIt;
    double yMax = *maxIt;
    double margin = (yMax - yMin) * 0.1; // 10% margin
    customPlot->yAxis->setRange(yMin - margin, yMax + margin);

    // Set x axis range
    if (!x.empty()) {
        customPlot->xAxis->setRange(x.front(), x.back());
    }

    // Colors for different highlight sets
    QVector<QColor> colors = {Qt::red, Qt::blue, Qt::green, Qt::magenta, Qt::cyan};
    QVector<int> sizes = {10, 10, 10, 10, 10};

    // Helper function to add highlighted points
    auto addHighlights = [&](const QVector<int>& indices, const QString& legendText, 
                            int graphIndex, const QColor& color, int size) {
        if (!indices.isEmpty()) {
            customPlot->addGraph();
            customPlot->graph(graphIndex)->setScatterStyle(
                QCPScatterStyle(QCPScatterStyle::ssCircle, color, size));
            customPlot->graph(graphIndex)->setLineStyle(QCPGraph::lsNone);

            QVector<double> highlightX, highlightY;
            for (int index : indices) {
                if (index >= 0 && index < x.size()) {
                    highlightX.append(x[index]);
                    highlightY.append(y[index]);
                }
            }

            customPlot->graph(graphIndex)->setData(highlightX, highlightY);
            customPlot->graph(graphIndex)->setName(legendText);
        }
    };

    // Add all highlight sets
    if (!highlightIndices.isEmpty()) addHighlights(highlightIndices, legend0, 1, colors[0], sizes[0]);
    if (!highlightIndices1.isEmpty()) addHighlights(highlightIndices1, legend1, 2, colors[1], sizes[1]);
    if (!highlightIndices2.isEmpty()) addHighlights(highlightIndices2, legend2, 3, colors[2], sizes[2]);
    if (!highlightIndices3.isEmpty()) addHighlights(highlightIndices3, legend3, 4, colors[3], sizes[3]);
    if (!highlightIndices4.isEmpty()) addHighlights(highlightIndices4, legend4, 5, colors[4], sizes[4]);

    setTitle(title);
    customPlot->replot();
}