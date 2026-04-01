# node_b

`tempservo_led.c`를 `node_c`와 통합할 때 필요한 자료만 모아둔 폴더입니다.

이 폴더의 목적:

- `node_c`가 구독/발행해야 하는 MQTT 토픽 정리
- 허용하는 명령 형식 정리
- 상태/센서 JSON 예시 제공
- 통합 전에 확인할 핀, 브로커, 동작 규칙 정리

핵심 자료:

- `docs/tempservo_led_node_c_integration.md`
- `docs/mqtt_examples.md`
- `docs/node_c_checklist.md`
- `include/tempservo_led_contract.h`

현재 기준 `tempservo_led.c` 특성:

- DHT22 온습도 센서 읽기
- 습도 기준 자동 서보 제어
- 자동/수동 LED 제어
- LCD 상태 표시
- MQTT 텍스트/JSON 동시 지원

브로커 기본값:

- Host: `163.152.213.101`
- Port: `1883`

제어 토픽:

- `/tempservo_led/servo`
- `/tempservo_led/led`

센서/상태 토픽:

- `/tempservo_led/temperature`
- `/tempservo_led/humidity`
- `/tempservo_led/sensor`
- `/tempservo_led/sensor/json`
- `/tempservo_led/servo/state`
- `/tempservo_led/servo/state/json`
- `/tempservo_led/led/state`
- `/tempservo_led/led/state/json`
- `/tempservo_led/status`
- `/tempservo_led/status/json`
- `/tempservo_led/online`
