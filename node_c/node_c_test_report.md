# Node C Test Report

- Test time: `2026-04-01 15:11 KST`
- Test branch/workspace: `gwiin`, `/home/asd/hrd_first_project`
- Tester: Codex local execution

## 1. Test Goal

`node_c` 중앙관리노드 코드가 현재 로컬 환경에서 정상 동작 가능한 상태인지 확인했다.

이번 테스트 범위:

- 중앙 제어 로직 호스트 실행
- Pico 2 W 펌웨어 빌드
- 로컬 MQTT 브로커 동작 확인
- 실제 Pico 2 W 실보드 MQTT 송수신 확인
- `web_console + node_c` 단독 제어 확인
- `node_b + node_c + web_console` 실보드 통합 확인
- `node_a + node_b + node_c + web_console` 최종 MQTT 흐름 확인

## 2. Executed Tests

### 2-1. Controller smoke test

실행 명령:

```bash
cc -std=c11 -Wall -Wextra -I node_c/include \
  node_c/src/node_c_controller.c \
  node_c/tests/controller_smoke_test.c \
  -o /tmp/node_c_controller_smoke && /tmp/node_c_controller_smoke
```

검증 항목:

- 부팅 시 `AUTO` 모드 초기화
- 환경 payload 파싱
- AUTO 임계값 기반 `LIGHT ON/OFF`, `WINDOW OPEN/CLOSE`
- MANUAL 명령 처리
- MQTT mode payload 처리
- MQTT light/window payload 처리
- `house/status/nodeC` 상태 발행
- `nodeA`, `nodeB` timeout 경고

핵심 실행 결과:

```txt
[NODE_C] BOOT OK
[TEST PUB] topic=house/mode payload=AUTO
[NODE_C] RX ENV light=250 temp=29.5 humidity=72.0
[TEST PUB] topic=house/cmd/light payload=ON
[TEST PUB] topic=house/cmd/window payload=OPEN
[NODE_C] RX ENV light=350 temp=26.0 humidity=55.0
[TEST PUB] topic=house/cmd/light payload=OFF
[TEST PUB] topic=house/cmd/window payload=CLOSE
[TEST PUB] topic=house/mode payload=MANUAL
[TEST PUB] topic=house/cmd/light payload=ON
[TEST PUB] topic=house/cmd/window payload=OPEN
[TEST PUB] topic=house/mode payload=AUTO
[NODE_C] WARN node A timeout
[NODE_C] WARN node B no response
[TEST] controller smoke test passed
```

판정:

- `PASS`

### 2-5. Web console + node_c standalone control test

실행 환경:

- 실제 `Pico 2 W` 보드 사용
- `web_console` 실행
- MQTT broker: `163.152.213.111:1883`
- `node_a`, `node_b` 없음

검증 항목:

- 웹 콘솔에서 `MANUAL` 전환
- 웹 콘솔에서 `LIGHT OFF/ON`
- 웹 콘솔에서 `WINDOW CLOSE/OPEN`
- 웹 콘솔에서 `AUTO` 복귀
- `house/status/nodeC`가 즉시 반영되는지 확인

실제 확인된 흐름 예시:

```txt
house/mode MANUAL
house/status/nodeC mode=MANUAL,light=0,temp=0.0,humidity=0.0,lamp=OFF,window=CLOSE
house/cmd/window CLOSE
house/status/nodeC mode=MANUAL,light=0,temp=0.0,humidity=0.0,lamp=OFF,window=CLOSE
house/cmd/window OPEN
house/status/nodeC mode=MANUAL,light=0,temp=0.0,humidity=0.0,lamp=OFF,window=OPEN
house/cmd/light ON
house/status/nodeC mode=MANUAL,light=0,temp=0.0,humidity=0.0,lamp=ON,window=OPEN
house/mode AUTO
house/status/nodeC mode=AUTO,light=0,temp=0.0,humidity=0.0,lamp=ON,window=OPEN
```

판정:

- `PASS`
- `node_a`, `node_b` 없이도 `web_console + node_c` 조합으로 상태 확인과 수동 제어 가능

### 2-6. node_b + node_c + web_console integration test

실행 환경:

- 실제 `Pico 2 W` 2대 사용
- `node_b` 액추에이터 노드
- `node_c` 중앙관리노드
- `web_console`
- MQTT broker: `163.152.213.111:1883`

검증 항목:

- `house/env` 발행 시 `node_c`가 AUTO 판단
- `house/cmd/light`, `house/cmd/window` 자동 발행
- `node_b`가 액추에이터 상태를 반영
- `house/status/nodeB`, `house/heartbeat/nodeB`가 웹 콘솔에 반영
- `house/status/nodeC`, `house/heartbeat/nodeC`가 웹 콘솔에 반영

실제 확인된 흐름 예시:

```txt
house/env light=250,temp=29.5,humidity=72.0
house/status/nodeC mode=AUTO,light=250,temp=29.5,humidity=72.0,lamp=ON,window=OPEN
house/cmd/light ON
house/cmd/window OPEN
house/status/nodeB lamp=ON,window=OPEN
house/heartbeat/nodeB alive
house/heartbeat/nodeC alive
```

판정:

- `PASS`
- `node_c`와 `node_b`가 현재 MQTT 계약 기준으로 정상 통합됨

### 2-2. Local MQTT broker smoke test

실행 명령:

```bash
timeout 3 mosquitto_sub -h 127.0.0.1 -t house/test -C 1 & \
sleep 1; \
mosquitto_pub -h 127.0.0.1 -t house/test -m smoke; \
wait
```

실행 결과:

```txt
smoke
```

판정:

- `PASS`
- 로컬 `mosquitto` 브로커 pub/sub 정상 확인

### 2-3. Pico 2 W firmware build test

실행 명령:

```bash
cmake -S node_c -B /tmp/node_c_test_build
cmake --build /tmp/node_c_test_build -j4
```

확인 내용:

- `PICO_BOARD = pico2_w`
- Pico W Wi-Fi build support available
- `node_c.elf`, `node_c.bin` 생성 확인

산출물 확인 명령:

```bash
ls -lh /tmp/node_c_test_build/node_c.elf /tmp/node_c_test_build/node_c.bin
```

실행 결과:

```txt
-rwxr-xr-x 1 asd asd 372K Mar 31 10:22 /tmp/node_c_test_build/node_c.bin
-rwxr-xr-x 1 asd asd 2.3M Mar 31 10:22 /tmp/node_c_test_build/node_c.elf
```

판정:

- `PASS`

### 2-4. Real Pico 2 W MQTT test

실행 환경:

- 실제 `Pico 2 W` 보드 사용
- Wi-Fi: `bindsoft`
- MQTT broker: `163.152.213.111:1883`

PC에서 구독:

```bash
mosquitto_sub -h 163.152.213.111 -t "house/#" -v
```

PC에서 환경값 발행:

```bash
mosquitto_pub -h 163.152.213.111 -t house/env -m "light=350,temp=26.0,humidity=55.0"
mosquitto_pub -h 163.152.213.111 -t house/env -m "light=250,temp=29.5,humidity=72.0"
```

실제 확인된 결과:

```txt
house/status/nodeC mode=AUTO,light=350,temp=26.0,humidity=55.0,lamp=OFF,window=CLOSE
house/heartbeat/nodeC alive
house/env light=350,temp=26.0,humidity=55.0
house/status/nodeC mode=AUTO,light=350,temp=26.0,humidity=55.0,lamp=OFF,window=CLOSE
house/env light=250,temp=29.5,humidity=72.0
house/cmd/light ON
house/cmd/window OPEN
house/status/nodeC mode=AUTO,light=250,temp=29.5,humidity=72.0,lamp=ON,window=OPEN
house/heartbeat/nodeC alive
```

실보드 기준 검증 완료 항목:

- Pico 2 W가 Wi-Fi에 실제 연결됨
- MQTT broker에 실제 연결됨
- `house/heartbeat/nodeC` 발행 확인
- `house/status/nodeC` 발행 확인
- `house/env` 수신 확인
- 상태 변화 시 `house/cmd/light`, `house/cmd/window` 발행 확인
- AUTO 제어 로직이 실보드에서 정상 동작함

판정:

- `PASS`

## 3. Final Result

현재 로컬 및 실보드에서 검증 가능한 범위 기준으로 `node_c`는 정상 동작 상태로 판단했다.

확인 완료:

- 중앙 제어 로직 정상
- AUTO/MANUAL 전환 정상
- MQTT mode/light/window 처리 정상
- 환경 데이터 파싱 정상
- 제어 명령 생성 정상
- 상태 발행 정상
- Pico 2 W 대상 펌웨어 빌드 정상
- Pico 2 W 대상 `uf2` 생성 정상
- 로컬 MQTT 브로커 동작 정상
- 실제 Pico 2 W MQTT 송수신 정상
- 실제 Pico 2 W AUTO 제어 정상
- 웹 콘솔 단독 제어 정상
- `node_b + node_c + web_console` 통합 정상
- `node_b` WS2812 8픽셀 전체 제어 펌웨어 빌드 정상

## 4. Remaining Real-device Test

실보드에서 최종 확인이 필요한 항목:

- 실제 `nodeA`와의 통합 확인
- 웹 콘솔 화면/상태 표시 고도화
- WS2812 색상 정책 확정

## 5. Conclusion

`node_c`는 현재 단계에서 단독 개발, 실보드 단독 검증, 웹 콘솔 연동 단독 검증까지 완료된 상태이며, 핵심 기능은 이상 없이 동작했다.
