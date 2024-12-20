#include "histogram_plot.h"
#include <QString>
#include "qcustomplot.h"

Histogram_Plot::Histogram_Plot(QWidget *parent)
    : QWidget(parent),
    customPlot(new QCustomPlot(this)),  // Inicjalizacja obiektu QCustomPlot
    bars(new QCPBars(customPlot->xAxis, customPlot->yAxis))  // Inicjalizacja wskaźnika na QCPBars
{
    customPlot->setGeometry(10, 10, 800, 600);  // Ustawienie rozmiaru wykresu w widgetzie

    // Ustawienie stylu dla wykresu słupkowego
    bars->setPen(QPen(Qt::black));  // Kolor obramowania słupków
    bars->setBrush(QBrush(Qt::blue));  // Kolor wypełnienia słupków
    bars->setWidth(0.9);  // Zmniejszenie szerokości słupków, aby były bliżej siebie

    // Ustawienia osi
    customPlot->xAxis->setLabel("Bin");
    customPlot->yAxis->setLabel("Ilość wystąpień");

    // Włączenie legendy
    customPlot->legend->setVisible(true);
    customPlot->legend->setFont(QFont("Helvetica", 9));
    customPlot->legend->setBrush(QBrush(Qt::white));
    customPlot->legend->setBorderPen(QPen(Qt::black));

    // Ustawienia interakcji z wykresem
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);

    // Ograniczenie przesuwania poniżej 0 dla osi X i Y
    customPlot->xAxis->setRange(0, 10);  // Ustawiamy minimalny zakres osi X na 0
    customPlot->yAxis->setRange(0, 10);  // Ustawiamy minimalny zakres osi Y na 0
}

Histogram_Plot::~Histogram_Plot()
{
    delete customPlot;  // Zwolnienie pamięci
    delete bars;  // Zwolnienie pamięci dla wskaźnika bars
}

void Histogram_Plot::setTitle(const QString& title)
{
    QCPTextElement *titleElement = new QCPTextElement(customPlot, title, QFont("Helvetica", 16, QFont::Bold));
    customPlot->plotLayout()->insertRow(0);  // Dodanie wiersza nad wykresem
    customPlot->plotLayout()->addElement(0, 0, titleElement);
}

void Histogram_Plot::updateHistogramPlot(const Signal& signal, const QString& title)
{
    QVector<double> x, y;

    // Wczytanie danych do wykresu
    for (double value : signal.getX()) {
        x.append(value);
    }

    for (double value : signal.getY()) {
        y.append(value);
    }

    bars->setData(x, y);  // Ustawienie danych dla słupków
    bars->setName(title); // Ustawienie nazwy wykresu

    // Ustawienie zakresu dla osi Y (aby unikać wartości ujemnych)
    double minY = *std::min_element(y.begin(), y.end());
    double maxY = *std::max_element(y.begin(), y.end());

    double minX = *std::min_element(x.begin(), x.end());
    double maxX = *std::max_element(x.begin(), x.end());

    // Ustawiamy minimalny zakres na 0, aby unikać wartości ujemnych
    customPlot->yAxis->setRange(std::max(0.0, minY - 0.1), maxY + 5);
    customPlot->xAxis->setRange(std::max(0.0, minX - 0.1), maxX + 1);

    // Ustawienie tytułu wykresu
    setTitle(title);

    // Odświeżenie wykresu
    customPlot->replot();
}
