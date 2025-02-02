#include "histogram_plot.h"
#include <QString>
#include "qcustomplot.h"
#include <QVBoxLayout>

Histogram_Plot::Histogram_Plot(QWidget *parent)
    : QWidget(parent),
    customPlot(new QCustomPlot(this)),  // Inicjalizacja obiektu QCustomPlot
    bars(new QCPBars(customPlot->xAxis, customPlot->yAxis))  // Inicjalizacja wskaźnika na QCPBars
{
    // Ustawienie stylu dla wykresu słupkowego
    bars->setPen(QPen(Qt::black));  // Kolor obramowania słupków
    bars->setBrush(QBrush(Qt::blue));  // Kolor wypełnienia słupków
    bars->setWidth(0.009);  // Zmniejszenie szerokości słupków, aby były bliżej siebie

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(customPlot);
    setLayout(layout);

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
    delete customPlot;  // Zwolnienie pamięci  // Zwolnienie pamięci dla wskaźnika bars
}

void Histogram_Plot::setTitle(const QString& title)
{
    QCPTextElement *titleElement = new QCPTextElement(customPlot, title, QFont("Helvetica", 16, QFont::Bold));
    customPlot->plotLayout()->insertRow(0);  // Dodanie wiersza nad wykresem
    customPlot->plotLayout()->addElement(0, 0, titleElement);
}

void Histogram_Plot::updateHistogramPlot(const Signal& signal, const QString& title, const QString& legend, const QString& xtitle, const QString& ytitle)
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

    // Ustawienie zakresu dla osi Y (aby unikać wartości ujemnych)
    double minY = *std::min_element(y.begin(), y.end());
    double maxY = *std::max_element(y.begin(), y.end());

    double minX = *std::min_element(x.begin(), x.end());
    double maxX = *std::max_element(x.begin(), x.end());

    // Ustawiamy minimalny zakres na 0, aby unikać wartości ujemnych
    customPlot->yAxis->setRange(std::max(0.0, minY - 0.1), maxY + 5);
    customPlot->xAxis->setRange(std::max(0.0, minX - 0.1), maxX + 1);

    customPlot->xAxis->setLabel(xtitle);
    customPlot->yAxis->setLabel(ytitle);
    bars->setName(legend);

    // Ustawienie tytułu wykresu
    setTitle(title);

    // Odświeżenie wykresu
    customPlot->replot();
}
