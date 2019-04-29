#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QMQTT.h"

#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QMQTT *_mqtt;
public slots:
    void onMessage(QMQTTMessage msg);

private:
    QSlider *_slider;
    QPushButton *_toggle;
    QLineEdit *_lineEdit;
    QDoubleSpinBox *_spinBox;
};

#endif // MAINWINDOW_H
