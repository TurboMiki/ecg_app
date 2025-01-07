#ifndef BASIC_PLOT_H
#define BASIC_PLOT_H

#include <QWidget>
#include <QCustomPlot.h>  // Zakładając, że masz zainstalowaną bibliotekę QCustomPlot
#include <QString>

class Basic_Plot : public QWidget
{
    Q_OBJECT

public:
    explicit Basic_Plot(QWidget *parent = nullptr);
    ~Basic_Plot();

    void setTitle(const QString& title);  // Ustawienie tytułu wykresu
    void updateBasicPlot(const Signal& signal, const QVector<int>& highlightIndices,const QString& legend , const QString& title, const QString& xtitle, const QString& ytitle);  // Metoda do rysowania danych

    // Metoda zwracająca widget QCustomPlot (można używać do dalszej konfiguracji wykresu)
    QCustomPlot* getCustomPlot() const { return customPlot; }

private:
    QCustomPlot *customPlot;  // Wskaźnik na obiekt QCustomPlot
};

#endif // BASIC_PLOT_H
