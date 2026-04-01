# hrd_first_project
고려대 세종 HRD 개발자 교육 1차 프로젝트 Repo.

## 현재 기준 요약

현재 저장소 기준으로 바로 이어서 보기 좋은 핵심 상태는 아래와 같다.

- `node_b` MQTT 액추에이터 노드 구현 포함
- `node_c` 중앙관리노드 구현 및 MQTT 연동 포함
- `web_console` Flask + MQTT 기반 웹 콘솔 포함
- `node_a`, `node_b` 없이도 `web_console + node_c` 조합으로 단독 상태 확인 및 수동 제어 가능
- `node_b + node_c + web_console` 통합 테스트까지 진행
- `house/env` 임의 환경값 발행 시 `node_c`가 자동 판단하고 `node_b` 액추에이터가 반응하는 흐름 확인

주요 참고 위치:

- [node_b/README.md](/home/asd/hrd_first_project/node_b/README.md)
- [node_c/README.md](/home/asd/hrd_first_project/node_c/README.md)
- [node_c/node_c_handover_log.md](/home/asd/hrd_first_project/node_c/node_c_handover_log.md)
- [node_c/node_c_test_report.md](/home/asd/hrd_first_project/node_c/node_c_test_report.md)
- [web_console/README.md](/home/asd/hrd_first_project/web_console/README.md)

현재 업로드용 펌웨어:

- `node_b`: [node_b/build/node_b.uf2](/home/asd/hrd_first_project/node_b/build/node_b.uf2)
- `node_c`: [node_c/build/node_c.uf2](/home/asd/hrd_first_project/node_c/build/node_c.uf2)

현재 하드웨어/통합 메모:

- `node_b`는 `GP14` 서보, `GP16` 액추에이터 출력 사용
- `GP16` 연결 장치가 일반 LED가 아니라 `WS2812 RGB Strip`인 경우 단순 GPIO 제어로는 동작하지 않음
- 현재 `node_b`는 `WS2812` 8픽셀 전체를 `lamp ON/OFF` 상태에 맞춰 제어하도록 수정됨

## 1차 프로젝트 개발 일지
|일자|요일|타임|내용|
|---|---|---|---|
|3/30|월|오전|아이디어 회의|
|.|.|오후|아이디어 선정 및 역할 분담|
|3/31|화|오전|정귀인 : 중앙관리노드 코드 작성 <br>황지용 : LCD, 액추에이터, 센서 연결 및 실습<br>김도경 : LCD, 센서 연결 및 실습|
|.|.|오후|정귀인 : 중앙관리노드 MQTT 작성 및 실습<br>황지용 : LCD, 액추에이터, 센서 연결 및 실습<br>김도경 : LCD, 센서 연결 및 실습|
|4/1|수|오전|정귀인 : 웹 콘솔 개발 및 테스트<br>황지용 : 액추에이터노드 코드 작성<br>김도경 : 센서노드 코드 작성|
|.|.|오후|정귀인 : `node_c` MQTT/웹 콘솔/통합 테스트 정리<br>황지용 : `node_b` MQTT 액추에이터 노드 통합 및 WS2812 스트립 테스트<br>김도경 : 센서노드 작업|
|4/2|목|오전||
|.|.|오후||
|4/3|금|오전||
|.|.|오후||

---
