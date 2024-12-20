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
    customPlot->xAxis->setLabel("Czas [s]");
    customPlot->yAxis->setLabel("Amplituda [mV]");

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

void Basic_Plot::updateBasicPlot(const Signal& signal, int highlightIndex, const QString& title)
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

    double minY = *std::min_element(y.begin(), y.end());
    double maxY = *std::max_element(y.begin(), y.end());
    customPlot->yAxis->setRange(minY - 0.1, maxY + 0.1);

    if (highlightIndex >= 0 && highlightIndex < x.size()) {
        // Jeśli wykres do wyróżnienia nie istnieje, dodaj go
        if (customPlot->graphCount() < 2) {
            customPlot->addGraph();
            customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, 10));  // Czerwone kółko
        }

        // Nadpisanie danych wykresu wyróżnionego punktu
        customPlot->graph(1)->setData(x.mid(highlightIndex, 1), y.mid(highlightIndex, 1));  // Ustawienie tylko wyróżnionego punktu
        customPlot->graph(1)->setName("Wyróżniony punkt");
    }


    setTitle(title);  // Ustawienie tytułu wykresu
    customPlot->replot();  // Rysowanie wykresu
}



