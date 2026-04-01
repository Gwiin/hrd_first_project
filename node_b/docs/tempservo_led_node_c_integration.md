# tempservo_led - node_c integration

## 1. 목적

`node_c`가 `tempservo_led.c`와 MQTT로 연동할 때 필요한 인터페이스만 정리한 문서입니다.

`node_c`는 아래 역할 중 하나 또는 둘 다를 수행하면 됩니다.

- 센서 상태 수신기
- 원격 제어 송신기

## 2. 브로커 정보

- MQTT broker host: `163.152.213.101`
- MQTT broker port: `1883`
- 인증: 현재 코드 기준 없음

## 3. 하드웨어 정보

`tempservo_led.c` 기준 핀맵:

- DHT22: `GP15`
- SG90 servo: `GP14`
- LED: `GP16`
- I2C SDA: `GP0`
- I2C SCL: `GP1`

## 4. 동작 규칙

- 습도 `70.0%` 이상이면 자동 모드에서 서보가 열림
- LED도 자동 모드이면 서보 동작과 같은 기준으로 ON/OFF
- 제어 명령을 받으면 해당 장치는 수동 모드로 전환 가능
- `auto` 명령을 받으면 다시 자동 모드로 복귀

## 5. node_c가 구독하면 좋은 토픽

### 텍스트 토픽

- `/tempservo_led/temperature`
- `/tempservo_led/humidity`
- `/tempservo_led/sensor`
- `/tempservo_led/servo/state`
- `/tempservo_led/led/state`
- `/tempservo_led/status`
- `/tempservo_led/online`

### JSON 토픽

- `/tempservo_led/sensor/json`
- `/tempservo_led/servo/state/json`
- `/tempservo_led/led/state/json`
- `/tempservo_led/status/json`

## 6. node_c가 발행하는 제어 토픽

- `/tempservo_led/servo`
- `/tempservo_led/led`

## 7. 허용 명령

### 서보 제어

텍스트:

- `auto`
- `open`
- `close`
- `1`
- `0`

JSON:

- `{"mode":"auto"}`
- `{"servo":"open"}`
- `{"servo":"close"}`
- `{"state":"open"}`
- `{"state":"closed"}`

### LED 제어

텍스트:

- `auto`
- `on`
- `off`
- `1`
- `0`

JSON:

- `{"mode":"auto"}`
- `{"led":"on"}`
- `{"led":"off"}`
- `{"state":"on"}`
- `{"state":"off"}`

## 8. 수신 예시

### 센서 JSON

```json
{"temperature":24.7,"humidity":68.2}
```

### 서보 상태 JSON

```json
{"servo":"OPEN"}
```

### LED 상태 JSON

```json
{"led":"OFF"}
```

### 전체 상태 JSON

```json
{"servo":"OPEN","servo_mode":"AUTO","led":"ON","led_mode":"AUTO"}
```

### 상태 메시지 JSON

```json
{"message":"mqtt subscribed"}
```

## 9. 온라인 상태

- 연결 성공 후 `/tempservo_led/online` 에 `1` 발행
- 비정상 종료 시 LWT로 `0`

## 10. node_c 구현 권장 사항

- 텍스트와 JSON 둘 중 하나만 써도 되지만, 신규 구현은 JSON 우선 권장
- 상태 토픽과 센서 토픽은 별도 처리
- `online` 토픽을 보고 장치 연결 여부 판단
- `status/json`은 로그/디버그용으로 활용

## 11. 통합 최소 요구사항

`node_c`가 최소로 처리해야 하는 것:

- `/tempservo_led/sensor/json` 구독
- `/tempservo_led/status/json` 구독
- `/tempservo_led/servo` 또는 `/tempservo_led/led` 발행

## 12. 주의사항

- 토픽 이름은 현재 펌웨어에 하드코딩되어 있으므로 바꾸면 양쪽 동시 수정 필요
- 문자열 상태값은 대문자 기준으로 발행됨: `OPEN`, `CLOSED`, `ON`, `OFF`
- 자동/수동 모드 정보는 `status/json`에서 확인 가능
