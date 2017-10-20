#ifndef PARLAMENTTIMER_H
#define PARLAMENTTIMER_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QSettings>
#include <QDir>
#include <QMouseEvent>
#include <QApplication>

class Clock : public QLabel
{
    Q_OBJECT

    QPoint dragPosition;

public:
    Clock();
    virtual ~Clock() {}
    void setUpdatedText(const QString &);
    void setColor(const QColor &);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

namespace Ui {
class ParlamentTimer;
}

class ParlamentTimer : public QWidget
{
    Q_OBJECT

public:
    explicit ParlamentTimer(QWidget *parent = 0);
    ~ParlamentTimer();

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_clearButton_clicked();

    void on_plus1Button_clicked();
    void on_plus3Button_clicked();
    void on_plus5Button_clicked();
    void on_plus10Button_clicked();
    void on_plus20Button_clicked();
    void on_plus30Button_clicked();

    void on_timerEdit_timeChanged(const QTime &time);

    void on_fontButton_clicked();
    void on_colorButton_clicked();

    void on_addButton_clicked();
    void on_removeButton_clicked();
    void on_musicBox_activated(int index);
    void on_musicBox_currentIndexChanged(int index);

    void timerTimeout();
    void on_volumeSlider_valueChanged(int value);


    void onMusicStop();

    void on_checkBox_clicked(bool checked);

private:
    Ui::ParlamentTimer *ui;

    Clock *clock;

    void addTime(int sec);
    void loadSettings();
    void saveSettings();
    QSettings *settings;

    QFont textFont;
    QColor textColor;
    int mainVolume;
    QDir musicDir;
    int currentMusic;

    QTimer *mainTimer;
    void updateButtons(const QTime &time);
    void updateMusicBox();

protected:
    void closeEvent(QCloseEvent *) override { QApplication::quit(); }
};

#endif // PARLAMENTTIMER_H
