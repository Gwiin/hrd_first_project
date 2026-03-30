# 노드 B 개발 문서

## 1. 노드 B 역할

노드 B는 `장치 제어 노드`이다.

주요 역할:

- 조명 역할의 `LED` 제어
- 환기창 역할의 `서보모터` 제어
- 중앙 관리 노드의 명령 수신
- 현재 장치 상태 UART 출력
- 필요 시 현재 장치 상태를 MQTT로 회신

노드 B는 `명령을 정확하게 받고 안전하게 동작시키는 것`이 핵심이다.

---

## 2. 연결 장비

필수 장비:

- `Pico 2 W`
- `LED`
- `저항`
- `서보모터`
- `브레드보드`
- `점퍼선`

선택 장비:

- 로컬 테스트용 `버튼`
- 상태 알림용 `부저`

---

## 3. 노드 B 완료 목표

노드 B 담당자는 아래가 되면 1차 완료이다.

1. 전원 인가 후 부팅 로그가 나온다.
2. 테스트 코드만으로 LED ON/OFF가 된다.
3. 테스트 코드만으로 서보 OPEN/CLOSE 동작이 된다.
4. MQTT 명령을 받아 LED와 서보를 제어할 수 있다.
5. 현재 장치 상태를 UART에 출력할 수 있다.
6. 명령이 잘못 들어오면 무시하거나 오류 로그를 남긴다.

---

## 4. 구현해야 할 세부 기능

### 4-1. LED 제어

- `LIGHT_ON` 명령에 따라 LED를 켠다.
- `LIGHT_OFF` 명령에 따라 LED를 끈다.
- 현재 상태는 내부 변수로 저장한다.

권장 내부 상태:

- `lamp_state = ON | OFF`

### 4-2. 서보 제어

- `WINDOW_OPEN` 명령에 따라 서보를 열린 각도로 이동한다.
- `WINDOW_CLOSE` 명령에 따라 서보를 닫힌 각도로 이동한다.
- 각도 값은 코드 상단 상수로 관리한다.

권장 예시:

- `SERVO_OPEN_ANGLE`
- `SERVO_CLOSE_ANGLE`

### 4-3. 명령 수신

권장 구독 토픽:

- `house/cmd/light`
- `house/cmd/window`

payload:

- `ON`
- `OFF`
- `OPEN`
- `CLOSE`

### 4-4. 로컬 테스트 모드

MQTT 연결 전에도 검증할 수 있도록 아래 중 하나를 준비한다.

- 버튼을 누를 때마다 LED 토글
- 버튼을 누를 때마다 서보 OPEN/CLOSE 전환
- UART 명령으로 테스트

### 4-5. 상태 보고

권장 토픽:

- `house/status/nodeB`

예시 payload:

```txt
lamp=ON,window=OPEN,result=OK
```

### 4-6. UART 로그 출력

권장 형식:

```txt
[NODE_B] WIFI OK
[NODE_B] CMD LIGHT ON
[NODE_B] CMD WINDOW OPEN
[NODE_B] lamp=ON window=OPEN
```

---

## 5. 권장 개발 순서

노드 B는 아래 순서로 개발한다.

1. LED 단독 제어 성공
2. 서보 단독 제어 성공
3. LED와 서보를 한 코드에 묶기
4. 상태 변수 정리
5. UART 테스트 명령 추가
6. Wi-Fi 연결 코드 추가
7. MQTT subscribe 성공
8. 고정 명령 수신 테스트
9. 실제 명령에 따라 장치 동작 테스트
10. 상태 보고 및 예외 처리 추가

---

## 6. 로컬 단독 테스트 항목

### 테스트 1. LED 기본 동작

- 부팅 후 테스트 함수로 LED ON/OFF 확인
- 명령 없이도 GPIO 연결이 올바른지 먼저 검증

### 테스트 2. 서보 기본 동작

- OPEN 각도와 CLOSE 각도가 확실히 구분되는지 확인
- 전원 부족으로 떨림이 심하면 배선과 전원 상태를 먼저 점검

### 테스트 3. UART 명령 테스트

- 시리얼 입력 또는 고정 테스트 코드로
  - `LIGHT_ON`
  - `LIGHT_OFF`
  - `WINDOW_OPEN`
  - `WINDOW_CLOSE`
  동작을 확인

### 테스트 4. MQTT 명령 테스트

- PC 또는 다른 노드에서 토픽에 직접 publish 하여 장치가 움직이는지 확인

### 테스트 5. 잘못된 명령 테스트

- 이상한 문자열을 보냈을 때 멈추지 않고 무시하는지 확인

---

## 7. 업로드 전 체크리스트

- LED 제어가 안정적이다
- 서보 OPEN/CLOSE가 안정적이다
- 토픽 이름이 합의안과 같다
- payload 값이 합의안과 같다
- 잘못된 명령에 대한 처리 로그가 있다
- 현재 장치 상태를 UART로 확인할 수 있다
- 노드 이름 `NODE_B`가 로그에 포함된다

---

## 8. GitHub 업로드 시 커밋 예시

```bash
git checkout feature/node-b-actuator
git add <수정한 파일>
git commit -m "feat: implement node B actuator control and command subscribe"
git push origin feature/node-b-actuator
```

---

## 9. 통합 팀원에게 반드시 전달할 정보

- LED 핀 번호
- 서보 PWM 핀 번호
- 서보 OPEN/CLOSE 각도 값
- 전원 연결 시 주의사항
- 상태 보고 payload 예시
- 명령 수신 후 반응 시간

---

## 10. 통합 시 주의사항

- 서보는 전원 상태에 민감하므로 배선이 흔들리면 오동작할 수 있다
- `OPEN`, `CLOSE` 대신 다른 문자열을 쓰면 통합이 깨진다
- 노드 C가 상태 중복 명령을 많이 보내면 서보가 계속 움직일 수 있으니, 같은 상태일 때는 재동작하지 않도록 처리하는 것이 좋다

---

## 11. 노드 B 최종 인수 기준

아래가 가능하면 노드 B는 통합 준비 완료이다.

- 독립 실행 가능
- LED 단독 제어 가능
- 서보 단독 제어 가능
- MQTT 명령 수신 가능
- 현재 장치 상태 확인 가능
- 잘못된 명령을 받아도 시스템이 멈추지 않음
