# 1차 프로젝트 기술서

## 1. 프로젝트명

스마트 환경제어 하우스

## 2. 프로젝트 개요

본 프로젝트는 `Raspberry Pi Pico 2 W` 3대를 이용하여 실내 환경을 실시간으로 측정하고, 측정된 환경 정보에 따라 조명과 환기 장치를 자동 또는 수동으로 제어하는 `스마트 환경제어 하우스 시스템`을 구현한 것이다.

시스템은 역할에 따라 세 개의 노드로 구성하였다.

- `Node A`: 실내 환경을 측정하는 센서 노드
- `Node B`: 조명 및 환기 장치를 제어하는 액추에이터 노드
- `Node C`: 전체 상태를 판단하고 제어 명령을 생성하는 중앙 관리 노드

각 노드는 `Wi-Fi`와 `MQTT`를 기반으로 통신하며, 별도의 `Web Console`을 통해 전체 상태를 모니터링하고 수동 제어가 가능하도록 구성하였다. 이를 통해 단순 센서 측정에 그치지 않고, 센서 입력, 제어 로직, 액추에이터 동작, 사용자 인터페이스를 하나의 통합 시스템으로 구현하였다.

## 3. 개발 목적

본 프로젝트의 주요 목적은 다음과 같다.

- 실내 환경을 실시간으로 측정할 수 있는 센서 시스템 구현
- 측정값에 따라 조명과 환기 장치를 자동 제어하는 시스템 구현
- `AUTO` 모드와 `MANUAL` 모드를 모두 지원하는 제어 구조 설계
- `MQTT` 기반 노드 간 통신 구조 학습 및 적용
- 분산형 임베디드 시스템 개발 및 협업 방식 경험
- 향후 웹, 모바일, 데이터 기록 기능으로 확장 가능한 구조 마련

## 4. 시스템 구성 및 동작 원리

시스템 구조는 아래와 같다.

```txt
[Node A: 환경 센서] ---- MQTT/Wi-Fi ----\
                                        > [Node C: 중앙 관리] ---- MQTT/Wi-Fi ---- [Node B: 액추에이터]
                                                      \
                                                       \---- [Web Console]
```

전체 동작 흐름은 다음과 같다.

1. `Node A`가 조도, 온도, 습도를 주기적으로 측정한다.
2. 측정된 데이터는 `house/env` 토픽으로 발행된다.
3. `Node C`는 해당 데이터를 수신하여 기준값과 비교한다.
4. 조건에 따라 조명과 창문 제어 명령을 생성한다.
5. `Node B`는 명령을 수신하여 실제 LED와 서보모터를 동작시킨다.
6. `Web Console`은 각 노드의 상태, heartbeat, 최근 로그를 표시하며 필요 시 수동 제어 명령을 발행한다.

즉, 본 시스템은 `환경 감지 -> 중앙 판단 -> 장치 제어 -> 상태 모니터링`의 흐름으로 동작하는 구조를 가진다.

## 5. 개발 환경 및 사용 기술

### 5-1. 하드웨어 구성

- `Raspberry Pi Pico 2 W` 3대
- `CDS` 조도 센서
- `DHT` 계열 온습도 센서
- `WS2812 RGB Strip`
- `SG90` 서보모터
- 브레드보드
- 점퍼선

### 5-2. 소프트웨어 및 개발 도구

- `C 언어`
- `Pico SDK`
- `lwIP MQTT`
- `Python`
- `Flask`
- `HTML`
- `CSS`
- `JavaScript`
- `GitHub`

## 6. 노드별 역할 및 구현 내용

### 6-1. Node A: 환경 센서 노드

`Node A`는 실내 환경 정보를 수집하여 중앙 관리 노드가 사용할 수 있도록 MQTT로 전송하는 역할을 수행한다.

구현한 주요 기능은 다음과 같다.

- 조도 센서 값 읽기
- 온도 및 습도 센서 값 읽기
- 조도 ADC raw 값을 `0~400` 범위로 정규화
- `house/env` 토픽으로 환경 데이터 발행
- `house/heartbeat/nodeA` 토픽으로 heartbeat 발행
- UART 로그 출력

현재 하드웨어 연결 기준은 다음과 같다.

- `DHT data pin`: `GP15`
- `CDS ADC pin`: `GP27 (ADC1)`

환경 데이터 payload 예시는 아래와 같다.

```txt
light=250,temp=29.5,humidity=72.0
```

### 6-2. Node B: 액추에이터 노드

`Node B`는 중앙 관리 노드에서 생성한 제어 명령을 실제 하드웨어 동작으로 반영하는 역할을 수행한다.

구현한 주요 기능은 다음과 같다.

- `house/cmd/light` 수신 후 조명 상태 제어
- `house/cmd/window` 수신 후 창문 상태 제어
- `WS2812 RGB Strip` 8픽셀 전체 ON/OFF 제어
- 서보모터 OPEN/CLOSE 제어
- `house/status/nodeB` 상태 발행
- `house/heartbeat/nodeB` heartbeat 발행
- 비정상 명령 수신 시 경고 로그 출력

현재 하드웨어 연결 기준은 다음과 같다.

- `Servo pin`: `GP14`
- `WS2812 data pin`: `GP16`

상태 payload 예시는 아래와 같다.

```txt
lamp=ON,window=OPEN
lamp=OFF,window=CLOSE
```

### 6-3. Node C: 중앙 관리 노드

`Node C`는 전체 시스템의 제어 중심 역할을 수행하며, 센서 데이터를 바탕으로 상태를 판단하고 적절한 제어 명령을 생성한다.

구현한 주요 기능은 다음과 같다.

- `AUTO / MANUAL` 모드 관리
- `house/env` payload 파싱
- 기준값 기반 자동 제어
- 중복 명령 방지
- UART 기반 수동 명령 처리
- `house/mode` 수신 처리
- `house/cmd/light`, `house/cmd/window` 수신 처리
- `house/status/nodeC` 상태 발행
- `house/heartbeat/nodeC` heartbeat 발행
- `Node A`, `Node B` timeout 경고 출력

UART 수동 명령 예시는 아래와 같다.

```txt
mode manual
mode auto
light on
light off
window open
window close
```

## 7. 통신 설계

### 7-1. MQTT 토픽 구성

본 프로젝트에서 사용한 주요 MQTT 토픽은 다음과 같다.

- `house/env`
- `house/mode`
- `house/cmd/light`
- `house/cmd/window`
- `house/status/nodeB`
- `house/status/nodeC`
- `house/heartbeat/nodeA`
- `house/heartbeat/nodeB`
- `house/heartbeat/nodeC`

### 7-2. 자동 제어 기준

자동 제어 기준은 다음과 같이 설정하였다.

- `light < 280` 이면 조명 `ON`
- `light > 320` 이면 조명 `OFF`
- `temp > 28` 또는 `humidity > 70` 이면 창문 `OPEN`
- `temp <= 27` 그리고 `humidity <= 65` 이면 창문 `CLOSE`

이와 같은 기준을 적용함으로써 센서값이 변할 때만 필요한 제어 명령이 전송되도록 하였고, 동일한 상태에서 명령이 반복 발행되는 문제를 줄였다.

## 8. 웹 콘솔 구현

본 프로젝트에서는 하드웨어 노드 외에도 `Flask + MQTT` 기반의 `Web Console`을 구현하였다.

주요 기능은 다음과 같다.

- `house/#` 전체 토픽 구독
- `house/env` 센서값 표시
- `house/status/nodeB`, `house/status/nodeC` 상태 표시
- `house/heartbeat/nodeA`, `nodeB`, `nodeC` 연결 상태 표시
- 최근 MQTT 송수신 로그 표시
- 버튼 기반 수동 제어 명령 발행

지원하는 수동 제어 명령은 다음과 같다.

- `Mode Auto`
- `Mode Manual`
- `Light On`
- `Light Off`
- `Window Open`
- `Window Close`

웹 콘솔을 통해 사용자는 전체 상태를 한 화면에서 확인할 수 있으며, 필요 시 중앙 관리 노드와 액추에이터 노드에 수동 제어 명령을 전달할 수 있다. 이를 통해 하드웨어 동작 검증과 통합 테스트를 보다 직관적으로 수행할 수 있었다.

## 9. 테스트 및 검증 결과

### 9-1. Node C 제어 로직 테스트

`controller_smoke_test`를 통해 다음 항목을 검증하였다.

- 초기 `AUTO` 모드 설정
- 환경 데이터 파싱
- 자동 제어 명령 생성
- 수동 명령 처리
- MQTT mode/light/window 처리
- 상태 발행
- timeout 경고

검증 결과:

- `PASS`

### 9-2. Node B + Node C 통합 테스트

실제 `Pico 2 W` 2대를 사용하여 다음 항목을 검증하였다.

- `house/env` 기준 자동 판단
- `house/cmd/light`, `house/cmd/window` 자동 발행
- `Node B` 액추에이터 실제 반영
- `house/status/nodeB`, `house/heartbeat/nodeB` 정상 반영

검증 결과:

- `PASS`

### 9-3. Node A + Node B + Node C + Web Console 통합 테스트

최종 통합 단계에서는 아래의 흐름을 확인하였다.

1. `Node A`가 환경 데이터를 발행한다.
2. `Node C`가 데이터를 수신하여 자동 제어를 판단한다.
3. `Node B`가 제어 명령을 받아 실제 장치를 동작시킨다.
4. `Web Console`에서 전체 상태와 heartbeat를 확인한다.

검증 결과:

- 전체 MQTT 기반 동작 흐름 정상 확인

## 10. 프로젝트 수행 결과

본 프로젝트를 통해 다음과 같은 결과를 얻었다.

- 센서 측정, 중앙 판단, 액추에이터 제어 기능을 노드별로 분리하여 구현하였다.
- `MQTT` 기반 분산형 홈 IoT 구조를 실제로 구성하였다.
- 자동 제어와 수동 제어를 모두 지원하는 제어 시스템을 완성하였다.
- 웹 콘솔을 추가하여 실시간 모니터링과 원격 제어 가능성을 확인하였다.
- 하드웨어, 네트워크, 제어 로직, 사용자 인터페이스를 하나의 시스템으로 통합하였다.

## 11. 한계점 및 향후 개선 방향

현재 프로젝트는 1차 구현 단계로서 핵심 기능 중심으로 완성되었으며, 향후 다음과 같은 방향으로 개선할 수 있다.

- `OLED` 또는 `LCD`를 추가하여 보드 단독 상태 표시 기능 구현
- MQTT 재연결 및 예외 처리 로직 강화
- 센서값 필터링 및 평균 처리 고도화
- 데이터 저장 및 이력 조회 기능 추가
- 모바일 또는 웹 UI 고도화
- 알람 기능 또는 부저 연동

## 12. 결론

스마트 환경제어 하우스 프로젝트는 실내 환경 측정, 자동 제어, 수동 제어, 웹 기반 모니터링을 하나로 통합한 IoT 시스템으로 구현되었다.

특히 `Node A`, `Node B`, `Node C`의 역할을 명확히 분리하고 `MQTT` 기반으로 연동함으로써, 실제 스마트 홈 시스템과 유사한 구조를 경험할 수 있었다. 또한 하드웨어 제어와 네트워크 통신, 상태 관리 로직을 하나의 프로젝트로 통합하는 과정을 통해 임베디드 시스템 설계와 협업 개발의 흐름을 실제로 익힐 수 있었다.
