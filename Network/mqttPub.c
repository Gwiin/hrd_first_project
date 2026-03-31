#include <stdio.h>
#include <string.h>
#include "MQTTClient.h"
// 브로커 PC의 Windows 실제 IP 주소를 입력하세요
#define ADDRESS "tcp://163.152.213.106:1883"
#define CLIENTID "Hwang"
#define TOPIC "school/test"

int main() {
 MQTTClient client;
 MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
 MQTTClient_message pubmsg = MQTTClient_message_initializer;

 MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

 if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS) {
 printf("접속 실패!\n");
 return -1;
 }
 pubmsg.payload = "안녕하세요 황지용입니다.";
 pubmsg.payloadlen = (int)strlen(pubmsg.payload);
 pubmsg.qos = 1;
 MQTTClient_publishMessage(client, TOPIC, &pubmsg, NULL);
 printf("메시지 전송 완료!\n");
 MQTTClient_disconnect(client, 10000);
 MQTTClient_destroy(&client);
 return 0;
}
