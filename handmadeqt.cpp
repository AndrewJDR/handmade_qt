#include "handmadeqt.h"
#include <QApplication>
#include <QElapsedTimer>
#include <QObject>

HandmadeQt::HandmadeQt(QWidget *parent)
    : QMainWindow(parent), didClose(false)
{
}

HandmadeQt::~HandmadeQt()
{

}

void HandmadeQt::closeEvent(QCloseEvent *)
{
    didClose = true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HandmadeQt w;

    w.show();

    while(true) {
        QElapsedTimer timer;
        timer.start();
        QCoreApplication::processEvents(QEventLoop::AllEvents, 5);

        if(w.didClose) {
            break;
        }

        //printf( "elapsed: %i\n", timer.elapsed() );
        timer.restart();
        while(!timer.hasExpired(15));
    }
}
