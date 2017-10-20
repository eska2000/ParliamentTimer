#include "parlamenttimer.h"
#include "ui_parlamenttimer.h"
#include <QtDebug>
#include <QFontDialog>
#include <QColorDialog>
#include <QFileDialog>
#include <QErrorMessage>
#include <QMessageBox>
#include <QSound>
#include <QSoundEffect>

ParlamentTimer::ParlamentTimer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParlamentTimer)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose);

    clock = new Clock;
    clock->setText(ui->timerEdit->text());

    loadSettings();

    clock->setFont(textFont);
    clock->setColor(textColor);
    clock->show();

    mainTimer = new QTimer(this);
    mainTimer->setTimerType(Qt::PreciseTimer);
    connect(mainTimer, &QTimer::timeout, this, &ParlamentTimer::timerTimeout);
    updateButtons(ui->timerEdit->time());

    ui->volumeSlider->setValue(mainVolume);
    updateMusicBox();
}

ParlamentTimer::~ParlamentTimer()
{
    clock->close();
    saveSettings();
    delete ui;
}

void ParlamentTimer::updateMusicBox()
{
    ui->musicBox->clear();
    ui->musicBox->addItem("Без звука");

    QFileInfoList files = musicDir.entryInfoList(QDir::Files);

    foreach (QFileInfo file, files) {
        if (file.suffix() == "wav")
            ui->musicBox->addItem(file.baseName());
    }

    ui->musicBox->setCurrentIndex(currentMusic);
}

void ParlamentTimer::timerTimeout()
{
    ui->timerEdit->setTime(ui->timerEdit->time().addSecs(-1));
}

void ParlamentTimer::on_startButton_clicked()
{
    mainTimer->start(1000);
    updateButtons(ui->timerEdit->time());
}

void ParlamentTimer::on_stopButton_clicked()
{
    mainTimer->stop();
    updateButtons(ui->timerEdit->time());
}

void ParlamentTimer::on_clearButton_clicked()
{
    ui->timerEdit->setTime(QTime(0, 0));
}

void ParlamentTimer::addTime(int sec)
{
    QTime time = ui->timerEdit->time();
    time = time.addSecs(sec);
    if (time < QTime(0, 59, 59))
        ui->timerEdit->setTime(time);
    else
        ui->timerEdit->setTime(QTime(0, 59, 59));
}

void ParlamentTimer::loadSettings()
{
    settings = new QSettings("NT Group", "Parliament timer", this);
    textFont = settings->value("font", QFont("Arial", 30)).value<QFont>();
    textColor = settings->value("color", QColor(Qt::black)).value<QColor>();
    mainVolume = settings->value("volume", 50).toInt();
    currentMusic = settings->value("currentMusic", 0).toInt();
    clock->move(settings->value("clockPoint").toPoint());
    ui->checkBox->setChecked(true);

    musicDir = QDir(QDir::currentPath() + QDir::separator() + "music");
    if (!musicDir.exists())
        musicDir.mkdir(musicDir.absolutePath());
}

void ParlamentTimer::saveSettings()
{
    settings->setValue("font", textFont);
    settings->setValue("color", textColor);
    settings->setValue("volume", mainVolume);
    settings->setValue("currentMusic", currentMusic);
    settings->setValue("clockPoint", clock->pos());
}

void ParlamentTimer::on_plus1Button_clicked()
{
    addTime(60);
}

void ParlamentTimer::on_plus3Button_clicked()
{
    addTime(3*60);
}

void ParlamentTimer::on_plus5Button_clicked()
{
    addTime(5*60);
}

void ParlamentTimer::on_plus10Button_clicked()
{
    addTime(10*60);
}

void ParlamentTimer::on_plus20Button_clicked()
{
    addTime(20*60);
}

void ParlamentTimer::on_plus30Button_clicked()
{
    addTime(30*60);
}

void ParlamentTimer::updateButtons(const QTime &time)
{
    if (time == QTime(0, 0, 0)) {
        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(false);
        ui->clearButton->setEnabled(false);
    } else if (mainTimer->isActive()) {
        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
        ui->clearButton->setEnabled(true);
    } else {
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        ui->clearButton->setEnabled(true);
    }

    QTime dif = QTime(0, 59 - time.minute(), 59 - time.second());
    ui->plus1Button->setEnabled((dif < QTime(0, 1)) ? false : true);
    ui->plus3Button->setEnabled((dif < QTime(0, 3)) ? false : true);
    ui->plus5Button->setEnabled((dif < QTime(0, 5)) ? false : true);
    ui->plus10Button->setEnabled((dif < QTime(0, 10)) ? false : true);
    ui->plus20Button->setEnabled((dif < QTime(0, 20)) ? false : true);
    ui->plus30Button->setEnabled((dif < QTime(0, 30)) ? false : true);
}

void ParlamentTimer::on_timerEdit_timeChanged(const QTime &time)
{
    updateButtons(time);

    clock->setUpdatedText(ui->timerEdit->text());

    if (ui->timerEdit->time() == QTime(0, 0) && mainTimer->isActive()) {
        mainTimer->stop();
        if (ui->musicBox->currentIndex()) {
            QSoundEffect *effect = new QSoundEffect(this);
            effect->setSource(QUrl::fromLocalFile(musicDir.absolutePath() + QDir::separator() + ui->musicBox->currentText() + ".wav"));
            effect->setVolume(mainVolume/100.0);
            effect->play();
            connect(effect, &QSoundEffect::playingChanged, this, &ParlamentTimer::onMusicStop);
        }
    }
}

void ParlamentTimer::onMusicStop()
{
    QSoundEffect *effect = qobject_cast<QSoundEffect*>(sender());
    if (effect == NULL)
        return;
    if (effect->isPlaying())
        return;
    effect->deleteLater();
}

void ParlamentTimer::on_fontButton_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, textFont, this, "Параметры шрифта", QFontDialog::DontUseNativeDialog);
    if (ok) {
        textFont = font;
        clock->setFont(font);
        clock->adjustSize();
    }
}

void ParlamentTimer::on_colorButton_clicked()
{
    QColor color = QColorDialog::getColor(textColor, this, "Цвет шрифта", QColorDialog::ShowAlphaChannel);
    if (color.isValid()) {
        textColor = color;
        clock->setColor(textColor);
    }
}

void ParlamentTimer::on_addButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите звуковой файл", QDir::homePath(), "Звуки (*.wav)");
    if (fileName.isEmpty())
        return;
    QFileInfo info(fileName);
    QString copyFileName = musicDir.absolutePath() + QDir::separator() + info.fileName();
    if (QFile(copyFileName).exists()) {
        QMessageBox::critical(this, "Ошибка", "Файл с таким именем уже существует.");
        return;
    }
    bool ok = QFile(fileName).copy(musicDir.absolutePath() + QDir::separator() + info.fileName());
    if (!ok){
        QMessageBox::critical(this, "Ошибка", "Файл не скопирован.");
        return;
    }
    updateMusicBox();

    int lastAddItem = ui->musicBox->findText(info.baseName());
    ui->musicBox->setCurrentIndex(lastAddItem);
}

void ParlamentTimer::on_removeButton_clicked()
{
    int answer = QMessageBox::question(this, "Удаление файла", "Вы уверены, что хотите удалить этот звуковой файл?", QMessageBox::Yes|QMessageBox::No);
    if (answer == QMessageBox::No)
        return;
    QFile(musicDir.absolutePath() + QDir::separator() + ui->musicBox->currentText() + ".wav").remove();
    updateMusicBox();
}

void ParlamentTimer::on_volumeSlider_valueChanged(int value)
{
    mainVolume = value;
}

void ParlamentTimer::on_musicBox_activated(int)
{
    currentMusic = ui->musicBox->currentIndex();
}

void ParlamentTimer::on_musicBox_currentIndexChanged(int index)
{
    ui->removeButton->setEnabled((index == 0) ? false : true);
}

Clock::Clock()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::NoFocus);
}

void Clock::setUpdatedText(const QString &text)
{
    setText(text);
}

void Clock::setColor(const QColor &color)
{
    QString value = QString("%1, %2, %3, %4").arg(QString::number(color.red()),
                                         QString::number(color.green()),
                                         QString::number(color.blue()),
                                         QString::number(color.alpha()));
    setStyleSheet("QLabel {color: rgba(" + value + ")}");
}

void Clock::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Clock::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void ParlamentTimer::on_checkBox_clicked(bool checked)
{
    clock->setVisible(checked);

}
