#ifndef TABLE_H
#define TABLE_H

#include <QWidget>
#include <QTableWidget>
#include <QVector>
#include <QString>
#include <QLabel>  // Dodanie nagłówka dla QLabel

class Table : public QWidget  // Dziedziczymy po QWidget
{
    Q_OBJECT

public:
    explicit Table(QWidget *parent = nullptr);
    ~Table();

    // Metoda do ustawiania tytułu
    void setTitle(const QString& title);

    // Metoda do ustawiania danych w tabeli
    void setData(const QVector<QVector<QString>>& data);

private:
    QTableWidget* tableWidget;  // Obiekt QTableWidget
    QLabel* titleLabel;         // Obiekt QLabel do wyświetlania tytułu
};

#endif // TABLE_H
