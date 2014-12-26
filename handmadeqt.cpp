#include "handmadeqt.h"
#include "handmade/code/handmade.h"
#include <QApplication>
#include <QElapsedTimer>
#include <QObject>
#include <QScreen>
#include <QPainter>

global_variable bool32 GlobalRunning;
global_variable bool32 GlobalPause;

global_variable QImage *qImgBuffer;

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender);

HandmadeQt::HandmadeQt(QWidget *parent)
    : QMainWindow(parent)
{
}

HandmadeQt::~HandmadeQt()
{

}

void HandmadeQt::closeEvent(QCloseEvent *)
{
    GlobalRunning = false;
}

void HandmadeQt::paintEvent(QPaintEvent *paintEvent) {
    QPainter painter(this);
    QRectF rect(0.0, 0.0, (qreal)qImgBuffer->width(), (qreal)qImgBuffer->height());
    painter.drawImage(rect, *qImgBuffer, rect);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HandmadeQt w;

    w.show();

    int MonitorRefreshHz = (int)a.primaryScreen()->refreshRate();
    printf("%i\n", MonitorRefreshHz);
    if(MonitorRefreshHz <= 1) {
        MonitorRefreshHz = 60;
    }
    real32 GameUpdateHz = (MonitorRefreshHz / 2.0f);
    real32 TargetSecondsPerFrame = 1.0f / (real32)GameUpdateHz;

    qImgBuffer = new QImage(1280, 720, QImage::Format_RGBX8888);

    game_memory GameMemory = {};
    GameMemory.PermanentStorageSize = Megabytes(64);
    GameMemory.TransientStorageSize = Gigabytes(1);
    GameMemory.DEBUGPlatformFreeFileMemory = 0;
    GameMemory.DEBUGPlatformReadEntireFile = 0;
    GameMemory.DEBUGPlatformWriteEntireFile = 0;

    size_t TotalSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;
    GameMemory.PermanentStorage = (void*) new char[ TotalSize ];
    GameMemory.TransientStorage = ((uint8 *)GameMemory.PermanentStorage + GameMemory.PermanentStorageSize);

    GlobalRunning = true;
    GlobalPause = false;

    game_input Input[2] = {};
    game_input *NewInput = &Input[0];
    game_input *OldInput = &Input[1];

    QElapsedTimer frameTimer;
    frameTimer.start();
    while(GlobalRunning) {
        if(!GlobalPause) {
            thread_context Thread = {};

            game_offscreen_buffer Buffer = {};
            Buffer.Memory = qImgBuffer->bits();
            Buffer.Width = qImgBuffer->width();
            Buffer.Height = qImgBuffer->height();
            Buffer.Pitch = qImgBuffer->bytesPerLine();
            Buffer.BytesPerPixel = qImgBuffer->depth() / 8;

            GameUpdateAndRender(&Thread, &GameMemory, NewInput, &Buffer);

            w.update();
            QCoreApplication::processEvents(QEventLoop::AllEvents, 5);

            real32 SecondsElapsedForFrame = NSTOS(frameTimer.nsecsElapsed());
            //qDebug( "secsperframe: %f\n", SecondsElapsedForFrame );
            if(SecondsElapsedForFrame < TargetSecondsPerFrame) {
                while(SecondsElapsedForFrame < TargetSecondsPerFrame) {
                    SecondsElapsedForFrame = NSTOS(frameTimer.nsecsElapsed());
                }
            } else {
                // Missed frame
                //qDebug( "missed frame!!\n" );
            }

            real32 MSPerFrame = NSTOMS(frameTimer.nsecsElapsed());
            frameTimer.restart();
            //qDebug( "MSPerFrame: %f\n", MSPerFrame );
        }
    }
}
