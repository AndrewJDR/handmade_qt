#include "handmadeqt.h"
#include "handmade/code/handmade.h"
#include <QApplication>
#include <QElapsedTimer>
#include <QObject>
#include <QScreen>
#include <QPainter>
#include <QKeyEvent>

global_variable bool32 GlobalRunning;
global_variable bool32 GlobalPause;

global_variable QImage *qImgBuffer;

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender);

HandmadeQt::HandmadeQt(QWidget *parent)
    : QMainWindow(parent), doPainting(false)
{
}

HandmadeQt::~HandmadeQt()
{

}

void HandmadeQt::closeEvent(QCloseEvent *)
{
    GlobalRunning = false;
}

internal void
QtProcessKeyboardMessage(game_button_state *NewState, bool32 IsDown)
{
    if(NewState->EndedDown != IsDown)
    {
        NewState->EndedDown = IsDown;
        ++NewState->HalfTransitionCount;
    }
}

struct QtKeyToGameButton {
    Qt::Key qtKey;
    size_t btnIdx;
};

#define ButtonArrayOffset(btn) ((offsetof(game_controller_input, btn) - offsetof(game_controller_input, Buttons)) / sizeof(game_button_state))
const struct QtKeyToGameButton qtKeyToGameButton[] = {
    { Qt::Key_W, 		ButtonArrayOffset(MoveUp)			},
    { Qt::Key_A, 		ButtonArrayOffset(MoveLeft)			},
    { Qt::Key_S, 		ButtonArrayOffset(MoveDown)			},
    { Qt::Key_D, 		ButtonArrayOffset(MoveRight)		},
    { Qt::Key_Q, 		ButtonArrayOffset(LeftShoulder)		},
    { Qt::Key_E, 		ButtonArrayOffset(RightShoulder)	},

    { Qt::Key_Up, 		ButtonArrayOffset(ActionUp)			},
    { Qt::Key_Left,		ButtonArrayOffset(ActionLeft)		},
    { Qt::Key_Down,		ButtonArrayOffset(ActionDown)		},
    { Qt::Key_Right,	ButtonArrayOffset(ActionRight)		},

    { Qt::Key_Escape,	ButtonArrayOffset(Start)			},
    { Qt::Key_Space,	ButtonArrayOffset(Back)				},
};

game_button_state *HandmadeQt::getButtonStateFromQtKey(int qtKey)
{
    int numKeyButtons = ArrayCount(qtKeyToGameButton);
    for(int i = 0; i < numKeyButtons; i++) {
        if(qtKeyToGameButton[i].qtKey == qtKey) {
            size_t btnIdx = qtKeyToGameButton[i].btnIdx;
            return &NewKeyboardController->Buttons[qtKeyToGameButton[i].btnIdx];
        }
    }
    return 0;
}

void HandmadeQt::keyPressEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->isAutoRepeat()) {
        keyEvent->ignore();
        return;
    }

    game_button_state *thisButtonState = getButtonStateFromQtKey(keyEvent->key());
    if(thisButtonState) {
        if( !thisButtonState->EndedDown ) {
            thisButtonState->EndedDown = true;
            ++thisButtonState->HalfTransitionCount;
        }
    } else {
        // Unrecognized key
        QMainWindow::keyReleaseEvent(keyEvent);
    }
}

void HandmadeQt::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->isAutoRepeat()) {
        keyEvent->ignore();
        return;
    }

    game_button_state *thisButtonState = getButtonStateFromQtKey(keyEvent->key());
    if(thisButtonState) {
        if( thisButtonState->EndedDown ) {
            thisButtonState->EndedDown = false;
            ++thisButtonState->HalfTransitionCount;
        }
    } else {
        // Unrecognized key
        QMainWindow::keyReleaseEvent(keyEvent);
    }
}

void HandmadeQt::paintEvent(QPaintEvent *paintEvent) {
    if( doPainting ) {
        QPainter painter(this);
        QRectF rect(0.0, 0.0, (qreal)qImgBuffer->width(), (qreal)qImgBuffer->height());
        painter.drawImage(rect, *qImgBuffer, rect);
    }
}

void HandmadeQt::forceRepaint() {
    doPainting = true;
    repaint(QRect(0.0, 0.0, (qreal)qImgBuffer->width(), (qreal)qImgBuffer->height()));
    doPainting = false;
}

DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory) { }

DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile) {
    return {};
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile) {
    return false;
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
    GameMemory.DEBUGPlatformFreeFileMemory = DEBUGPlatformFreeFileMemory;
    GameMemory.DEBUGPlatformReadEntireFile = DEBUGPlatformReadEntireFile;
    GameMemory.DEBUGPlatformWriteEntireFile = DEBUGPlatformWriteEntireFile;

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
        w.OldKeyboardController = GetController(OldInput, 0);
        w.NewKeyboardController = GetController(NewInput, 0);
        *w.NewKeyboardController = {};
        w.NewKeyboardController->IsConnected = true;
        for(int ButtonIndex = 0; ButtonIndex < ArrayCount(w.NewKeyboardController->Buttons); ++ButtonIndex) {
            w.NewKeyboardController->Buttons[ButtonIndex].EndedDown = w.OldKeyboardController->Buttons[ButtonIndex].EndedDown;
        }
        QCoreApplication::processEvents(QEventLoop::AllEvents, 5);

        if(!GlobalPause) {
            thread_context Thread = {};

            game_offscreen_buffer Buffer = {};
            Buffer.Memory = qImgBuffer->bits();
            Buffer.Width = qImgBuffer->width();
            Buffer.Height = qImgBuffer->height();
            Buffer.Pitch = qImgBuffer->bytesPerLine();
            Buffer.BytesPerPixel = qImgBuffer->depth() / 8;

            GameUpdateAndRender(&Thread, &GameMemory, NewInput, &Buffer);
            game_input *Temp = NewInput;
            NewInput = OldInput;
            OldInput = Temp;

            w.forceRepaint();

            real32 SecondsElapsedForFrame = NSTOS(frameTimer.nsecsElapsed());
            //qDebug( "secsperframe: %f\n", SecondsElapsedForFrame );
            if(SecondsElapsedForFrame < TargetSecondsPerFrame) {
                while(SecondsElapsedForFrame < TargetSecondsPerFrame) {
                    SecondsElapsedForFrame = NSTOS(frameTimer.nsecsElapsed());
                }
            } else {
                // Missed frame
                qDebug( "missed frame %f!!\n", SecondsElapsedForFrame );
            }

            real32 MSPerFrame = NSTOMS(frameTimer.nsecsElapsed());
            frameTimer.restart();
            //qDebug( "MSPerFrame: %f\n", MSPerFrame );
        }
    }
}
