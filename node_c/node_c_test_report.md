# Node C Test Report

- Test time: `2026-03-31 10:22:28 KST`
- Test branch/workspace: `gwiin`, `/home/asd/hrd_first_project`
- Tester: Codex local execution

## 1. Test Goal

`node_c` 중앙관리노드 코드가 현재 로컬 환경에서 정상 동작 가능한 상태인지 확인했다.

이번 테스트 범위:

- 중앙 제어 로직 호스트 실행
- Pico 2 W 펌웨어 빌드
- 로컬 MQTT 브로커 동작 확인

실보드 없이 진행했기 때문에 `실제 Pico 2 W에서 Wi-Fi 연결 후 MQTT 송수신` 자체는 이번 보고서 범위에 포함되지 않는다.

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

## 3. Final Result

현재 로컬에서 검증 가능한 범위 기준으로 `node_c`는 정상 동작 상태로 판단했다.

확인 완료:

- 중앙 제어 로직 정상
- AUTO/MANUAL 전환 정상
- 환경 데이터 파싱 정상
- 제어 명령 생성 정상
- 상태 발행 정상
- Pico 2 W 대상 펌웨어 빌드 정상
- 로컬 MQTT 브로커 동작 정상

## 4. Remaining Real-device Test

실보드에서 최종 확인이 필요한 항목:

- Pico 2 W가 `bindsoft` Wi-Fi에 실제 연결되는지
- `163.152.213.111:1883` 브로커에 실제 접속되는지
- PC에서 `mosquitto_sub -h 163.152.213.111 -t "house/#" -v`로 토픽이 보이는지
- 실시간 센서값 또는 수동 명령에 맞춰 MQTT 토픽이 기대대로 올라오는지

## 5. Conclusion

`node_c`는 현재 단계에서 혼자 개발 및 단독 검증이 가능한 상태이며, 로컬 테스트 기준으로는 이상 없이 동작했다.
