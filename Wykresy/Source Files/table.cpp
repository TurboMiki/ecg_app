#include "table.h"
#include <QVBoxLayout>
#include <QTableWidgetItem>  // Dodanie tego nagłówka
#include <QLabel>  // Do wyświetlania tytułu

Table::Table(QWidget *parent)
    : QWidget(parent),
    tableWidget(new QTableWidget(this)),  // Tworzymy obiekt tabeli
    titleLabel(new QLabel(this))  // Tworzymy obiekt do wyświetlania tytułu
{
    // Ustawienie układu dla widgetu
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Dodanie tytułu do layoutu
    layout->addWidget(titleLabel);  // Dodajemy tytuł do layoutu

    // Dodanie tabeli do layoutu
    layout->addWidget(tableWidget);  // Dodajemy tabelę do layoutu

    setLayout(layout);
}

Table::~Table()
{
    delete tableWidget;  // Zwolnienie pamięci
    delete titleLabel;  // Zwolnienie pamięci
}

void Table::setData(const QVector<QVector<QString>>& data)
{
    if (data.isEmpty() || data[0].isEmpty()) {
        return;  // Jeśli dane są puste, nic nie rób
    }

    // Ustawienie liczby wierszy i kolumn w tabeli na podstawie danych
    tableWidget->setRowCount(data.size());
    tableWidget->setColumnCount(data[0].size());

    // Wstawianie danych do komórek tabeli
    for (int row = 0; row < data.size(); ++row) {
        for (int col = 0; col < data[row].size(); ++col) {
            tableWidget->setItem(row, col, new QTableWidgetItem(data[row][col]));
        }
    }

    // Dopasowanie rozmiaru kolumn do zawartości
    tableWidget->resizeColumnsToContents();

    // Dopasowanie rozmiaru wierszy do zawartości
    tableWidget->resizeRowsToContents();
}

void Table::setTitle(const QString& title)
{
    titleLabel->setText(title);  // Ustawienie tytułu tabeli
    titleLabel->setAlignment(Qt::AlignCenter);  // Wyśrodkowanie tytułu
    titleLabel->setFont(QFont("Helvetica", 12, QFont::Bold));  // Ustawienie czcionki
}
