#ifndef HANDMADEQT_H
#define HANDMADEQT_H

#include <QMainWindow>

class HandmadeQt : public QMainWindow
{
    Q_OBJECT

public:
    bool didClose;
    HandmadeQt(QWidget *parent = 0);
    ~HandmadeQt();
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // HANDMADEQT_H
