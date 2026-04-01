#ifndef TEMPSERVO_LED_CONTRACT_H
#define TEMPSERVO_LED_CONTRACT_H

#define TEMPSERVO_LED_BROKER_HOST "163.152.213.101"
#define TEMPSERVO_LED_BROKER_PORT 1883

#define TEMPSERVO_LED_TOPIC_TEMPERATURE "/tempservo_led/temperature"
#define TEMPSERVO_LED_TOPIC_HUMIDITY "/tempservo_led/humidity"
#define TEMPSERVO_LED_TOPIC_SENSOR "/tempservo_led/sensor"
#define TEMPSERVO_LED_TOPIC_SENSOR_JSON "/tempservo_led/sensor/json"

#define TEMPSERVO_LED_TOPIC_SERVO_CONTROL "/tempservo_led/servo"
#define TEMPSERVO_LED_TOPIC_LED_CONTROL "/tempservo_led/led"

#define TEMPSERVO_LED_TOPIC_SERVO_STATE "/tempservo_led/servo/state"
#define TEMPSERVO_LED_TOPIC_SERVO_STATE_JSON "/tempservo_led/servo/state/json"
#define TEMPSERVO_LED_TOPIC_LED_STATE "/tempservo_led/led/state"
#define TEMPSERVO_LED_TOPIC_LED_STATE_JSON "/tempservo_led/led/state/json"
#define TEMPSERVO_LED_TOPIC_STATUS "/tempservo_led/status"
#define TEMPSERVO_LED_TOPIC_STATUS_JSON "/tempservo_led/status/json"
#define TEMPSERVO_LED_TOPIC_ONLINE "/tempservo_led/online"

#define TEMPSERVO_LED_CMD_AUTO "auto"
#define TEMPSERVO_LED_CMD_OPEN "open"
#define TEMPSERVO_LED_CMD_CLOSE "close"
#define TEMPSERVO_LED_CMD_ON "on"
#define TEMPSERVO_LED_CMD_OFF "off"

#define TEMPSERVO_LED_STATE_OPEN "OPEN"
#define TEMPSERVO_LED_STATE_CLOSED "CLOSED"
#define TEMPSERVO_LED_STATE_ON "ON"
#define TEMPSERVO_LED_STATE_OFF "OFF"

#endif
