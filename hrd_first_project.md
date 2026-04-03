# hrd_first_project 프로젝트 상세 설명

## 1. 프로젝트 전체 개요

이 프로젝트의 목적은 집 안의 환경을 감지하고, 그 값을 바탕으로 조명과 창문을 자동 또는 수동으로 제어하는 것입니다.

전체 구성은 아래 4개입니다.

- `node_a`  
  환경 센서 노드  
  조도, 온도, 습도 값을 읽어서 MQTT로 발행

- `node_b`  
  액추에이터 노드  
  MQTT 명령을 받아 창문과 조명을 실제로 제어

- `node_c`  
  중앙관리 노드  
  환경값을 보고 자동 제어 판단을 하거나, 수동 명령을 관리

- `web_console`  
  웹 기반 관제 화면  
  현재 상태를 모니터링하고 버튼으로 제어 명령 전송

즉, 역할을 한 줄로 정리하면:

- `node_a`는 **측정**
- `node_c`는 **판단**
- `node_b`는 **실행**
- `web_console`은 **관찰 + 수동제어**

입니다.

---

## 2. 최신 기준 프로젝트의 핵심 특징

최신 README와 코드 기준으로 현재 프로젝트는 단순한 센서 실습이 아니라, **실제 통합형 스마트 홈 제어 시스템 형태**로 정리되어 있습니다.

현재 포함된 핵심 기능은 다음과 같습니다.

### 2-1. 환경 측정
`node_a`가 다음 센서값을 읽습니다.

- CDS 조도 센서
- DHT 계열 온습도 센서

그리고 값을 아래 형식으로 MQTT에 발행합니다.

```txt
light=250,temp=29.5,humidity=72.0
```

여기서 조도는 원래 ADC raw 0~4095 값인데, 중앙 제어 기준에 맞게 **0~400 범위로 정규화**해서 발행합니다.

이 점이 중요합니다.  
왜냐하면 `node_c`의 자동 제어 임계값이 `280`, `320`으로 설정되어 있어서, 센서값과 중앙 제어 로직이 바로 맞물리도록 설계되어 있기 때문입니다.

### 2-2. 자동 제어
`node_c`는 `house/env` 토픽으로 들어온 환경값을 해석해서 자동으로 다음을 판단합니다.

- 어두우면 조명 ON
- 충분히 밝으면 조명 OFF
- 온도 또는 습도가 높으면 창문 OPEN
- 온도와 습도가 정상 범위면 창문 CLOSE

기본 임계값은 코드 기준 아래와 같습니다.

- 조명 ON 기준: `light < 280`
- 조명 OFF 기준: `light > 320`
- 창문 OPEN 기준:
  - `temp > 28.0`
  - 또는 `humidity > 70.0`
- 창문 CLOSE 기준:
  - `temp <= 27.0`
  - 그리고 `humidity <= 65.0`

여기서 조명과 창문 모두 **히스테리시스 구조**가 적용되어 있습니다.  
즉, ON과 OFF 기준이 다르게 잡혀 있어서 경계값 근처에서 너무 자주 깜빡이거나 열렸다 닫혔다 하지 않도록 되어 있습니다.

### 2-3. 수동 제어
수동 제어는 두 방식이 있습니다.

#### 웹 콘솔 버튼
`web_console`에서 버튼을 누르면 다음 명령이 MQTT로 전송됩니다.

- `house/mode` → `AUTO` 또는 `MANUAL`
- `house/cmd/light` → `ON`, `OFF`
- `house/cmd/window` → `OPEN`, `CLOSE`

#### UART 입력
`node_c`는 USB 시리얼(UART) 명령도 직접 받습니다.

지원 명령:

```txt
mode manual
mode auto
light on
light off
window open
window close
```

### 2-4. 상태 모니터링
이 프로젝트는 단순히 명령만 보내는 구조가 아니라, 각 노드의 상태를 다시 수집해서 웹에 보여주는 구조입니다.

#### heartbeat
각 노드는 heartbeat를 보냅니다.

- `house/heartbeat/nodeA`
- `house/heartbeat/nodeB`
- `house/heartbeat/nodeC`

`web_console`은 이 heartbeat를 받아 노드가 살아 있는지 표시합니다.

#### status snapshot
각 노드는 현재 상태를 snapshot 형태로 발행합니다.

- `node_b` → `house/status/nodeB`
- `node_c` → `house/status/nodeC`

예시:

```txt
lamp=ON,window=OPEN
```

또는

```txt
mode=AUTO,light=250,temp=29.5,humidity=72.0,lamp=ON,window=OPEN
```

즉, 웹 콘솔에서는 단순 메시지 로그가 아니라 현재 상태 요약도 볼 수 있습니다.

---

## 3. 각 구성요소 상세 설명

### 3-1. node_a: 센서 노드

`node_a`는 환경 측정 담당입니다.

#### 하드웨어
- DHT data pin: `GP15`
- CDS ADC pin: `GP27` (`ADC1`)

#### 소프트웨어 흐름
1. Wi-Fi 연결
2. MQTT 브로커 DNS 조회
3. MQTT 연결
4. 일정 주기로 heartbeat 발행
5. 일정 주기로 센서 읽기
6. 조도 raw 값을 0~400으로 정규화
7. `house/env` 발행

#### 최신 코드 기준 동작 특징
- heartbeat 주기: `2초`
- 센서 발행 주기: `2초`
- MQTT 연결 후 즉시 heartbeat 발행
- DHT 읽기 실패 시 환경 발행을 건너뜀
- 로그에 raw 조도값과 normalized 값 둘 다 출력

즉, `node_a`는 단순 센서값만 보내는 게 아니라, 중앙 제어가 바로 사용할 수 있는 형태로 데이터를 가공해서 보냅니다.

### 3-2. node_b: 액추에이터 노드

`node_b`는 실제 장치를 움직이는 실행부입니다.

#### 하드웨어
- 서보 모터: `GP14`
- WS2812 RGB Strip: `GP16`

현재 기준으로 창문 제어는 **싱글 서보 1개**를 사용합니다.

#### 조명 제어
- `house/cmd/light` 구독
- `ON`이면 WS2812 8픽셀 전체 흰색 점등
- `OFF`이면 전체 소등

중요한 점:
- 일반 LED처럼 `gpio_put()`로 제어하는 방식이 아니라
- **PIO 기반 WS2812 제어**로 수정되어 있습니다.

즉, 현재는 단순 GPIO LED가 아니라 **RGB LED Strip에 맞춘 구현**입니다.

#### 창문 제어
- `house/cmd/window` 구독
- `OPEN` → 서보 동작
- `CLOSE` → 서보 동작

최신 코드 기준 특징:
- 싱글 서보 사용
- 창문 개폐가 직관적으로 보이도록 각도를 조정해 동작
- 서보 목표 각도를 단계적으로 이동시켜 보다 부드럽게 개폐되도록 설계

#### 상태 보고
- 현재 lamp / window 상태를 `house/status/nodeB`로 발행
- heartbeat를 `house/heartbeat/nodeB`로 발행

즉, `node_b`는 중앙에서 받은 명령을 실제 하드웨어로 반영하고, 그 결과를 다시 시스템에 알려주는 역할입니다.

### 3-3. node_c: 중앙관리 노드

이 프로젝트의 핵심 두뇌입니다.

#### 담당 역할
- `AUTO` / `MANUAL` 모드 관리
- 환경값 파싱
- 임계값 기반 자동 판단
- 수동 명령 처리
- 중복 명령 방지
- `nodeA`, `nodeB` timeout 감시
- 상태 snapshot 발행
- heartbeat 발행

#### 왜 중요한가
이 노드는 단순 중계기가 아닙니다.  
실제로 시스템의 정책을 들고 있는 **컨트롤러**입니다.

예를 들어:

- 센서 데이터가 들어오면 해석하고
- 임계값과 비교해서
- 조명/창문 명령을 발행하고
- 현재 모드와 상태를 관리하고
- 응답 노드가 죽었는지도 감시합니다.

#### 내부 구조
코드가 두 부분으로 나뉩니다.

- `node_c_controller.c`
  - 제어 로직
  - 상태 관리
  - 환경값 파싱
  - AUTO 규칙 적용
  - UART 명령 처리
- `node_c_network.c`
  - Wi-Fi
  - MQTT 연결
  - 토픽 구독/발행
  - heartbeat 송신

즉, 로직과 네트워크가 분리되어 있어 구조가 비교적 깔끔합니다.

#### AUTO / MANUAL 규칙
- `AUTO`에서는 환경값 기반 자동 제어 수행
- `MANUAL`에서는 사용자가 직접 조명/창문 제어 가능

중요한 점:
- `AUTO` 모드에서는 수동 `light/window` MQTT 명령을 무시합니다.
- 이 부분은 코드에서도 `ensure_manual_mode()`로 막고 있습니다.

즉, 웹에서 실수로 수동 명령을 보내도 모드가 AUTO면 적용되지 않습니다.

#### 데모 모드
`node_c`는 Wi-Fi나 MQTT 설정이 없으면 아예 멈추는 구조가 아니라 **demo mode**로 동작합니다.

이 경우:
- 내부에 준비된 demo payload를 주기적으로 넣고
- 자동 제어 로직을 UART 로그로 검증할 수 있습니다.

이건 발표나 시연에서 꽤 유용한 구조입니다.

### 3-4. web_console: 웹 관제 콘솔

이 프로젝트를 사람이 보기 쉽게 만들어 주는 인터페이스입니다.

#### 기술 스택
- Flask
- Paho MQTT
- HTML / JS / CSS

#### 역할
- `house/#` 전체 구독
- 환경 상태 표시
- `nodeB`, `nodeC` 상태 표시
- heartbeat 표시
- 최근 송수신 로그 표시
- 웹 버튼으로 MQTT 명령 publish

#### 웹에서 보이는 정보
- Broker 연결 상태
- Environment
  - light
  - temp
  - humidity
- Controller (`node_c`)
  - mode
  - light
  - temp
  - humidity
  - lamp
  - window
- Actuator (`node_b`)
  - lamp
  - window
- Heartbeat
  - nodeA / nodeB / nodeC online 여부
- 최근 MQTT 로그

#### 내부 동작
웹 콘솔은 백엔드에서 MQTT를 직접 구독합니다.  
따라서 단순한 웹페이지가 아니라, **브로커에 직접 붙어 있는 모니터링 도구**입니다.

그리고 버튼을 누르면 `/api/command`를 통해 Flask가 MQTT publish를 수행합니다.

즉, 이 웹 콘솔은 “현재 상태 보기”와 “수동 제어”를 모두 수행하는 운영 도구입니다.

---

## 4. MQTT 토픽 구조 정리

이 프로젝트의 핵심은 MQTT 계약이므로 이것도 정리해두는 것이 좋습니다.

### 센서 관련
- `house/env`
- `house/heartbeat/nodeA`

### 제어 명령 관련
- `house/mode`
- `house/cmd/light`
- `house/cmd/window`

### 상태 보고 관련
- `house/status/nodeB`
- `house/status/nodeC`

### 생존 신호 관련
- `house/heartbeat/nodeB`
- `house/heartbeat/nodeC`

---

## 5. 실제 동작 시나리오

### 시나리오 1: 자동 제어
1. `node_a`가 조도, 온도, 습도 측정
2. `house/env`에 발행
3. `node_c`가 수신
4. AUTO 모드인지 확인
5. 임계값 비교
6. 필요 시 `house/cmd/light`, `house/cmd/window` 발행
7. `node_b`가 명령 수신
8. LED 스트립과 서보를 실제로 동작시킴
9. `node_b`가 `house/status/nodeB` 발행
10. `web_console`이 모든 상태를 화면에 반영

### 시나리오 2: 수동 제어
1. 사용자가 웹 콘솔에서 MANUAL 선택
2. 웹이 `house/mode = MANUAL` 발행
3. `node_c`가 MANUAL로 전환
4. 사용자가 Light On 또는 Window Open 버튼 클릭
5. 웹이 해당 명령 발행
6. `node_b`가 받아 실제 장치 동작
7. 상태가 다시 웹에 반영

### 시나리오 3: 노드 이상 감시
1. `node_a` 또는 `node_b` heartbeat가 끊김
2. `node_c` 또는 `web_console`에서 timeout 판단
3. warning 로그 또는 offline 상태 표시

---

## 6. 이 프로젝트의 장점

이 프로젝트는 단순한 센서-LED 연결보다 훨씬 발전된 구조입니다.

### 1) 역할 분리
센서, 제어, 실행, UI가 분리되어 있습니다.

### 2) MQTT 기반 확장성
노드를 더 붙이기 쉽습니다.  
예를 들어 나중에 팬, 에어컨, 도어락 노드를 추가하기 좋습니다.

### 3) 중앙 정책 관리
자동 제어 기준이 `node_c`에 모여 있어서 유지보수가 쉽습니다.

### 4) 웹 기반 운영 가능
시연과 발표에 유리합니다.

### 5) heartbeat / status 구조
단순 데모를 넘어서 “운영형 시스템”에 가까운 형태입니다.

---

## 7. 현재 기준에서 발표 때 강조하면 좋은 포인트

발표용으로는 아래처럼 표현하면 좋습니다.

- 이 프로젝트는 단일 보드 실습이 아니라 **분산형 스마트 홈 제어 시스템**이다.
- 센서 측정, 의사결정, 액추에이터 실행, 사용자 인터페이스를 각각 분리했다.
- 각 노드는 MQTT를 통해 느슨하게 연결되며, 중앙관리 노드가 정책을 수행한다.
- 자동 제어뿐 아니라 웹 기반 수동 제어, 상태 모니터링, heartbeat 감시까지 포함한다.
- 최신 구현에서는 조명 제어가 단순 LED가 아니라 WS2812 스트립 기반으로 확장되었고, 창문은 싱글 서보 기반으로 동작한다.
