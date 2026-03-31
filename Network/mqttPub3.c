#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "MQTTClient.h"

#define ADDRESS "tcp://163.152.213.106:1883"
#define TOPIC "sensor/temp"
#define QOS 1

MQTTClient client;

// 수신 콜백
int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    float temp = atof((char*)message->payload);

    if (temp > 30)
        printf("\n🔥 온도: %.1f°C (높음!)\n", temp);
    else
        printf("\n🌡️ 온도: %.1f°C\n", temp);

    fflush(stdout);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int main() {
    srand(time(NULL));

    char clientID[50];
    sprintf(clientID, "client_%d", rand()); // 랜덤 ID

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

    MQTTClient_create(&client, ADDRESS, clientID,
                      MQTTCLIENT_PERSISTENCE_NONE, NULL);

    MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL);

    if (MQTTClient_connect(client, &conn_opts) != MQTTCLIENT_SUCCESS) {
        printf("연결 실패\n");
        return -1;
    }

    MQTTClient_subscribe(client, TOPIC, QOS);

    printf("온도 모니터 시작!\n");

    while (1) {
        float temp = (rand() % 400) / 10.0; // 0~40도

        char msg[20];
        sprintf(msg, "%.1f", temp);

        MQTTClient_message pubmsg = MQTTClient_message_initializer;
        pubmsg.payload = msg;
        pubmsg.payloadlen = strlen(msg);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;

        MQTTClient_publishMessage(client, TOPIC, &pubmsg, NULL);

        sleep(1); // 1초마다 전송
    }

    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    return 0;
}