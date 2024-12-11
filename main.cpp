#include <QApplication>
#include <QPushButton>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QPushButton button("Click me!");
    QObject::connect(&button, &QPushButton::clicked, [&]() {
        QMessageBox::information(nullptr, "Message", "Hello, ECG Processing!");
    });

    button.show();
    return app.exec();
}
