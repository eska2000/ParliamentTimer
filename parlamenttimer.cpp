#include "parlamenttimer.h"
#include "ui_parlamenttimer.h"

ParlamentTimer::ParlamentTimer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParlamentTimer)
{
    ui->setupUi(this);
}

ParlamentTimer::~ParlamentTimer()
{
    delete ui;
}
