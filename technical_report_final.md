# MQTT 기반 스마트 환경제어 하우스 분산 제어 시스템 기술서

## 1. 프로젝트 개요

### 1.1 프로젝트명
스마트 환경제어 하우스  
부제: MQTT 기반 분산 제어 시스템

### 1.2 프로젝트 목적
본 프로젝트는 실내 환경을 실시간으로 측정하고, 측정된 조도, 온도, 습도 값에 따라 조명과 창문을 자동 또는 수동으로 제어하는 스마트 하우스 시스템을 구현하는 것을 목표로 한다.  
단일 보드에 모든 기능을 넣는 방식이 아니라, 센서 노드, 액추에이터 노드, 중앙관리 노드, 웹 콘솔을 분리하고 MQTT 브로커를 통해 연동함으로써 실제 IoT 시스템에 가까운 구조를 구성하였다.

### 1.3 프로젝트 배경
본 프로젝트는 학습 이후 진행한 종합 프로젝트로서, 지금까지 학습한 센서 제어, 액추에이터 구동, MQTT 통신, 웹 연동 기술을 실제 시스템에 적용해보는 것을 목표로 하였다.  
또한 팀원별 역할을 분담하고 각 기능을 통합하여 하나의 결과물을 완성하는 협업 과정을 통해 학습 내용을 더욱 정확히 이해하고 실제 프로젝트 형태로 응용하고자 하였다.

## 2. 시스템 구성

본 시스템은 다음 요소들로 구성된다.

- `node_a`: 환경 센서 노드
- `node_b`: 액추에이터 노드
- `node_c`: 중앙관리 노드
- `web_console`: 웹 관제 인터페이스
- `MQTT Broker`: 메시지 허브

역할을 한 줄로 정리하면 다음과 같다.

- `node_a`는 측정
- `node_c`는 판단
- `node_b`는 실행
- `web_console`은 모니터링과 수동 제어

### 2.1 계층 관점 구조

#### 현장 계층
- `node_a`: 조도, 온도, 습도 측정
- `node_b`: 조명과 창문 액추에이터 구동

#### 중앙 관리 계층
- `node_c`: 제어 정책 판단, 모드 관리, 상태 집계
- `MQTT Broker`: 토픽 기반 메시지 전달

#### 사용자 계층
- `web_console`: 상태 확인, 로그 확인, 수동 명령 전송

## 3. 팀원 역할 분담

| 팀원 | 주요 담당 | 실제 수행 내용 |
|---|---|---|
| 정귀인 | 중앙 제어 및 통합 | `node_c` 제어 로직 구현, MQTT 연동, 웹 콘솔 개발, 통합 테스트 정리, 기술서 작성 |
| 황지용 | 액추에이터 및 하드웨어 | `node_b` 액추에이터 노드 구현, WS2812 및 싱글 서보 제어, 하드웨어 배선, 모형 제작 |
| 김도경 | 센서 및 발표 자료 | `node_a` 센서 노드 작업, 센서 실습 및 연동 지원, PPT 초안 작성, 모형 제작 지원 |

## 4. 하드웨어 구성

### 4.1 node_a 하드웨어
- `DHT data pin`: `GP15`
- `CDS ADC pin`: `GP27`

### 4.2 node_b 하드웨어
- `SG90 servo`: `GP14`
- `WS2812 data pin`: `GP16`

창문 제어는 `SG90` 서보모터 1개를 사용하며, 조명은 `WS2812 RGB Strip` 8픽셀 전체를 제어한다.

### 4.3 node_c 하드웨어
- `Raspberry Pi Pico 2 W`

`node_c`는 센서나 액추에이터를 직접 구동하기보다 MQTT 기반 제어 정책과 상태 관리를 담당한다.

### 4.4 공통 장비
- `Raspberry Pi Pico 2 W` 3대
- `CDS` 조도 센서
- `DHT` 계열 온습도 센서
- `WS2812 RGB Strip`
- `SG90` 서보모터 1개
- 브레드보드
- 점퍼선

### 4.5 전원 및 배선 고려사항
- 서보는 외부 `5V` 전원 사용 권장
- Pico와 서보의 `GND`는 공통으로 연결 필요
- `WS2812`는 데이터선뿐 아니라 전원 안정성도 중요

## 5. 소프트웨어 구성

### 5.1 node_a: 센서 노드

주요 기능은 다음과 같다.

- Wi-Fi 연결
- MQTT 브로커 접속
- DHT 센서 읽기
- CDS 조도값 읽기
- 조도값 정규화
- `house/env` 발행
- `house/heartbeat/nodeA` 발행

환경 payload 예시:

```txt
light=250,temp=29.5,humidity=72.0
```

조도 ADC raw 값은 `0~4095` 범위지만, 자동 제어 기준과 맞추기 위해 `0~400` 범위로 정규화하여 발행한다.

### 5.2 node_b: 액추에이터 노드

주요 기능은 다음과 같다.

- `house/cmd/light` 구독
- `house/cmd/window` 구독
- `WS2812` 8픽셀 전체 ON/OFF
- 싱글 서보 기반 창문 OPEN/CLOSE
- `house/status/nodeB` 발행
- `house/heartbeat/nodeB` 발행

상태 payload 예시:

```txt
lamp=ON,window=OPEN
```

### 5.3 node_c: 중앙관리 노드

주요 기능은 다음과 같다.

- `AUTO / MANUAL` 모드 관리
- `house/env` payload 파싱
- 임계값 기반 자동 제어
- MQTT 수동 명령 처리
- UART 수동 명령 처리
- `nodeA`, `nodeB` timeout 감시
- `house/status/nodeC` 발행
- `house/heartbeat/nodeC` 발행

### 5.4 web_console: 웹 관제 인터페이스

주요 기능은 다음과 같다.

- `house/#` 전체 구독
- 환경 데이터 표시
- `nodeB`, `nodeC` 상태 표시
- heartbeat 기반 온라인 상태 표시
- 최근 MQTT 로그 표시
- 웹 버튼 기반 수동 명령 publish

## 6. MQTT 통신 구조

### 6.1 주요 토픽
- `house/env`
- `house/mode`
- `house/cmd/light`
- `house/cmd/window`
- `house/status/nodeB`
- `house/status/nodeC`
- `house/heartbeat/nodeA`
- `house/heartbeat/nodeB`
- `house/heartbeat/nodeC`

### 6.2 MQTT 토픽 역할표

| 토픽 | 발행 주체 | 수신 주체 | 의미 |
|---|---|---|---|
| `house/env` | `node_a` | `node_c`, `web_console` | 센서 데이터 |
| `house/mode` | `web_console`, `node_c` | `node_c`, `web_console` | AUTO / MANUAL 전환 |
| `house/cmd/light` | `node_c`, `web_console` | `node_b`, `node_c`, `web_console` | 조명 제어 |
| `house/cmd/window` | `node_c`, `web_console` | `node_b`, `node_c`, `web_console` | 창문 제어 |
| `house/status/nodeB` | `node_b` | `node_c`, `web_console` | 액추에이터 상태 |
| `house/status/nodeC` | `node_c` | `web_console` | 중앙 제어 상태 |
| `house/heartbeat/nodeA` | `node_a` | `node_c`, `web_console` | 센서 노드 생존 확인 |
| `house/heartbeat/nodeB` | `node_b` | `node_c`, `web_console` | 액추에이터 노드 생존 확인 |
| `house/heartbeat/nodeC` | `node_c` | `web_console` | 중앙관리 노드 생존 확인 |

## 7. 제어 로직

### 7.1 AUTO 모드
1. `node_a`가 조도, 온도, 습도를 측정한다.
2. 측정값을 `house/env`로 발행한다.
3. `node_c`가 payload를 파싱한다.
4. 현재 모드가 `AUTO`인지 확인한다.
5. 조도 기준으로 조명 ON/OFF를 판단한다.
6. 온도/습도 기준으로 창문 OPEN/CLOSE를 판단한다.
7. 필요할 때만 `house/cmd/light`, `house/cmd/window`를 발행한다.
8. `node_b`가 실제 장치를 동작시킨다.
9. `node_b`, `node_c`가 상태를 발행한다.
10. `web_console`이 상태와 heartbeat를 갱신한다.

자동 제어 기준은 다음과 같다.

- 조명 ON: `light < 280`
- 조명 OFF: `light > 320`
- 창문 OPEN: `temp > 28.0` 또는 `humidity > 70.0`
- 창문 CLOSE: `temp <= 27.0` 그리고 `humidity <= 65.0`

### 7.2 MANUAL 모드
1. 사용자가 웹 콘솔에서 `Mode Manual`을 선택한다.
2. `house/mode = MANUAL`이 발행된다.
3. `node_c`가 MANUAL 모드로 전환된다.
4. 사용자가 조명 또는 창문 버튼을 선택한다.
5. 웹 콘솔이 제어 명령을 발행한다.
6. `node_b`는 이를 수신하여 장치를 제어한다.
7. `node_c`는 수신된 제어 명령을 상태에 반영한다.
8. `node_b`, `node_c` 상태가 웹 콘솔에 반영된다.

### 7.3 핵심 설계 포인트
- `AUTO` 모드에서는 수동 `light/window` 명령을 무시한다.
- 이미 같은 상태이면 중복 명령을 다시 발행하지 않는다.
- timeout 경고를 통해 `nodeA`, `nodeB` 이상 여부를 감지한다.
- status snapshot을 주기적으로 발행하여 UI와 로그를 안정적으로 동기화한다.

## 8. 구현 및 테스트 결과

### 8.1 확인된 통합 흐름
- `node_a + node_c + node_b + web_console` 전체 MQTT 흐름 확인
- `node_b + node_c + web_console` 통합 테스트 확인
- `web_console + node_c` 조합으로 상태 확인 및 수동 제어 가능

### 8.2 검증된 주요 장면
- 센서값 변화에 따라 조명과 창문이 자동 동작
- 웹 콘솔에서 `AUTO / MANUAL` 모드 전환
- 웹에서 조명 ON/OFF, 창문 OPEN/CLOSE 수동 제어
- `nodeB`, `nodeC`, heartbeat 상태가 웹 콘솔에 반영

### 8.3 테스트 근거
- `node_c` smoke test 통과
- `node_c` 실보드 MQTT 송수신 테스트 통과
- `node_b + node_c + web_console` 통합 검증 기록 존재
- `node_a + node_b + node_c + web_console` 전체 흐름 검증 기록 존재

## 9. 프로젝트 진행 중 어려웠던 점 및 해결 과정

### 9.1 협업 및 통합 개발 방식
처음으로 팀 단위 협업을 진행하다 보니 GitHub를 활용한 실시간 통합 개발 과정에서 충돌이나 오류가 발생할 가능성이 크다고 판단하였다. 이에 따라 초기 단계에서 각 MCU의 역할을 명확히 분리하고, 각 노드를 독립적으로 개발 및 테스트한 뒤 최종적으로 통합하는 방식을 선택하였다. 또한 각 MCU별 구현 기능과 통합 시 참고해야 할 사항을 텍스트로 정리하여 팀원 간 기준을 맞추었고, 이를 바탕으로 개발을 진행함으로써 통합 과정에서 발생할 수 있는 문제를 사전에 예방할 수 있었다.

### 9.2 창문 구동 구조 구현
서보모터를 이용해 자동으로 제어되는 창문을 구현하는 과정에서 창문의 회전축과 모터의 회전축이 서로 다르다는 구조적 문제가 있었다. 이로 인해 단순히 모터를 연결하는 방식만으로는 자연스러운 개폐 동작을 구현하기 어려웠다. 이에 추가 조사와 구조 검토를 진행한 뒤 싱글 서보 기반 창문 구동 구조를 설계하였고, 이를 적용하여 자동 창문 개폐 기능을 구현할 수 있었다.

## 10. 프로젝트 강점

- 역할 분리가 명확한 분산형 구조
- MQTT 기반의 높은 확장성
- 중앙 정책 기반 자동 제어
- 웹 콘솔 기반 시각적 모니터링
- heartbeat와 status snapshot을 포함한 운영형 구조
- WS2812와 싱글 서보를 활용한 실제 동작 중심 구현

## 11. 한계점 및 향후 계획

### 11.1 현재 한계
- 실제 배선과 전원 안정성에 따라 서보 동작이 민감할 수 있음
- 센서와 액추에이터 배치에 따라 측정값과 제어 동작이 서로 간섭할 수 있음
- 환경에 따라 임계값 보정이 추가로 필요할 수 있음
- MQTT reconnect 및 DNS 재시도 안정성이 더 보완될 필요가 있음

### 11.2 향후 개선 방향
- reconnect / 재시도 로직 강화
- heartbeat payload에 uptime 또는 상태 코드 추가
- 센서 데이터 저장 및 이력 시각화 추가
- 모바일 또는 알림 기능 확장
- `OLED/LCD` 상태 출력 추가
- 추가 센서 및 액추에이터 확장

## 12. 결론

본 프로젝트는 센서 노드, 액추에이터 노드, 중앙관리 노드, 웹 콘솔을 MQTT 기반으로 분리하여 구성한 스마트 환경제어 하우스 시스템이다.  
`node_a`는 환경을 측정하고, `node_c`는 정책을 판단하며, `node_b`는 실제 장치를 제어하고, `web_console`은 상태 모니터링과 수동 제어를 제공한다.  
결과적으로 단순 센서 실습을 넘어, 역할 분리, 제어 정책, 운영 상태 확인까지 포함한 실제 시스템형 프로젝트로 정리할 수 있다.
