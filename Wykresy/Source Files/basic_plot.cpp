#include "basic_plot.h"
#include <QString>

Basic_Plot::Basic_Plot(QWidget *parent)
    : QWidget(parent),
    customPlot(new QCustomPlot(this))  // Inicjalizacja obiektu QCustomPlot
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

Basic_Plot::~Basic_Plot()
{
    delete customPlot;  // Zwolnienie pamięci
}

void Basic_Plot::setTitle(const QString& title)
{
    QCPTextElement *titleElement = new QCPTextElement(customPlot, title, QFont("Helvetica", 16, QFont::Bold));
    customPlot->plotLayout()->insertRow(0);  // Dodanie wiersza nad wykresem
    customPlot->plotLayout()->addElement(0, 0, titleElement);
}

void Basic_Plot::updateBasicPlot(const Signal& signal, const QVector<int>& highlightIndices,const QString& legend , const QString& title, const QString& xtitle, const QString& ytitle)
{
    QVector<double> x, y;
    for (double value : signal.getX()) {
        x.append(value);
    }

    for (double value : signal.getY()) {
        y.append(value);
    }

    customPlot->clearGraphs();  // Usuwanie poprzednich wykresów
    customPlot->addGraph();     // Dodanie nowego wykresu
    customPlot->graph(0)->setData(x, y);  // Ustawienie danych
    customPlot->graph(0)->setName(title); // Ustawienie nazwy wykresu
    customPlot->xAxis->setLabel(xtitle);
    customPlot->yAxis->setLabel(ytitle);

    double minY = *std::min_element(y.begin(), y.end());
    double maxY = *std::max_element(y.begin(), y.end());
    customPlot->yAxis->setRange(minY - 0.1, maxY + 0.1);

    if (!highlightIndices.isEmpty()) {
        // Jeśli wykres do wyróżnienia nie istnieje, dodaj go
        if (customPlot->graphCount() < 2) {
            customPlot->addGraph();
            customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, 10));
            customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);            // Czerwone kółka
        }

        QVector<double> highlightX, highlightY;
        for (int index : highlightIndices) {
            if (index >= 0 && index < x.size()) {
                highlightX.append(x[index]);
                highlightY.append(y[index]);
            }
        }

        customPlot->graph(1)->setData(highlightX, highlightY);  // Ustawienie wyróżnionych punktów
        customPlot->graph(1)->setName(legend);
    }

    setTitle(title);  // Ustawienie tytułu wykresu
    customPlot->replot();  // Rysowanie wykresu
}

