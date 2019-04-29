#include "mainwindow.h"
#include <QDebug>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      _mqtt(new QMQTT("localhost", 1883, this)),
      _slider(new QSlider(this)),
      _toggle(new QPushButton(this)),
      _lineEdit(new QLineEdit(this)),
      _spinBox(new QDoubleSpinBox)
{
    connect(_mqtt, SIGNAL(gotMessage(QMQTTMessage)), this, SLOT(onMessageReceived(QMQTTMessage)));


    setWindowTitle("QMQTTExample");
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->addWidget(_slider);
    mainLayout->addWidget(_lineEdit);
    mainLayout->addWidget(_toggle);
    mainLayout->addWidget(_spinBox);
    setCentralWidget(mainWidget);

    mainWidget->setEnabled(false);
    _toggle->setCheckable(true);

}

MainWindow::~MainWindow()
{

}

void MainWindow::onMessage(QMQTTMessage msg)
{
    qDebug()<<"new message" << msg.topic << msg.payload;
}


