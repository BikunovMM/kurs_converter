#include "main.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("bikunovmm");
    QApplication::setOrganizationDomain("bikunovmm.ru");
    QApplication::setApplicationName("converter");

    QFile style_file(":/styles/style.qss");
    if (!style_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fprintf(stderr, "Failed to open style.qss.\n");
        return 1;
    }
    app.setStyleSheet(QLatin1String(style_file.readAll()));

    if (Server::Requester::set_up()) {/* Failed to load server_config */
        fprintf(stderr, "Failed to load server_config.\n");
        return 1;
    }

    MainWidget window = MainWidget();
    window.show();

    return app.exec();
}
