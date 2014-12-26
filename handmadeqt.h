#ifndef HANDMADEQT_H
#define HANDMADEQT_H

#include <QMainWindow>
#include "handmade/code/handmade.h"

class HandmadeQt : public QMainWindow
{
    Q_OBJECT

public:
    bool doPainting;
    game_controller_input *OldKeyboardController;
    game_controller_input *NewKeyboardController;

    HandmadeQt(QWidget *parent = 0);
    ~HandmadeQt();
    void forceRepaint();
protected:
    bool doRepaint;
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    game_button_state *getButtonStateFromQtKey(int qtKey);
};

#define NSTOS(x) ((real64)x / 1000000000.0)
#define NSTOMS(x) ((real64)x / 1000000.0)

#endif // HANDMADEQT_H
