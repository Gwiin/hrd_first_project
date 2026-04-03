# hrd_first_project 기술서 검토본 및 Gamma PPT 프롬프트 개선안

## 1. 검토 요약

`hrd_first_project` 하위 문서와 코드, 테스트 기록을 기준으로 확인한 결과 현재 프로젝트 이해 자체는 비교적 정확하게 반영되어 있다.  
특히 아래 내용은 실제 구현과 잘 맞는다.

- `node_a`, `node_b`, `node_c`, `web_console`로 분리된 구조
- `house/env`, `house/mode`, `house/cmd/light`, `house/cmd/window` 중심의 MQTT 설계
- `AUTO / MANUAL` 모드 분리
- `WS2812` 조명 제어와 싱글 서보 창문 제어
- heartbeat 및 status snapshot 기반 모니터링 구조

다만 발표 자료 기준으로는 아래 항목이 더 선명하게 들어가야 완성도가 높아진다.

- 프로젝트명 표기 통일
- 팀원별 수행 역할 명시
- 자동 / 수동 플로우차트 분리 제시
- 핵심 코드 리뷰 슬라이드 추가
- 테스트 근거와 실제 검증 범위 제시
- 향후 확장 가능성과 한계점을 분리해서 정리

아래 내용은 위 보완점을 반영해 바로 제출용 / 발표준비용으로 사용할 수 있게 다시 정리한 최종본이다.

---

## 2. 최종 기술서

# MQTT 기반 스마트 환경제어 하우스 분산 제어 시스템 기술서

## 2.1 프로젝트 개요

### 2.1.1 프로젝트명
MQTT 기반 스마트 환경제어 하우스 분산 제어 시스템

발표 자료에서는 아래처럼 표기하면 가장 자연스럽다.

- 메인 제목: `스마트 환경제어 하우스`
- 부제: `MQTT 기반 분산 제어 시스템`

### 2.1.2 프로젝트 목적
본 프로젝트는 실내 환경을 실시간으로 측정하고, 측정된 조도, 온도, 습도 값에 따라 조명과 창문을 자동 또는 수동으로 제어하는 스마트 하우스 시스템을 구현하는 것을 목표로 한다.  
단일 보드에 모든 기능을 넣는 방식이 아니라, 센서 노드, 액추에이터 노드, 중앙관리 노드, 웹 콘솔을 분리하고 MQTT 브로커를 통해 연동함으로써 실제 IoT 시스템에 가까운 구조를 만들고자 했다.

### 2.1.3 개발 배경
기존의 단일 보드 실습은 센서 입력과 장치 제어를 빠르게 확인하는 데는 유리하지만, 시스템 구조가 복잡해지고 역할 분리가 어렵다는 한계가 있다.  
본 프로젝트는 이러한 한계를 해결하기 위해 기능을 노드별로 분리하고, MQTT 기반 메시지 구조를 통해 느슨하게 연결하였다. 이 방식은 유지보수와 확장에 유리하며, 팀 협업에도 적합하다.

---

## 2.2 시스템 아키텍처

본 시스템은 아래 4개 요소와 MQTT 브로커로 구성된다.

- `node_a`: 환경 센서 노드
- `node_b`: 액추에이터 노드
- `node_c`: 중앙관리 노드
- `web_console`: 웹 관제 인터페이스
- `MQTT Broker`: 메시지 허브

역할을 한 줄로 정리하면 다음과 같다.

- `node_a`는 측정
- `node_c`는 판단
- `node_b`는 실행
- `web_console`은 관찰과 수동제어

### 2.2.1 계층 관점 구조

#### 현장 계층
- `node_a`: 조도, 온도, 습도 측정
- `node_b`: 조명과 창문 액추에이터 구동

#### 중앙 관리 계층
- `node_c`: 정책 판단, 모드 관리, 상태 집계
- `MQTT Broker`: 토픽 기반 메시지 전달

#### 사용자 계층
- `web_console`: 상태 확인, 로그 확인, 수동 명령 전송

---

## 2.3 팀원 역할 분담

발표 자료에는 아래처럼 정리하는 것이 좋다.

| 팀원 | 주요 담당 | 실제 수행 내용 |
|---|---|---|
| 정귀인 | 중앙 제어 및 통합 | `node_c` 제어 로직 구현, MQTT 연동, 웹 콘솔 개발, 통합 테스트 정리, 기술서 작성 |
| 황지용 | 액추에이터 및 하드웨어 | `node_b` 액추에이터 노드 구현, WS2812 및 싱글 서보 제어, 배선 및 모형 제작 |
| 김도경 | 센서 및 발표 자료 | `node_a` 센서 노드 작업, 센서 실습 및 연동 지원, PPT 초안 작성, 모형 제작 |

주의할 점은 발표 때 "누가 무엇을 만들었는지"가 보이되, 최종 결과는 협업 산출물이라는 점을 함께 강조하는 것이다.

---

## 2.4 하드웨어 구성

### 2.4.1 node_a 하드웨어
- `DHT data pin`: `GP15`
- `CDS ADC pin`: `GP27`

`node_a`는 조도, 온도, 습도를 읽어 `house/env` 토픽으로 발행한다.

### 2.4.2 node_b 하드웨어
- `SG90 servo`: `GP14`
- `WS2812 data pin`: `GP16`

최신 구현 기준으로 창문 제어는 `SG90` 서보모터 1개를 사용한다. 창문 회전축과 서보모터 회전축 차이를 고려해 구조를 설계했으며, 서보 각도를 조절해 자동 개폐를 표현한다.

조명은 단순 LED가 아니라 `WS2812 RGB Strip` 8픽셀 전체를 제어한다. `ON`이면 전체 흰색 점등, `OFF`이면 전체 소등된다.

### 2.4.3 node_c 하드웨어
`node_c`는 Raspberry Pi Pico 2 W 기반 중앙 제어 노드이며, 센서나 액추에이터를 직접 구동하기보다 MQTT 기반 제어 정책과 상태 관리를 담당한다.

### 2.4.4 공통 장비
- `Raspberry Pi Pico 2 W` 3대
- `CDS` 조도 센서
- `DHT` 계열 온습도 센서
- `WS2812 RGB Strip`
- `SG90` 서보모터
- 브레드보드
- 점퍼선

### 2.4.5 전원 및 배선 고려사항
- 서보는 외부 `5V` 전원 사용 권장
- Pico와 서보의 `GND`는 공통으로 연결 필요
- `WS2812`는 데이터선뿐 아니라 전원 안정성도 중요

---

## 2.5 소프트웨어 구성

### 2.5.1 node_a: 센서 노드

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

조도 ADC raw 값은 `0~4095` 범위지만, `node_c`의 자동 제어 기준과 맞추기 위해 `0~400` 범위로 정규화하여 발행한다.

### 2.5.2 node_b: 액추에이터 노드

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

### 2.5.3 node_c: 중앙관리 노드

주요 기능은 다음과 같다.

- `AUTO / MANUAL` 모드 관리
- `house/env` payload 파싱
- 임계값 기반 자동 제어
- MQTT 수동 명령 처리
- UART 수동 명령 처리
- `nodeA`, `nodeB` timeout 감시
- `house/status/nodeC` 발행
- `house/heartbeat/nodeC` 발행

자동 제어 기준은 다음과 같다.

- 조명 ON: `light < 280`
- 조명 OFF: `light > 320`
- 창문 OPEN: `temp > 28.0` 또는 `humidity > 70.0`
- 창문 CLOSE: `temp <= 27.0` 그리고 `humidity <= 65.0`

이 구조는 경계값에서 장치가 반복 동작하지 않도록 ON/OFF 기준을 분리한 히스테리시스 설계라는 점을 발표에서 강조하면 좋다.

### 2.5.4 web_console: 웹 관제 인터페이스

주요 기능은 다음과 같다.

- `house/#` 전체 구독
- 환경 데이터 표시
- `nodeB`, `nodeC` 상태 표시
- heartbeat 기반 온라인 상태 표시
- 최근 MQTT 로그 표시
- 웹 버튼 기반 수동 명령 publish

웹 콘솔은 시연에서 "보여주는 화면" 이상의 의미가 있다.  
실제 운영 관제 화면처럼 현재 상태, 명령, 로그를 하나로 묶어주기 때문에 프로젝트 완성도를 높여주는 요소다.

---

## 2.6 MQTT 통신 구조

### 2.6.1 환경 데이터 토픽
- `house/env`

`node_a`가 발행하고 `node_c`, `web_console`이 활용한다.

### 2.6.2 제어 명령 토픽
- `house/mode`
- `house/cmd/light`
- `house/cmd/window`

`house/mode`는 모드 전환에 사용된다.  
`house/cmd/light`, `house/cmd/window`는 조명과 창문 제어에 사용된다.

### 2.6.3 상태 보고 토픽
- `house/status/nodeB`
- `house/status/nodeC`

`node_b`는 액추에이터 반영 결과를, `node_c`는 전체 제어 상태를 snapshot 형태로 발행한다.

### 2.6.4 heartbeat 토픽
- `house/heartbeat/nodeA`
- `house/heartbeat/nodeB`
- `house/heartbeat/nodeC`

각 노드는 heartbeat를 발행해 살아 있음을 알리고, 웹 콘솔은 이를 기반으로 online / offline을 표시한다.

### 2.6.5 MQTT 토픽 역할표

| 토픽 | 발행 주체 | 수신 주체 | 의미 |
|---|---|---|---|
| `house/env` | `node_a` | `node_c`, `web_console` | 센서 데이터 |
| `house/mode` | `web_console`, `node_c` | `node_c`, `web_console` | AUTO / MANUAL 전환 |
| `house/cmd/light` | `node_c`, `web_console` | `node_b`, `node_c`, `web_console` | 조명 제어 |
| `house/cmd/window` | `node_c`, `web_console` | `node_b`, `node_c`, `web_console` | 창문 제어 |
| `house/status/nodeB` | `node_b` | `node_c`, `web_console` | 액추에이터 상태 |
| `house/status/nodeC` | `node_c` | `web_console` | 중앙 제어 상태 |
| `house/heartbeat/nodeA` | `node_a` | `web_console`, `node_c` | 센서 노드 생존 확인 |
| `house/heartbeat/nodeB` | `node_b` | `web_console`, `node_c` | 액추에이터 노드 생존 확인 |
| `house/heartbeat/nodeC` | `node_c` | `web_console` | 중앙관리 노드 생존 확인 |

---

## 2.7 제어 로직

### 2.7.1 AUTO 모드 흐름

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

### 2.7.2 MANUAL 모드 흐름

1. 사용자가 웹 콘솔에서 `Mode Manual`을 선택한다.
2. `house/mode = MANUAL`이 발행된다.
3. `node_c`가 MANUAL 모드로 전환된다.
4. 사용자가 조명 또는 창문 버튼을 선택한다.
5. 웹 콘솔이 제어 명령을 발행한다.
6. `node_b`는 이를 수신하여 장치를 제어한다.
7. `node_c`는 수신된 제어 명령을 상태에 반영한다.
8. `node_b`, `node_c` 상태가 웹 콘솔에 반영된다.

### 2.7.3 핵심 설계 포인트

- `AUTO` 모드에서는 수동 `light/window` 명령을 무시한다.
- 이미 같은 상태이면 중복 명령을 다시 발행하지 않는다.
- timeout 경고를 통해 `nodeA`, `nodeB` 이상 여부를 감지한다.
- status snapshot을 주기적으로 발행하여 UI와 로그를 안정적으로 동기화한다.

---

## 2.8 핵심 코드 리뷰 포인트

발표에 이 항목이 들어가면 단순 시연 발표가 아니라 "우리가 어떤 설계를 코드로 구현했는지"를 보여줄 수 있다.

### 2.8.1 임계값 기반 자동 제어
`node_c`는 환경값을 파싱한 뒤 조명과 창문 제어를 각각 판단한다.  
조명은 `280 / 320`, 창문은 `28.0 / 27.0`, `70.0 / 65.0` 기준으로 동작한다.

발표 슬라이드에 넣기 좋은 실제 코드 예시는 아래와 같다.

```c
if (controller->env.light < controller->thresholds.light_on_threshold) {
    set_lamp_state(controller, NODE_C_SWITCH_ON, "light below threshold");
} else if (controller->env.light > controller->thresholds.light_off_threshold) {
    set_lamp_state(controller, NODE_C_SWITCH_OFF, "light above threshold");
}

if (controller->env.temp > controller->thresholds.window_open_temp_threshold ||
    controller->env.humidity > controller->thresholds.window_open_humidity_threshold) {
    set_window_state(controller, NODE_C_WINDOW_OPEN, "temp or humidity high");
}
```

위 코드는 [node_c_controller.c](/Users/jeong-gwiin/hrd_first_project/node_c/src/node_c_controller.c#L205) 의 핵심 부분으로, 센서값을 정책 조건에 따라 바로 명령으로 연결하는 구조를 보여준다.

### 2.8.2 AUTO 모드에서 수동 명령 차단
`node_c`는 `AUTO` 모드일 때 웹이나 MQTT에서 들어온 수동 명령을 무시한다.  
이 덕분에 사용자가 실수로 버튼을 눌러도 자동 제어 정책이 깨지지 않는다.

```c
static bool ensure_manual_mode(node_c_controller_t *controller, const char *source, const char *payload)
{
    if (controller->mode == NODE_C_MODE_MANUAL) {
        return true;
    }

    node_c_logf(controller, "[NODE_C] WARN manual command ignored in AUTO mode from %s: %s", source, payload);
    return false;
}
```

이 부분은 [node_c_controller.c](/Users/jeong-gwiin/hrd_first_project/node_c/src/node_c_controller.c#L109) 에 있으며, 발표에서는 "모드 정책 보호 장치"라고 설명하면 이해가 쉽다.

### 2.8.3 중복 명령 방지
현재 장치 상태와 같은 명령은 다시 발행하지 않도록 설계되어 있다.  
이는 불필요한 MQTT 트래픽과 액추에이터 반복 동작을 줄이는 데 유리하다.

```c
static void set_lamp_state(node_c_controller_t *controller, node_c_switch_state_t next_state, const char *reason)
{
    if (controller->lamp_state == next_state) {
        return;
    }

    controller->lamp_state = next_state;
    node_c_publish(controller, NODE_C_TOPIC_CMD_LIGHT, lamp_name(next_state));
}
```

이 예시는 [node_c_controller.c](/Users/jeong-gwiin/hrd_first_project/node_c/src/node_c_controller.c#L183) 에서 확인할 수 있고, "상태가 바뀔 때만 publish"라는 설계 의도를 잘 보여준다.

### 2.8.4 노드 상태 동기화
`node_b`는 실제 반영 결과를 `house/status/nodeB`로 다시 알려주고, `node_c`는 이를 반영해 전체 상태를 정리한다.  
즉, 제어 명령만 보내는 구조가 아니라 "결과 확인"까지 고려한 구조다.

```c
snprintf(payload, sizeof(payload), "mode=%s,light=%d,temp=%.1f,humidity=%.1f,lamp=%s,window=%s",
    mode_name(controller->mode),
    controller->env.light,
    controller->env.temp,
    controller->env.humidity,
    lamp_name(controller->lamp_state),
    window_name(controller->window_state));
```

이 snapshot 구성은 [node_c_controller.c](/Users/jeong-gwiin/hrd_first_project/node_c/src/node_c_controller.c#L166) 에 있으며, 웹 콘솔이 현재 상태를 안정적으로 그릴 수 있게 만드는 핵심이다.

### 2.8.5 heartbeat 및 timeout 감시
각 노드는 heartbeat를 보내고, `node_c`는 일정 시간 응답이 없으면 timeout 경고를 출력한다.  
이 기능은 발표에서 "운영 관점의 설계"로 설명하기 좋다.

### 2.8.6 실제 하드웨어 구동 코드
`node_b`에서는 명령을 받은 뒤 단순 상태 변경만 하는 것이 아니라, WS2812와 서보를 실제로 구동한다.

```c
if (servo->current_us < target_us) {
    next_us = (int32_t) servo->current_us + WINDOW_SERVO_STEP_US;
} else {
    next_us = (int32_t) servo->current_us - WINDOW_SERVO_STEP_US;
}
```

위 코드는 [tempservo_led.c](/Users/jeong-gwiin/hrd_first_project/node_b/src/tempservo_led.c#L168) 에 있으며, 서보를 목표 각도로 한 번에 점프시키지 않고 단계적으로 이동시키는 흐름을 보여준다.

```c
g_state.lamp_on = lamp_on;
ws2812_fill(lamp_on ? WS2812_COLOR_WHITE : 0);
publish_status_snapshot();
```

이 부분은 [tempservo_led.c](/Users/jeong-gwiin/hrd_first_project/node_b/src/tempservo_led.c#L392) 에 있으며, 조명 상태를 실제 LED 스트립에 반영하고 즉시 상태를 다시 발행하는 흐름을 보여준다.

발표에서는 코드를 전부 읽기보다 아래처럼 요약하면 좋다.

- `node_c`: 정책 판단과 명령 생성
- `node_b`: 실제 하드웨어 반영과 결과 보고
- `web_console`: 상태 시각화와 수동 제어

---

## 2.9 구현 및 테스트 결과

### 2.9.1 확인된 통합 흐름
- `node_a + node_c + node_b + web_console` 전체 MQTT 흐름 확인
- `node_b + node_c + web_console` 통합 테스트 확인
- `web_console + node_c` 조합으로 상태 확인 및 수동 제어 가능

### 2.9.2 검증된 주요 장면
- 센서값 변화에 따라 조명과 창문이 자동 동작
- 웹 콘솔에서 `AUTO / MANUAL` 모드 전환
- 웹에서 조명 ON/OFF, 창문 OPEN/CLOSE 수동 제어
- `nodeB`, `nodeC`, heartbeat 상태가 웹 콘솔에 반영

### 2.9.3 테스트 근거
- `node_c` smoke test 통과
- `node_c` 실보드 MQTT 송수신 테스트 통과
- `node_b + node_c + web_console` 통합 검증 기록 존재
- `node_a + node_b + node_c + web_console` 전체 흐름 검증 기록 존재

---

## 2.10 프로젝트 강점

- 역할 분리가 명확한 분산형 구조
- MQTT 기반의 높은 확장성
- 중앙 정책 기반 자동 제어
- 웹 콘솔 기반 시각적 모니터링
- heartbeat와 status snapshot을 포함한 운영형 구조
- WS2812와 싱글 서보를 활용한 실제 동작 중심 구현

---

## 2.11 한계점 및 향후 계획

### 2.11.1 현재 한계
- 실제 배선과 전원 안정성에 따라 서보 동작이 민감할 수 있음
- MQTT reconnect 및 DNS 재시도 안정성이 더 보완될 필요가 있음
- 환경에 따라 임계값 보정이 추가로 필요할 수 있음

### 2.11.2 향후 개선 방향
- reconnect / 재시도 로직 강화
- heartbeat payload에 uptime 또는 상태 코드 추가
- 센서 데이터 저장 및 이력 시각화 추가
- 모바일 또는 알림 기능 확장
- `OLED/LCD` 상태 출력 추가
- 추가 센서 및 액추에이터 확장

발표에서는 "한계"와 "향후 계획"을 같은 슬라이드에 넣더라도, 현재 부족한 점과 다음 단계 목표를 분리해서 말하는 것이 더 자연스럽다.

---

## 2.12 결론

본 프로젝트는 센서 노드, 액추에이터 노드, 중앙관리 노드, 웹 콘솔을 MQTT 기반으로 분리하여 구성한 스마트 환경제어 하우스 시스템이다.  
`node_a`는 환경을 측정하고, `node_c`는 정책을 판단하며, `node_b`는 실제 장치를 제어하고, `web_console`은 상태 모니터링과 수동 제어를 제공한다.  
결과적으로 단순 센서 실습을 넘어, 역할 분리, 제어 정책, 운영 상태 확인까지 포함한 실제 시스템형 프로젝트로 정리할 수 있다.

---

## 3. PPT 구성 추천

현재 요구사항을 반영하면 PPT는 `12장` 구성이 가장 안정적이다.

### 추천 슬라이드 순서

1. 표지
2. 프로젝트 배경 및 목표
3. 프로젝트 개요
4. 전체 시스템 구성도
5. 팀원 역할 분담
6. 하드웨어 및 개발 환경
7. MQTT 통신 구조
8. AUTO 모드 플로우차트
9. MANUAL 모드 플로우차트
10. 웹 콘솔 및 핵심 코드 리뷰
11. 테스트 결과 및 프로젝트 강점
12. 한계점, 향후 계획, 결론

이 구성은 사용자가 들은 요구사항인 아래 요소를 모두 포함한다.

- 프로젝트 개요
- 팀원들이 수행한 역할
- 플로우 차트
- 향후 가능성 / 계획
- 핵심 코드 리뷰

---

## 4. 감마(Gamma)용 상세 발표 PPT 프롬프트

아래 프롬프트는 감마에 바로 넣을 수 있도록 발표 요구사항을 반영해 다시 정리한 버전이다.

```text
다음 정보를 바탕으로 대학 팀프로젝트 최종 발표용 PPT를 한국어로 만들어줘.

프로젝트명:
스마트 환경제어 하우스

부제:
MQTT 기반 분산 제어 시스템

발표 대상:
대학교 또는 교육기관 팀프로젝트 최종 발표

발표 시간:
약 7~10분

발표 목적:
- 프로젝트의 문제의식, 시스템 구조, 실제 구현 내용, 제어 로직, 테스트 결과, 확장 가능성을 명확하게 전달하는 것
- 단순 실습이 아니라 역할 분리와 통합이 이루어진 IoT 시스템이라는 점을 보여주는 것

전체 톤앤매너:
- 공학/소프트웨어 프로젝트 발표 스타일
- 지나치게 화려하지 않고 깔끔하고 전문적인 느낌
- 배경은 밝은 톤 또는 화이트 계열
- 강조색은 블루, 네이비, 그레이 계열
- 텍스트는 짧고 명확하게
- 한 슬라이드에 텍스트를 너무 많이 넣지 말 것
- 표, 계층도, 플로우차트, 카드형 요약, 아이콘을 적극 활용할 것
- 각 슬라이드의 핵심 메시지가 한눈에 보이게 구성할 것

프로젝트 핵심 내용:
- Raspberry Pi Pico 2 W 3대를 사용한 분산형 IoT 시스템
- node_a: 환경 센서 노드
- node_b: 액추에이터 노드
- node_c: 중앙관리 노드
- web_console: 상태 모니터링 및 수동 제어 웹 인터페이스
- 통신 방식: Wi-Fi + MQTT
- 주요 기능:
  - 조도, 온도, 습도 측정
  - 조도 기준 자동 조명 제어
  - 온도/습도 기준 자동 창문 제어
  - AUTO / MANUAL 모드 지원
  - 웹 콘솔을 통한 상태 확인 및 수동 제어
  - heartbeat 기반 노드 상태 확인

자동 제어 기준:
- light < 280 이면 조명 ON
- light > 320 이면 조명 OFF
- temp > 28 또는 humidity > 70 이면 창문 OPEN
- temp <= 27 그리고 humidity <= 65 이면 창문 CLOSE

프로젝트의 핵심 설계 포인트:
- 센서, 판단, 실행, UI를 분리한 역할 기반 구조
- AUTO 모드에서는 수동 명령을 무시하여 정책 일관성 유지
- 같은 상태 명령은 반복 발행하지 않는 중복 제어 방지
- node_b와 node_c가 각각 status snapshot을 발행하여 상태 동기화
- heartbeat와 timeout 경고를 통해 운영 상태 확인 가능

하드웨어 구성:
- Raspberry Pi Pico 2 W 3대
- CDS 조도 센서
- DHT 계열 온습도 센서
- WS2812 RGB Strip
- SG90 서보모터 1개
- 브레드보드, 점퍼선

핀 구성:
- node_a: GP15 DHT, GP27 CDS
- node_b: GP14 서보, GP16 WS2812
- node_c: 중앙 제어용 Pico 2 W

MQTT 주요 토픽:
- house/env
- house/mode
- house/cmd/light
- house/cmd/window
- house/status/nodeB
- house/status/nodeC
- house/heartbeat/nodeA
- house/heartbeat/nodeB
- house/heartbeat/nodeC

팀원 역할:
- 정귀인: node_c 중앙 제어 로직, MQTT 연동, 웹 콘솔 개발, 통합 테스트 정리
- 황지용: node_b 액추에이터 노드 구현, WS2812 및 싱글 서보 제어, 하드웨어 배선 및 모형 제작
- 김도경: node_a 센서 노드 작업, 센서 실습 및 연동 지원, PPT 초안 작성, 모형 제작

테스트 및 구현 결과:
- node_a + node_c + node_b + web_console 전체 MQTT 흐름 확인
- node_b + node_c + web_console 통합 테스트 확인
- web_console + node_c만으로도 상태 확인 및 수동 제어 가능
- node_c smoke test 통과
- heartbeat, status snapshot, 명령 반영 흐름 확인

향후 계획:
- MQTT reconnect 및 DNS 재시도 안정화
- heartbeat payload 확장
- 센서 데이터 저장 및 이력 시각화
- OLED/LCD 상태 표시 추가
- 추가 센서 및 액추에이터 확장

반드시 아래 슬라이드 구조를 따를 것.

1. 표지
- 제목: 스마트 환경제어 하우스
- 부제: MQTT 기반 분산 제어 시스템
- 팀명 또는 팀원명 표시
- 한 줄 소개: 센서, 중앙 제어기, 액추에이터, 웹 콘솔을 MQTT로 연결한 스마트 하우스 시스템
- 스마트 하우스 / IoT / 네트워크 느낌의 심플한 비주얼 사용

2. 프로젝트 배경 및 목표
- 단일 보드 중심 실습의 한계
- 센서, 판단, 실행, UI를 분리할 필요성
- MQTT 기반 분산 구조를 선택한 이유
- 프로젝트 목표를 3~4개 키워드로 요약
- 문제의식 -> 해결 방향 -> 목표 구조로 정리

3. 프로젝트 개요
- 시스템이 무엇을 하는지 한눈에 설명
- node_a 측정, node_c 판단, node_b 실행, web_console 관제의 역할을 짧게 정리
- 환경 감지 -> 중앙 판단 -> 장치 제어 -> 상태 모니터링 흐름을 시각적으로 표현

4. 전체 시스템 구성도
- node_a, node_b, node_c, web_console, MQTT Broker를 모두 포함
- 현장 계층 / 중앙 관리 계층 / 사용자 계층으로 나누어 표현
- MQTT Broker를 중앙 허브처럼 보이게 배치
- 각 요소 옆에 역할을 한 줄 설명으로 붙일 것

5. 팀원 역할 분담
- 팀원별 담당 영역과 실제 수행 내용을 표나 카드 형태로 정리
- 개인 역할이 보이되 협업 프로젝트라는 느낌이 나게 구성
- 구현, 통합, 문서화, 모형 제작을 균형 있게 보여줄 것

6. 하드웨어 및 개발 환경
- Pico 2 W 3대, CDS, DHT, WS2812, 싱글 서보, Flask, MQTT, C, Python 등을 표로 정리
- node_a / node_b / node_c 핀 구성도 간단히 포함
- WS2812 8픽셀 제어와 싱글 서보 기반 창문 구동을 강조

7. MQTT 통신 구조
- 주요 토픽을 표로 정리
- 누가 발행하고 누가 구독하는지 명확하게 표시
- env / cmd / status / heartbeat의 의미를 짧게 설명
- Node A -> Node C -> Node B 흐름이 보이도록 시각화

8. AUTO 모드 플로우차트
- 1) node_a 센서 측정
- 2) house/env 발행
- 3) node_c 수신
- 4) AUTO 여부 확인
- 5) light 기준 조명 판단
- 6) temp / humidity 기준 창문 판단
- 7) house/cmd/light, house/cmd/window 발행
- 8) node_b 장치 제어
- 9) node_b / node_c 상태 발행
- 10) web_console 상태 갱신
- 오른쪽 또는 아래에 light / temp / humidity 기준을 작은 표로 넣을 것

9. MANUAL 모드 플로우차트
- 1) 사용자가 web_console 접속
- 2) Mode Manual 선택
- 3) house/mode = MANUAL 발행
- 4) node_c가 MANUAL 전환
- 5) 수동 명령 허용
- 6) 사용자가 Light 또는 Window 버튼 선택
- 7) house/cmd/light 또는 house/cmd/window 발행
- 8) node_b가 실제 장치 제어
- 9) node_c가 상태 반영
- 10) node_b / node_c 상태 발행
- 11) web_console 상태 갱신
- AUTO 모드에서는 수동 명령이 무시된다는 점을 강조

10. 웹 콘솔 및 핵심 코드 리뷰
- 왼쪽에는 웹 콘솔 역할 정리:
  - Environment 표시
  - Controller(node_c) 상태 표시
  - Actuator(node_b) 상태 표시
  - Heartbeat 표시
  - 최근 MQTT 로그 표시
  - 수동 제어 버튼 제공
- 오른쪽에는 핵심 코드 리뷰 포인트 정리:
  - 임계값 기반 자동 제어
  - AUTO 모드에서 수동 명령 차단
  - 중복 명령 방지
  - status snapshot과 heartbeat 구조
- 실제 소스코드 일부를 2~4줄 정도의 짧은 snippet으로 2개에서 3개 포함할 것
- 추천 코드 예시는 아래 성격을 반영할 것:
  - node_c의 AUTO 제어 조건문
  - node_c의 AUTO 모드 수동 명령 차단 함수
  - node_b의 서보 또는 WS2812 제어 부분
- 코드 전체를 길게 넣지 말고 핵심 로직만 짧게 강조할 것

11. 테스트 결과 및 프로젝트 강점
- 확인된 통합 조합을 체크 형태로 정리
- 센서값 변화 -> 자동 제어 / 웹 버튼 -> 수동 제어 / 상태 반영 흐름을 요약
- 강점:
  - 역할 분리가 명확한 분산 구조
  - MQTT 기반 확장성
  - 운영 상태 확인이 가능한 heartbeat 구조
  - 실제 동작성이 높은 WS2812 + 싱글 서보 구현
- 로그 캡처나 결과 카드가 들어갈 자리도 고려

12. 한계점, 향후 계획, 결론
- 한계:
  - 배선 및 전원 안정성 이슈 가능
  - reconnect / DNS 재시도 보완 필요
  - 환경별 임계값 보정 필요
- 향후 계획:
  - 데이터 저장 및 이력 시각화
  - OLED/LCD 추가
  - 모바일/알림 기능 확장
  - 추가 센서/액추에이터 확장
- 결론:
  - 단순 센서 실습을 넘어 실제 시스템 구조를 갖춘 스마트 하우스 분산 제어 프로젝트라는 메시지로 마무리

추가 지시사항:
- 각 슬라이드 제목과 핵심 bullet을 한국어로 작성
- 긴 문단 대신 발표용 bullet과 짧은 설명 중심으로 구성
- 슬라이드 수는 약 11~12장으로 맞출 것
- 텍스트 40%, 시각 요소 60% 비율로 구성
- 다이어그램, 플로우차트, 표, 카드형 요약을 적극적으로 활용
- 논문 스타일이 아니라 설명 중심의 기술 발표 자료 스타일로 만들 것
```

---

## 5. 발표 자료에 실제로 넣으면 좋은 추가 요소

감마가 자동으로 잘 뽑더라도 아래 자료는 직접 넣는 편이 좋다.

- `img/system_flowchart.png`
- `img/auto_flowchart.png`
- `img/manual_flowchart.png`
- 웹 콘솔 실제 실행 화면 캡처
- MQTT 로그 캡처
- 모형 사진

즉, 감마로 기본 틀을 만든 뒤 실제 프로젝트 이미지와 캡처를 수동으로 교체하면 발표 완성도가 훨씬 높아진다.

---

## 6. 최종 판단

기존 파일은 프로젝트 이해 자체는 대체로 맞았지만, 발표 요구사항을 기준으로 보면 "기술서"에 치우쳐 있었고 "발표 설계" 요소가 조금 부족했다.  
이번 정리본은 실제 구현 내용과 요구된 PPT 항목을 함께 반영했기 때문에, 이 버전을 기준으로 발표 자료를 만들면 훨씬 자연스럽고 설득력 있게 구성할 수 있다.
