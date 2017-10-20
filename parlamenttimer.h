#ifndef PARLAMENTTIMER_H
#define PARLAMENTTIMER_H

#include <QWidget>

namespace Ui {
class ParlamentTimer;
}

class ParlamentTimer : public QWidget
{
    Q_OBJECT

public:
    explicit ParlamentTimer(QWidget *parent = 0);
    ~ParlamentTimer();

private:
    Ui::ParlamentTimer *ui;
};

#endif // PARLAMENTTIMER_H
