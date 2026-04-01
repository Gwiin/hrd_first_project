# hrd_first_project
고려대 세종 HRD 개발자 교육 1차 프로젝트 Repo.

## 현재 기준 요약

현재 저장소 기준으로 바로 이어서 보기 좋은 핵심 상태는 아래와 같다.

- `node_b` 기본 소스 포함
- `node_c` 중앙관리노드 구현 및 MQTT 연동 포함
- `web_console` Flask + MQTT 기반 웹 콘솔 포함
- `node_a`, `node_b` 없이도 `web_console + node_c` 조합으로 단독 상태 확인 및 수동 제어 가능

주요 참고 위치:

- [node_c/README.md](/home/asd/hrd_first_project/node_c/README.md)
- [node_c/node_c_handover_log.md](/home/asd/hrd_first_project/node_c/node_c_handover_log.md)
- [node_c/node_c_test_report.md](/home/asd/hrd_first_project/node_c/node_c_test_report.md)
- [web_console/README.md](/home/asd/hrd_first_project/web_console/README.md)

## 1차 프로젝트 일지
|일자|요일|타임|내용|
|---|---|---|---|
|3/30|월|오전|아이디어 회의|
|.|.|오후|아이디어 선정 및 역할 분담|
|3/31|화|오전|각 파트별 기능 구현 조사|
|.|.|오후|파트별 pico2w 보드 실험|
|4/1|수|오전|`node_c` MQTT 개선, `web_console` 1차 구현, UF2 생성 정리|
|.|.|오후|`web_console + node_c` 단독 테스트 및 문서 정리|
|4/2|목|오전||
|.|.|오후||
|4/3|금|오전||
|.|.|오후||

---

