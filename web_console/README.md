# Web Console

`Flask + MQTT` 기반 스마트 하우스 관제 콘솔입니다.

현재 포함된 기능:

- `house/#` 전체 구독
- `house/env` 센서값 표시
- `house/status/nodeB`, `house/status/nodeC` 상태 표시
- `house/heartbeat/nodeA`, `house/heartbeat/nodeB`, `house/heartbeat/nodeC` 연결 상태 표시
- 최근 MQTT 송수신 로그 표시
- 웹 버튼으로 액추에이터/모드 명령 publish

## 폴더 구조

```txt
web_console/
├── app.py
├── requirements.txt
├── README.md
├── static/
│   ├── app.js
│   └── style.css
└── templates/
    └── index.html
```

## 실행 방법

```bash
cd web_console
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python3 app.py
```

브라우저에서 아래 주소로 접속:

```txt
http://127.0.0.1:5000
```

## 환경변수

기본 브로커는 현재 프로젝트 테스트에 맞춰 `163.152.213.111:1883` 입니다.

필요하면 아래 값으로 변경할 수 있습니다.

```bash
export MQTT_BROKER_HOST=163.152.213.111
export MQTT_BROKER_PORT=1883
export MQTT_USERNAME=your-user
export MQTT_PASSWORD=your-pass
export MQTT_SUBSCRIBE_TOPIC="house/#"
export HEARTBEAT_TIMEOUT_S=6
export WEB_CONSOLE_PORT=5000
```

## 지원 명령

버튼 클릭 시 아래 토픽으로 publish 합니다.

- `Light On` -> `house/cmd/light` / `ON`
- `Light Off` -> `house/cmd/light` / `OFF`
- `Window Open` -> `house/cmd/window` / `OPEN`
- `Window Close` -> `house/cmd/window` / `CLOSE`
- `Mode Auto` -> `house/mode` / `AUTO`
- `Mode Manual` -> `house/mode` / `MANUAL`

## 참고

- 현재 웹 콘솔은 `house/mode`, `house/cmd/light`, `house/cmd/window`를 publish 할 수 있다.
- `node_c`는 `house/mode`를 구독해서 `AUTO` / `MANUAL` 전환을 처리하도록 맞춰져 있다.
