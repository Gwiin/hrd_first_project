# Node C Handover Log

- 작성일시: `2026-04-01`
- 작성 목적: 다음 대화 또는 다음 작업 세션에서 `node_c` 개발을 끊김 없이 이어가기 위한 인수인계 문서
- 현재 브랜치 기준: `gwiin`

## 1. 현재까지 진행한 작업 요약

`node_c` 중앙관리노드 구현, 웹 콘솔 연동, 실보드 단독 검증, `web_console + node_c` 단독 제어 검증, `node_b` 실보드 액추에이터 통합 검증까지 완료했다.

완료된 항목:

- `AUTO / MANUAL` 모드 관리 구현
- 환경 데이터 payload 파싱 구현
- 문서 기준 임계값 자동 제어 구현
- 중복 명령 방지 구현
- UART 수동 명령 처리 구현
- `nodeA`, `nodeB` timeout 경고 구현
- `house/status/nodeC` 상태 발행 구현
- `house/heartbeat/nodeC` heartbeat 발행 구현
- Pico 2 W + Wi-Fi/MQTT 계층 구현
- `config.h` 기반 팀 공용 설정 방식 추가
- MQTT `house/mode` 수신 처리 구현
- MQTT `house/cmd/light`, `house/cmd/window` 수신 처리 구현
- `web_console` Flask + MQTT 상태판 구현
- `web_console + node_c` 단독 수동 제어 검증
- Pico 2 W 대상 `uf2` 생성 정리
- 로컬 스모크 테스트 문서화 완료
- 실보드 MQTT 단독 테스트 완료
- 실보드 AUTO 제어 테스트 완료
- `node_b` MQTT 액추에이터 노드 통합 완료
- `web_console + node_c + node_b` 통합 테스트 완료
- `house/env` 임의 환경값 기준 AUTO 명령 전파 확인

## 2. 주요 파일 구조

핵심 파일:

- [node_c/CMakeLists.txt](/home/asd/hrd_first_project/node_c/CMakeLists.txt)
- [node_c/include/node_c_config.h](/home/asd/hrd_first_project/node_c/include/node_c_config.h)
- [node_c/include/node_c_controller.h](/home/asd/hrd_first_project/node_c/include/node_c_controller.h)
- [node_c/include/node_c_network.h](/home/asd/hrd_first_project/node_c/include/node_c_network.h)
- [node_c/src/node_c_controller.c](/home/asd/hrd_first_project/node_c/src/node_c_controller.c)
- [node_c/src/node_c_network.c](/home/asd/hrd_first_project/node_c/src/node_c_network.c)
- [node_c/src/main.c](/home/asd/hrd_first_project/node_c/src/main.c)
- [node_c/tests/controller_smoke_test.c](/home/asd/hrd_first_project/node_c/tests/controller_smoke_test.c)
- [node_c/node_c_test_report.md](/home/asd/hrd_first_project/node_c/node_c_test_report.md)
- [web_console/README.md](/home/asd/hrd_first_project/web_console/README.md)
- [web_console/app.py](/home/asd/hrd_first_project/web_console/app.py)

## 3. 구현 상세

### 3-1. 중앙 제어 로직

파일:

- [node_c/src/node_c_controller.c](/home/asd/hrd_first_project/node_c/src/node_c_controller.c)

구현된 기능:

- 초기 모드 `AUTO`
- 초기 상태 `lamp=OFF`, `window=CLOSE`
- 센서 payload 형식 파싱
  - `light=275,temp=29.1,humidity=72.0`
- AUTO 제어 규칙
  - `light < 280` -> 조명 `ON`
  - `light > 320` -> 조명 `OFF`
  - `temp > 28` 또는 `humidity > 70` -> 창문 `OPEN`
  - `temp <= 27` 그리고 `humidity <= 65` -> 창문 `CLOSE`
- 상태가 바뀔 때만 명령 발행
- MANUAL 명령 처리
  - `mode manual`
  - `mode auto`
  - `light on`
  - `light off`
  - `window open`
  - `window close`
- MQTT 수동 제어 처리
  - `house/mode`
  - `house/cmd/light`
  - `house/cmd/window`
- `MANUAL` 모드일 때만 MQTT 수동 명령을 상태에 반영
- `AUTO` 모드에서는 MQTT 수동 `light/window` 명령 무시
- 상태 로그 및 상태 스냅샷 발행
- `nodeA`, `nodeB` timeout 경고

### 3-2. 네트워크 계층

파일:

- [node_c/src/node_c_network.c](/home/asd/hrd_first_project/node_c/src/node_c_network.c)

구현된 기능:

- Pico W `cyw43` 초기화
- STA 모드 전환
- Wi-Fi 연결 시도
- MQTT broker DNS lookup
- MQTT client connect
- 구독 토픽 등록
- 수신 토픽에 따라 controller 로직 호출
- `house/heartbeat/nodeC` 주기 발행

구독 토픽:

- `house/env`
- `house/mode`
- `house/cmd/light`
- `house/cmd/window`
- `house/status/nodeB`
- `house/heartbeat/nodeA`
- `house/heartbeat/nodeB`

발행 토픽:

- `house/mode`
- `house/cmd/light`
- `house/cmd/window`
- `house/status/nodeC`
- `house/heartbeat/nodeC`

### 3-2-1. 현재 node_b 통합 상태

현재 `node_b`는 아래 역할로 통합되었다.

- `house/cmd/light` 수신
- `house/cmd/window` 수신
- 서보 `GP14` 제어
- `WS2812 RGB Strip` 데이터 핀 `GP16` 제어
- `house/status/nodeB` 발행
- `house/heartbeat/nodeB` 발행

현재 `lamp ON` 상태는 `WS2812` 8픽셀 전체 흰색 점등으로 매핑되어 있다.

### 3-3. 설정 방식

파일:

- [node_c/include/node_c_config.h](/home/asd/hrd_first_project/node_c/include/node_c_config.h)

현재 기본값:

```c
#define NODE_C_DEFAULT_WIFI_SSID "bindsoft"
#define NODE_C_DEFAULT_WIFI_PASSWORD "bindsoft24"
#define NODE_C_DEFAULT_MQTT_SERVER "163.152.213.111"
```

추가 사항:

- 기본은 `node_c_config.h` 값 사용
- 필요하면 CMake 환경변수로 덮어쓰기 가능
- MQTT 계정도 헤더/환경변수 둘 다 지원
- `picotool` 기반 `uf2` 생성 가능하도록 정리

### 3-4. 웹 콘솔

파일:

- [web_console/app.py](/home/asd/hrd_first_project/web_console/app.py)
- [web_console/templates/index.html](/home/asd/hrd_first_project/web_console/templates/index.html)
- [web_console/static/app.js](/home/asd/hrd_first_project/web_console/static/app.js)
- [web_console/static/style.css](/home/asd/hrd_first_project/web_console/static/style.css)

구현된 기능:

- `house/#` 전체 구독
- `house/env` 상태 표시
- `house/status/nodeC` 상태 표시
- `house/heartbeat/nodeC` 상태 표시
- 웹 버튼으로 `house/mode`, `house/cmd/light`, `house/cmd/window` publish
- `node_a`, `node_b` 없이도 `node_c` 단독 제어 데모 가능
- `node_b`, `node_c` 통합 시 `Controller`와 `Actuator` 상태를 한 화면에서 동시 확인 가능

## 4. 지금까지 실행한 테스트

### 4-1. 중앙 로직 호스트 스모크 테스트

파일:

- [node_c/tests/controller_smoke_test.c](/home/asd/hrd_first_project/node_c/tests/controller_smoke_test.c)

검증 완료 항목:

- AUTO 초기화
- 환경 payload 파싱
- AUTO 제어 명령 발행
- MANUAL 명령 처리
- MQTT mode payload 처리
- MQTT light/window payload 처리
- 상태 발행
- timeout 경고

결과:

- `PASS`

### 4-2. 로컬 MQTT 브로커 테스트

검증 완료 항목:

- `mosquitto_sub`
- `mosquitto_pub`
- 동일 토픽 pub/sub 왕복

결과:

- `PASS`

### 4-3. Pico 2 W 펌웨어 빌드 테스트

검증 완료 항목:

- `PICO_BOARD = pico2_w`
- `node_c.elf` 생성
- `node_c.bin` 생성

결과:

- `PASS`

### 4-4. 실제 Pico 2 W 단독 테스트

검증 완료 항목:

- Wi-Fi 실제 연결
- MQTT broker 실제 연결
- `house/heartbeat/nodeC` 발행
- `house/status/nodeC` 발행
- `house/env` 수신
- 상태 변화 시 `house/cmd/light`, `house/cmd/window` 발행
- AUTO 제어 로직 실보드 검증
- `web_console`에서 `MANUAL` 모드 전환 검증
- `web_console`에서 `LIGHT/WINDOW` 수동 제어 검증
- `node_b` heartbeat 수신 및 상태 반영 확인
- `node_c` AUTO 명령을 `node_b`가 실제 반영하는 통합 흐름 확인

결과:

- `PASS`

### 4-5. node_b + node_c + web_console integration test

실행 환경:

- 실제 `Pico 2 W` 2대 사용
- `node_b` 액추에이터 노드
- `node_c` 중앙관리노드
- `web_console`
- MQTT broker: `163.152.213.111:1883`

실제 확인된 핵심 흐름:

```txt
house/env light=250,temp=29.5,humidity=72.0
house/status/nodeC mode=AUTO,light=250,temp=29.5,humidity=72.0,lamp=ON,window=OPEN
house/cmd/light ON
house/cmd/window OPEN
house/status/nodeB lamp=ON,window=OPEN
house/heartbeat/nodeB alive
house/heartbeat/nodeC alive
```

결과:

- `PASS`
- `node_c` AUTO 판단과 `node_b` 액추에이터 반영이 MQTT를 통해 정상 연동됨

상세는 [node_c/node_c_test_report.md](/home/asd/hrd_first_project/node_c/node_c_test_report.md) 참고

## 5. 아직 남은 작업

이제 남은 작업은 주로 안정화와 화면/상태 고도화다.

### 5-1. 실보드 업로드 및 연결 확인

확인 필요:

- MQTT 연결 실패 시 재시도 안정화
- USB 시리얼 로그 포맷 최종 점검

### 5-2. 웹 콘솔 고도화

확인/개선 후보:

- 상태 카드에 `waiting env data` 표현 강화
- `AUTO` / `MANUAL` 상태 강조 표시
- `nodeA`, `nodeB` 미연결 상태 안내 문구 정리

### 5-3. 단독 테스트

보드가 `node_c` 하나만 있어도 가능한 테스트:

- UART 수동 명령 입력
- 웹 콘솔에서 `mode/light/window` 단독 제어
- 외부 MQTT publish로 AUTO 동작 재확인

### 5-4. 이후 개선 후보

아직 미구현 또는 개선 가능 항목:

- MQTT reconnect / DNS 재시도 안정화
- 버튼 입력으로 모드 전환
- heartbeat payload에 uptime 추가
- 상태 출력 포맷 고도화
- WS2812 색상 정책 고도화
- 실제 `nodeA` 통합
- OLED/LCD 상태 출력

## 6. 내일 보드 도착 후 추천 작업 순서

1. 웹 콘솔과 `node_c` 단독 데모 안정화
2. `nodeA`와 연결해서 실제 센서값 연동 테스트
3. `nodeB`와 연결해서 실제 액추에이터 명령 연동 테스트
4. 세 노드 통합 테스트
5. 필요하면 로그/재연결 처리 보완

## 7. 주의할 점

- `MQTT_SERVER`는 반드시 Pico가 접근 가능한 PC의 실제 IPv4여야 한다.
- 현재 사용 예정 IP는 `163.152.213.111`
- Windows 방화벽에서 `1883` inbound 허용 필요
- `mosquitto` 브로커가 실행 중이어야 함
- 실보드 단독 검증과 웹 콘솔 단독 제어는 완료했지만 `nodeA`, `nodeB` 통합 검증은 아직 남아 있다

## 8. 한 줄 결론

`node_c`는 현재 `web_console + node_c` 단독 기준으로 상태 확인과 수동 제어까지 가능한 상태이며, 다음 핵심은 `nodeA`, `nodeB` 통합 검증이다.
