# node_b

`node_b`는 현재 `node_c`가 발행한 제어 명령을 실제 액추에이터에 반영하는 MQTT 액추에이터 노드입니다.

현재 포함된 기능:

- `house/cmd/light` 구독 후 LED ON/OFF
- `house/cmd/window` 구독 후 서보 OPEN/CLOSE
- `house/status/nodeB` 상태 스냅샷 발행
- `house/heartbeat/nodeB` heartbeat 발행
- Pico W Wi-Fi / MQTT 기반 실보드 동작
- `WS2812 RGB Strip` 8픽셀 전체 ON/OFF 제어

현재 하드웨어 기준:

- SG90 servo: `GP14`
- WS2812 data pin: `GP16`

현재 검증된 통합 흐름:

- `node_a`가 `house/env` 발행
- `node_c`가 조도/온습도 기준 AUTO 판단
- 웹 콘솔에서 `Light On/Off`, `Window Open/Close` 발행
- `node_b`가 `house/cmd/light`, `house/cmd/window` 수신
- `house/status/nodeB`와 `house/heartbeat/nodeB`가 웹 콘솔에 반영
- `node_c`와 함께 사용할 때 `house/env` 기준 AUTO 명령을 받아 액추에이터 반영

MQTT 계약:

- 구독
  - `house/cmd/light`
  - `house/cmd/window`
- 발행
  - `house/status/nodeB`
  - `house/heartbeat/nodeB`

상태 payload 형식:

```txt
lamp=ON,window=OPEN
lamp=OFF,window=CLOSE
```

WS2812 메모:

- 현재 `lamp ON`이면 스트립 8픽셀 전체를 흰색으로 점등
- 현재 `lamp OFF`이면 스트립 8픽셀 전체를 소등
- `WS2812`는 단순 `gpio_put()`로 제어되지 않으므로 PIO 기반 신호 생성 방식으로 수정 완료
- 배선 시 `DIN -> GP16`, `VCC -> 5V`, `GND -> 공통 GND` 연결 필요

기본 브로커 설정은 [node_b_config.h](/home/asd/hrd_first_project/node_b/include/node_b_config.h) 에 있으며, 필요하면 `WIFI_SSID`, `WIFI_PASSWORD`, `MQTT_SERVER` 환경변수로 빌드 시 덮어쓸 수 있습니다.

빌드 예시:

```bash
cd node_b
mkdir -p build
cd build
cmake ..
make -j4
```

최종 업로드 파일:

- [node_b.uf2](/home/asd/hrd_first_project/node_b/build/node_b.uf2)

관련 파일:

- [tempservo_led.c](/home/asd/hrd_first_project/node_b/src/tempservo_led.c)
- [tempservo_led_contract.h](/home/asd/hrd_first_project/node_b/include/tempservo_led_contract.h)
- [node_b_config.h](/home/asd/hrd_first_project/node_b/include/node_b_config.h)
- [lwipopts.h](/home/asd/hrd_first_project/node_b/lwipopts.h)
