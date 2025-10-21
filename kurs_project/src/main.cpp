#include "main.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("bikunovmm");
    QApplication::setOrganizationDomain("bikunovmm.ru");
    QApplication::setApplicationName("converter");

    QFile style_file(":/styles/style.qss");
    style_file.open(QFile::ReadOnly);
    app.setStyleSheet(QLatin1String(style_file.readAll()));

    MainWidget window = MainWidget();
    window.show();

    return app.exec();
}
