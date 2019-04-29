#include <QObject>
#include <QString>
#include <QTimer>

#include "mosquitto.h"

struct QMQTTMessage {
    QString topic;
    QString payload;
};

class QMQTT : public QObject {
    Q_OBJECT
public:
  QMQTT(QObject * parent = nullptr);
  QMQTT(QString host, int port, QObject * parent = nullptr);
  ~QMQTT();

  void setWill(QString willTopic, QString willPayload = "");
  bool connect(QString clientId, QString username = "", QString password = "");
  void publish(QString topic, int qos = 0, bool retain = false);
  void publish(QString topic, QString payload, int qos = 0, bool retain = false);
  void subscribe(QString topic, int qos = 0);
  void unsubscribe(QString topic);
  bool isConnected();
  void disconnect();

  static void onConnected(struct mosquitto *mosq, void *obj, int result)
  {
      Q_UNUSED(mosq);
      auto connected = (result == MOSQ_ERR_SUCCESS);
      if(connected){
          auto qmqtt = (QMQTT *)(obj);
          qmqtt->emitConnected();
      }
  }
  static void onDisconnected(struct mosquitto *mosq, void *obj, int result)
  {
      Q_UNUSED(mosq);
      Q_UNUSED(result);
      auto qmqtt = (QMQTT *)(obj);
      qmqtt->emitDisconnected();
  }

  static void onMessage(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
  {
      Q_UNUSED(mosq);
      Q_UNUSED(obj);
      QMQTTMessage msg;
      msg.topic = message->topic;
      auto payload = std::string((char*)message->payload, (uint)message->payloadlen);
      msg.payload = QString::fromStdString(payload);
      auto qmqtt = (QMQTT *)(obj);
      qmqtt->emitMessage(msg);
  }

  void emitConnected(){
      _connected = true;
      emit connected();
  }
  void emitDisconnected(){
      _connected = false;
      emit disconnected();
  }
  void emitMessage(QMQTTMessage message){
      emit gotMessage(message);
  }

private:
  struct mosquitto *_mosq;
  QString _host;
  int _port;
  QString _clientId;
  QString _username;
  QString _password;
  bool _connected;
  QString _willTopic;
  QString _willPayload;

  int mid = 0;
  int nextMid();

private slots:
  void update();


signals:
  void connected();
  void disconnected();
  void gotMessage(QMQTTMessage message);

};
