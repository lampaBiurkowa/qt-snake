#include <QApplication>
#include <QTime>
#include "snake.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Snake window;
    qsrand(QTime::currentTime().msec());
    window.show();
    return app.exec();
}
