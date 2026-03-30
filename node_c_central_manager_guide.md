# 노드 C 개발 문서

## 1. 노드 C 역할

노드 C는 `중앙 관리 노드`이다.

주요 역할:

- 노드 A의 환경 데이터 수신
- 현재 모드 `AUTO` 또는 `MANUAL` 관리
- 센서값 기준 비교
- 조명 및 창문 제어 명령 생성
- 노드 B로 명령 전송
- 전체 상태 UART 출력

노드 C는 `전체 시스템의 판단과 연결 중심`이다.

---

## 2. 연결 장비

필수 장비:

- `Pico 2 W`

선택 장비:

- `버튼` 1개 이상
- `LCD/OLED`
- `상태 확인용 LED`

노드 C는 하드웨어보다 `로직과 통신`이 더 중요하다.

---

## 3. 노드 C 완료 목표

노드 C 담당자는 아래가 되면 1차 완료이다.

1. 전원 인가 후 부팅 로그가 나온다.
2. 노드 A의 환경 데이터를 수신하여 저장할 수 있다.
3. `AUTO` 모드에서 기준값에 따라 조명/창문 명령을 만든다.
4. `MANUAL` 모드에서 사용자의 명령을 받아 직접 제어한다.
5. 노드 B에 MQTT 명령을 정상 발행한다.
6. 현재 전체 상태를 UART에 출력한다.

---

## 4. 구현해야 할 세부 기능

### 4-1. 환경 데이터 수신

구독 토픽:

- `house/env`
- `house/status/nodeB`
- 필요 시 `house/heartbeat/nodeA`
- 필요 시 `house/heartbeat/nodeB`

수신한 센서 payload 예시:

```txt
light=275,temp=29.1,humidity=72.0
```

노드 C는 이 문자열을 파싱해서 내부 상태 변수에 저장해야 한다.

### 4-2. 모드 관리

모드는 반드시 아래 두 개만 사용한다.

- `AUTO`
- `MANUAL`

모드 변경 입력은 아래 중 하나로 구현한다.

- 버튼 입력
- UART 시리얼 명령
- 임시 테스트용 고정 코드

### 4-3. AUTO 제어 규칙

초기 기준은 아래로 고정한다.

- `light < 280` 이면 조명 켜기
- `light > 320` 이면 조명 끄기
- `temp > 28` 또는 `humidity > 70` 이면 창문 열기
- `temp <= 27` 그리고 `humidity <= 65` 이면 창문 닫기

중요한 점은 `상태가 바뀔 때만 명령 전송`하는 것이다.

예:

- 이미 조명이 켜져 있으면 또 `ON`을 보내지 않음
- 이미 창문이 열려 있으면 또 `OPEN`을 보내지 않음

### 4-4. MANUAL 제어 규칙

MANUAL 모드에서는 사용자가 직접 명령한다.

지원 명령:

- `MODE_AUTO`
- `MODE_MANUAL`
- `LIGHT_ON`
- `LIGHT_OFF`
- `WINDOW_OPEN`
- `WINDOW_CLOSE`

UART 입력 예시:

```txt
mode manual
light on
window open
mode auto
```

### 4-5. 명령 발행

발행 토픽:

- `house/cmd/light`
- `house/cmd/window`
- `house/mode`

payload:

- 조명: `ON`, `OFF`
- 창문: `OPEN`, `CLOSE`
- 모드: `AUTO`, `MANUAL`

### 4-6. 상태 출력

권장 UART 출력:

```txt
[NODE_C] MODE AUTO
[NODE_C] light=275 temp=29.1 humidity=72.0
[NODE_C] lamp=ON window=OPEN
```

가능하면 아래 정보가 모두 보이게 한다.

- 현재 모드
- 최신 조도
- 최신 온도
- 최신 습도
- 현재 조명 상태
- 현재 창문 상태
- Wi-Fi 상태

### 4-7. heartbeat 및 연결 감시

선택이지만 추천하는 기능:

- 일정 시간 동안 노드 A 데이터가 안 오면 경고 출력
- 노드 B 상태 응답이 없으면 경고 출력

예:

```txt
[NODE_C] WARN node A timeout
[NODE_C] WARN node B no response
```

---

## 5. 권장 개발 순서

노드 C는 아래 순서로 개발한다.

1. 상태 구조체 또는 변수 정리
2. 가짜 센서값을 넣어 AUTO 제어 로직 먼저 구현
3. UART 출력 정리
4. MANUAL 명령 처리 추가
5. Wi-Fi 연결 코드 추가
6. MQTT subscribe/publish 성공
7. 실제 환경 데이터 파싱 연결
8. 노드 B 명령 전송 연결
9. 중복 명령 방지 처리
10. timeout 및 예외 로그 정리

---

## 6. 로컬 단독 테스트 항목

노드 C는 다른 노드가 없어도 아래 테스트가 가능해야 한다.

### 테스트 1. 가상 센서값 기반 AUTO 테스트

코드 안에서 임시값을 넣어 아래를 검증한다.

- 어두우면 조명 ON 명령 생성
- 밝아지면 조명 OFF 명령 생성
- 덥거나 습하면 창문 OPEN 명령 생성
- 정상 범위면 창문 CLOSE 명령 생성

### 테스트 2. MANUAL 테스트

- UART 명령 또는 버튼으로 모드 전환이 되는지 확인
- MANUAL에서 직접 조명/창문 명령이 가능한지 확인

### 테스트 3. MQTT publish 테스트

- 실제 노드 B가 없어도 PC 쪽 subscribe로 명령 발행이 되는지 확인

### 테스트 4. 파싱 테스트

- 센서 문자열 예시를 넣었을 때 `light`, `temp`, `humidity`가 제대로 분리되는지 확인

---

## 7. 업로드 전 체크리스트

- AUTO/MANUAL 전환이 된다
- AUTO 조건이 합의안과 같다
- 명령 토픽이 합의안과 같다
- payload 값이 합의안과 같다
- 중복 명령 방지 처리가 있다
- UART에 상태가 정리되어 보인다
- 로그에 `NODE_C`가 포함된다

---

## 8. GitHub 업로드 시 커밋 예시

```bash
git checkout feature/node-c-controller
git add <수정한 파일>
git commit -m "feat: implement node C mode control and automation logic"
git push origin feature/node-c-controller
```

---

## 9. 통합 팀원에게 반드시 전달할 정보

- 실제 사용 중인 임계값
- 상태 판단 순서
- 수동 명령 입력 방식
- 토픽 목록
- 중복 명령 방지 방식
- timeout 판단 기준

---

## 10. 통합 시 주의사항

- 노드 A의 payload 키 이름이 다르면 파싱이 깨진다
- 노드 B가 동일 명령 재수신 시 재동작할지 무시할지 미리 맞춰야 한다
- AUTO와 MANUAL 전환 시 이전 상태를 어떻게 유지할지 팀 내 합의가 필요하다

권장 정책:

- AUTO -> MANUAL 전환 시 현재 장치 상태 유지
- MANUAL -> AUTO 복귀 시 즉시 센서값 기준으로 다시 판단

---

## 11. 노드 C 최종 인수 기준

아래가 가능하면 노드 C는 통합 준비 완료이다.

- 독립 실행 가능
- 가상 데이터 기반 AUTO 동작 검증 완료
- MANUAL 명령 처리 가능
- MQTT 송수신 가능
- 전체 상태 출력 가능
- 통합 시 필요한 규칙이 문서화되어 있음
