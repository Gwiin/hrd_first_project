# Node C Central Manager

`node_c`는 스마트 환경제어 하우스의 중앙관리노드 초안 구현이다.

현재 포함된 기능:

- `AUTO` / `MANUAL` 모드 관리
- `house/env` payload 파싱
- 문서 기준 임계값 자동 제어
- 중복 명령 방지
- UART 수동 명령 처리
- `nodeA`, `nodeB` timeout 경고
- `house/status/nodeC` 상태 스냅샷 발행
- Pico W Wi-Fi / MQTT 연동용 네트워크 계층 골격

## 폴더 구조

```txt
node_c/
├── CMakeLists.txt
├── lwipopts.h
├── include/
│   ├── node_c_config.h
│   ├── node_c_controller.h
│   └── node_c_network.h
└── src/
    ├── main.c
    ├── node_c_controller.c
    └── node_c_network.c
```

## 현재 동작 방식

Wi-Fi 정보와 MQTT 서버 주소가 설정되지 않으면 자동으로 `node_c_config.h` 기본값을 사용한다.
기본값도 비어 있거나 사용할 수 없는 값이면 `demo mode`로 실행된다.
이 경우 MQTT 발행 자리에 UART 로그를 연결해 두고, 데모 환경값으로 중앙 로직을 검증한다.

가장 쉬운 사용법은 [node_c_config.h](/home/asd/hrd_first_project/node_c/include/node_c_config.h) 를 수정하는 것이다.
필요하면 `WIFI_SSID`, `WIFI_PASSWORD`, `MQTT_SERVER` 환경변수로 빌드 시 덮어쓸 수도 있다.

`main.c`는 데모 환경값을 주기적으로 넣어 자동 제어를 검증한다.

예시 데모 흐름:

- 어두우면 `house/cmd/light` 에 `ON`
- 밝아지면 `house/cmd/light` 에 `OFF`
- 덥거나 습하면 `house/cmd/window` 에 `OPEN`
- 정상 범위면 `house/cmd/window` 에 `CLOSE`

## UART 명령

아래 명령을 USB 시리얼로 입력할 수 있다.

```txt
mode manual
mode auto
light on
light off
window open
window close
```

수동 장치 명령은 `MANUAL` 모드에서만 적용된다.

## MQTT 연동 시 연결할 지점

현재 구현 기준 실제 MQTT 연결 흐름은 아래와 같다.

1. `house/env` 수신 시 `node_c_controller_apply_env_payload()`
2. `house/mode` 수신 시 `AUTO` / `MANUAL` 모드 전환
3. `house/status/nodeB` 수신 시 `node_c_controller_apply_node_b_status()`
4. 시리얼 또는 버튼 입력 시 `node_c_controller_handle_uart_command()`
5. 주기 루프에서 `node_c_controller_periodic()`
6. `house/heartbeat/nodeC` 는 2초 주기로 발행

구독 토픽:

- `house/env`
- `house/mode`
- `house/status/nodeB`
- `house/heartbeat/nodeA`
- `house/heartbeat/nodeB`

발행 토픽:

- `house/mode`
- `house/cmd/light`
- `house/cmd/window`
- `house/status/nodeC`
- `house/heartbeat/nodeC`

## 빌드 예시

`PICO_SDK_PATH`가 설정되어 있다는 가정이다.

헤더 설정 방식:

```c
// node_c/include/node_c_config.h
#define NODE_C_DEFAULT_WIFI_SSID "bindsoft"
#define NODE_C_DEFAULT_WIFI_PASSWORD "bindsoft24"
#define NODE_C_DEFAULT_MQTT_SERVER "163.152.213.111"
```

그 다음 빌드:

```bash
cd node_c
mkdir -p build
cd build
cmake ..
make -j4
```

환경변수로 덮어쓰기:

```bash
export WIFI_SSID=your-ssid
export WIFI_PASSWORD=your-password
export MQTT_SERVER=broker-host-or-ip
cd node_c/build
cmake ..
make -j4
```

선택적으로 MQTT 계정도 덮어쓸 수 있다:

```bash
export MQTT_USERNAME=your-user
export MQTT_PASSWORD=your-pass
```

## 다음 작업 후보

- 버튼 입력으로 모드 전환 추가
- MQTT reconnect / DNS 재시도 안정화
- heartbeat payload에 uptime 또는 상태 코드 추가
- OLED/LCD 상태 출력 추가
