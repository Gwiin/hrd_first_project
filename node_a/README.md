# node_a

`node_a`는 현재 스마트 하우스 통합 구조에서 환경 센서값을 MQTT로 발행하는 센서 노드입니다.

현재 포함된 기능:

- CDS 조도 센서 읽기
- DHT 계열 온습도 센서 읽기
- `house/env` 발행
- `house/heartbeat/nodeA` 발행
- Pico W Wi-Fi / MQTT 기반 실보드 동작

현재 하드웨어 기준:

- DHT data pin: `GP15`
- CDS ADC pin: `GP27` (`ADC1`)

MQTT 계약:

- 발행
  - `house/env`
  - `house/heartbeat/nodeA`

환경 payload 형식:

```txt
light=250,temp=29.5,humidity=72.0
```

기본 브로커 설정은 [node_a_config.h](/home/asd/hrd_first_project/node_a/pico/include/node_a_config.h) 에 있으며, 필요하면 `WIFI_SSID`, `WIFI_PASSWORD`, `MQTT_SERVER` 환경변수로 빌드 시 덮어쓸 수 있습니다.

빌드 예시:

```bash
cd node_a/pico
mkdir -p build
cd build
cmake ..
make -j4
```

최종 업로드 파일:

- [node_a.uf2](/home/asd/hrd_first_project/node_a/pico/build/node_a.uf2)

현재 통합 기준 기대 흐름:

- `node_a`가 `house/env` 발행
- `node_c`가 AUTO 규칙 적용
- `node_b`가 `house/cmd/light`, `house/cmd/window`를 받아 액추에이터 반영

관련 파일:

- [project0.c](/home/asd/hrd_first_project/node_a/pico/src/project0.c)
- [CMakeLists.txt](/home/asd/hrd_first_project/node_a/pico/CMakeLists.txt)
- [node_a_config.h](/home/asd/hrd_first_project/node_a/pico/include/node_a_config.h)
- [lwipopts.h](/home/asd/hrd_first_project/node_a/pico/lwipopts.h)
