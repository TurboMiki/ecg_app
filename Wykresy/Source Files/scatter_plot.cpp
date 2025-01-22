#include "scatter_plot.h"
#include <algorithm>

Scatter_Plot::Scatter_Plot(QWidget *parent)
    : QWidget(parent),
    customPlot(new QCustomPlot(this))
{

    // Parametry wykresu
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(customPlot);
    setLayout(layout);

    customPlot->addGraph();

    // Wlaczenie legendy
    customPlot->legend->setVisible(true);
    customPlot->legend->setFont(QFont("Helvetica", 9));
    customPlot->legend->setBrush(QBrush(Qt::white));
    customPlot->legend->setBorderPen(QPen(Qt::black));

    // Interaktywnosc
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}
// Destruktor
Scatter_Plot::~Scatter_Plot()
{
    delete customPlot;
}
// Ustawienie tytulu
void Scatter_Plot::setTitle(const QString& title)
{
    QCPTextElement *titleElement = new QCPTextElement(customPlot, title, QFont("Helvetica", 16, QFont::Bold));
    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->addElement(0, 0, titleElement);
}
// Wywolywanie wykresu
void Scatter_Plot::updateScatterPlot(const Signal& signal, double x_elipse, double y_elipse, double smaller_rad, double greater_rad, const QString& legend_rr, const QString& legend_sd_elipse, const QString& title, const QString& xtitle, const QString& ytitle)
{
    //  Konwersja wartosci z klasy signal tak by dalo sie je wyplottowac
    QVector<double> x, y;
    for (double value : signal.getX()) {
        x.append(value);
    }

    for (double value : signal.getY()) {
        y.append(value);
    }

    //Ustawienia do wykresu
    customPlot->clearGraphs();
    customPlot->addGraph();

    customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::blue, 7));
    customPlot->graph(0)->setData(x, y);
    customPlot->graph(0)->setName(legend_rr);

    customPlot->xAxis->setLabel(xtitle);
    customPlot->yAxis->setLabel(ytitle);
    // Generowanie Elipsy
    QVector<double> ellipseX, ellipseY;
    int points = 200;  // Ilość punktów elipsy

    double angleMinus45 = M_PI / 4.0;  // -45 stopni w radianach
    for (int i = 0; i <= points; ++i) {
        double t = 2 * M_PI * i / points;  // Parametr kąta

        // Współrzędne elipsy przed obrotem
        double ex = smaller_rad * cos(t);
        double ey = greater_rad * sin(t);

        // Obrót o -45 stopni
        double rotatedX = ex * cos(angleMinus45) - ey * sin(angleMinus45);
        double rotatedY = ex * sin(angleMinus45) + ey * cos(angleMinus45);

        // Przesunięcie elipsy do punktu (x_elipse, y_elipse)
        ellipseX.append(rotatedX + x_elipse);
        ellipseY.append(rotatedY + y_elipse);
    }

    // Dodanie elipsy jako punktów wypełnionych
    customPlot->addGraph();
    customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);  // Brak linii łączącej punkty
    customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::red, 3));  // Ustawienie stylu punktów jako wypełnionych (ssDisc) z czerwonym kolorem i większym rozmiarem
    customPlot->graph(1)->setData(ellipseX, ellipseY);
    customPlot->graph(1)->setName(legend_sd_elipse);  // Dodanie elipsy do legendy

    // Ustawienie zakresów wykresu
    double minX = *std::min_element(x.begin(), x.end());
    double maxX = *std::max_element(x.begin(), x.end());
    double minY = *std::min_element(y.begin(), y.end());
    double maxY = *std::max_element(y.begin(), y.end());

    customPlot->xAxis->setRange(minX - 0.1, maxX + 0.1);
    customPlot->yAxis->setRange(minY - 0.1, maxY + 0.1);

    setTitle(title);
    customPlot->replot();
}


