# node_c checklist

- MQTT broker `163.152.213.101:1883` 연결 확인
- `/tempservo_led/online` 구독 확인
- `/tempservo_led/sensor/json` 수신 확인
- `/tempservo_led/status/json` 수신 확인
- 서보 제어 토픽 `/tempservo_led/servo` 발행 확인
- LED 제어 토픽 `/tempservo_led/led` 발행 확인
- `auto` 복귀 명령 처리 확인
- 텍스트 명령과 JSON 명령 중 사용할 형식 확정
- 상태값 대문자 문자열 처리 확인: `OPEN`, `CLOSED`, `ON`, `OFF`
- 장애 시 `online=0` 처리 정책 정리
