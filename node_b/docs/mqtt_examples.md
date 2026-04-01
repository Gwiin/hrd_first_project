# MQTT examples

브로커:

- Host: `163.152.213.101`
- Port: `1883`

## 전체 토픽 구독

```bash
mosquitto_sub -h 163.152.213.101 -p 1883 -t '/tempservo_led/#' -v
```

## 센서 JSON만 구독

```bash
mosquitto_sub -h 163.152.213.101 -p 1883 -t '/tempservo_led/sensor/json' -v
```

## 상태 JSON만 구독

```bash
mosquitto_sub -h 163.152.213.101 -p 1883 -t '/tempservo_led/status/json' -v
```

## 서보 제어

```bash
mosquitto_pub -h 163.152.213.101 -p 1883 -t '/tempservo_led/servo' -m 'open'
mosquitto_pub -h 163.152.213.101 -p 1883 -t '/tempservo_led/servo' -m 'close'
mosquitto_pub -h 163.152.213.101 -p 1883 -t '/tempservo_led/servo' -m '{"mode":"auto"}'
```

## LED 제어

```bash
mosquitto_pub -h 163.152.213.101 -p 1883 -t '/tempservo_led/led' -m 'on'
mosquitto_pub -h 163.152.213.101 -p 1883 -t '/tempservo_led/led' -m 'off'
mosquitto_pub -h 163.152.213.101 -p 1883 -t '/tempservo_led/led' -m '{"mode":"auto"}'
```

## JSON 제어 예시

```bash
mosquitto_pub -h 163.152.213.101 -p 1883 -t '/tempservo_led/servo' -m '{"servo":"open"}'
mosquitto_pub -h 163.152.213.101 -p 1883 -t '/tempservo_led/servo' -m '{"state":"closed"}'
mosquitto_pub -h 163.152.213.101 -p 1883 -t '/tempservo_led/led' -m '{"led":"on"}'
mosquitto_pub -h 163.152.213.101 -p 1883 -t '/tempservo_led/led' -m '{"state":"off"}'
```
