#include "QMQTT.h"

#include <QDebug>

QMQTT::QMQTT(QObject * parent) : QObject(parent){
  mosquitto_lib_init();

  _mosq = mosquitto_new("QMQTT", true, this);
  mosquitto_connect_callback_set(_mosq, onConnected);
  mosquitto_disconnect_callback_set(_mosq, onDisconnected);
  mosquitto_message_callback_set(_mosq, onMessage);

  auto timer = new QTimer(this);
  QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(50);
}

QMQTT::QMQTT(QString host, int port, QObject * parent) : QObject(parent), _host(host), _port(port){
    _mosq = mosquitto_new("QMQTT", true, this);
    mosquitto_connect_callback_set(_mosq, onConnected);
    mosquitto_disconnect_callback_set(_mosq, onDisconnected);
    mosquitto_message_callback_set(_mosq, onMessage);
    auto timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(50);
}


QMQTT::~QMQTT() {
  mosquitto_disconnect(_mosq);
  mosquitto_destroy(_mosq);
  mosquitto_lib_cleanup();
}

int QMQTT::nextMid(){
  mid++;

  if(mid > 65536) {
    mid = 0;
  }

  return mid;
}

void QMQTT::setWill(QString topic, QString payload) {
  _willTopic = topic;
  _willPayload = payload;
}
bool QMQTT::connect(QString clientId, QString username, QString password) {
  _clientId = clientId;
  _username = username;
  _password = password;

  mosquitto_reinitialise(_mosq, _clientId.toStdString().c_str(), true, this);
  mosquitto_connect_callback_set(_mosq, onConnected);
  mosquitto_disconnect_callback_set(_mosq, onDisconnected);
  mosquitto_message_callback_set(_mosq, onMessage);

  if(!_username.isEmpty() && !_password.isEmpty()) {
    mosquitto_username_pw_set(_mosq, _username.toStdString().c_str(), _password.toStdString().c_str());
  }

  if(!_willTopic.isEmpty() && !_willPayload.isEmpty()) {
    mosquitto_will_set(_mosq, _willTopic.toStdString().c_str(), (int)_willPayload.length(), _willPayload.toStdString().c_str(), 0, false);
  }

  int rc = mosquitto_connect(_mosq, _host.toStdString().c_str(), _port, 60);

  if(rc != MOSQ_ERR_SUCCESS) {
    qDebug() << "Connect error:" << mosquitto_strerror(rc);
    return false;
  }

  return true;
}

void QMQTT::publish(QString topic, int qos, bool retain) {
  publish(topic, "", qos,retain);
}

void QMQTT::publish(QString topic, QString payload, int qos, bool retain) {
  int mid = nextMid();
  mosquitto_publish(_mosq, &mid, topic.toStdString().c_str(), (int)payload.length(), payload.toStdString().c_str(), qos, retain);
}

void QMQTT::subscribe(QString topic, int qos) {
  int mid = nextMid();
  mosquitto_subscribe(_mosq, &mid, topic.toStdString().c_str(), qos);
}

void QMQTT::unsubscribe(QString topic) {
  int mid = nextMid();
  mosquitto_unsubscribe(_mosq, &mid, topic.toStdString().c_str());
}

void QMQTT::update() {
  int rc1 = mosquitto_loop(_mosq, 0, 1);
  if (rc1 != MOSQ_ERR_SUCCESS) {
    qDebug() << "Loop error:" << mosquitto_strerror(rc1);

    int rc2 = mosquitto_reconnect(_mosq);
    if (rc2 != MOSQ_ERR_SUCCESS) {
      qDebug() << "Reconnect error: " << mosquitto_strerror(rc2);
    }
  }
}

bool QMQTT::isConnected() {
  return _connected;
}

void QMQTT::disconnect() {
  mosquitto_disconnect(_mosq);
}
