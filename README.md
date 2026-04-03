# hrd_first_project
고려대 세종 HRD 개발자 교육 1차 프로젝트 Repo.

## 현재 기준 요약

현재 저장소 기준으로 바로 이어서 보기 좋은 핵심 상태는 아래와 같다.

- `node_a` MQTT 센서 노드 구현 포함
- `node_b` MQTT 액추에이터 노드 구현 포함
- `node_c` 중앙관리노드 구현 및 MQTT 연동 포함
- `web_console` Flask + MQTT 기반 웹 콘솔 포함
- `node_a`, `node_b` 없이도 `web_console + node_c` 조합으로 단독 상태 확인 및 수동 제어 가능
- `node_b + node_c + web_console` 통합 테스트까지 진행
- `node_a + node_c + node_b + web_console` 전체 MQTT 흐름 확인
- `house/env` 실센서값 기준으로 `node_c`가 자동 판단하고 `node_b` 액추에이터가 반응하는 흐름 확인

주요 참고 위치:

- [node_a/README.md](/home/asd/hrd_first_project/node_a/README.md)
- [node_b/README.md](/home/asd/hrd_first_project/node_b/README.md)
- [node_c/README.md](/home/asd/hrd_first_project/node_c/README.md)
- [node_c/node_c_handover_log.md](/home/asd/hrd_first_project/node_c/node_c_handover_log.md)
- [node_c/node_c_test_report.md](/home/asd/hrd_first_project/node_c/node_c_test_report.md)
- [web_console/README.md](/home/asd/hrd_first_project/web_console/README.md)

## 각 노드 펌웨어 UF2 위치

보드 업로드 시 사용할 현재 `uf2` 파일 위치는 아래와 같다.

- `node_a`: [node_a/pico/build/node_a.uf2](/home/asd/hrd_first_project/node_a/pico/build/node_a.uf2)
- `node_b`: [node_b/build/node_b.uf2](/home/asd/hrd_first_project/node_b/build/node_b.uf2)
- `node_c`: [node_c/build/node_c.uf2](/home/asd/hrd_first_project/node_c/build/node_c.uf2)

현재 하드웨어/통합 메모:

- `node_b`는 `GP14` 서보모터, `GP16` 액추에이터 출력 사용
- `node_a`는 `GP15` DHT, `GP27` CDS 사용
- `GP16` 연결 장치가 일반 LED가 아니라 `WS2812 RGB Strip`인 경우 단순 GPIO 제어로는 동작하지 않음
- 현재 `node_b`는 `WS2812` 8픽셀 전체를 `lamp ON/OFF` 상태에 맞춰 제어하도록 수정됨
- `node_a`는 조도 ADC raw 값을 `0~400` 범위로 정규화해서 `house/env`에 발행하도록 수정됨
- 현재 `node_b` 창문 제어는 싱글 서보 기반으로 동작하며, `house/cmd/window` 명령에 따라 창문 개폐를 반영함

## 웹 콘솔 및 MQTT 확인 명령

루트 저장소에서 바로 따라할 수 있는 최소 실행 순서는 아래와 같다.

웹 콘솔 실행:

```bash
cd /home/asd/hrd_first_project/web_console
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python3 app.py
```

브라우저 접속:

```txt
http://127.0.0.1:5000
```

MQTT 전체 수신 확인:

```bash
mosquitto_sub -h 163.152.213.111 -p 1883 -t 'house/#' -v
```

필요하면 실행 전에 아래 환경변수로 브로커 정보를 덮어쓸 수 있다.

```bash
export MQTT_BROKER_HOST=163.152.213.111
export MQTT_BROKER_PORT=1883
export MQTT_USERNAME=your-user
export MQTT_PASSWORD=your-pass
export MQTT_SUBSCRIBE_TOPIC='house/#'
export WEB_CONSOLE_PORT=5000
```

## 1차 프로젝트 개발 일지
|일자|요일|시간|인원|내용|
|---|---|---|---|---|
|3/30|월|오전|전체|아이디어 회의|
|||오후|전체|아이디어 선정 및 역할 분담|
||||||
|3/31|화|오전|정귀인|중앙관리노드 코드 작성|
||||황지용|LCD, 액추에이터, 센서 연결 및 실습|
||||김도경|LCD, 센서 연결 및 실습|
|||오후|정귀인|중앙관리노드 MQTT 작성 및 실습|
||||황지용|LCD, 액추에이터, 센서 연결 및 실습|
||||김도경|LCD, 센서 연결 및 실습|
||||||
|4/1|수|오전|정귀인|웹 콘솔 개발 및 테스트|
||||황지용|액추에이터노드 코드 작성|
||||김도경|센서노드 코드 작성|
|||오후|정귀인|`node_c` MQTT/웹 콘솔/통합 테스트 정리|
||||황지용|`node_b` MQTT 액추에이터 노드 통합 및 WS2812 스트립 테스트|
||||김도경|센서노드 작업|
||||||
|4/2|목|오전|정귀인|파일 정리 및 테스트, 프로젝트 기술서 준비|
||||황지용|모형 제작 및 회로 최적화|
||||김도경|모형 제작 및 PPT 초안 작성|
|||오후|정귀인|기술서 및 PPT 자료 제작 및 문서화|
||||황지용|모형 제작 및 실행 테스트|
||||김도경|모형 제작 및 실행 테스트|
|4/3|금|오전|||
|||오후|||

---
