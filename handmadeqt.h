#ifndef HANDMADEQT_H
#define HANDMADEQT_H

#include <QMainWindow>

class HandmadeQt : public QMainWindow
{
    Q_OBJECT

public:
    HandmadeQt(QWidget *parent = 0);
    ~HandmadeQt();
protected:
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *);
};

#define NSTOS(x) ((real64)x / 1000000000.0)
#define NSTOMS(x) ((real64)x / 1000000.0)

#endif // HANDMADEQT_H
