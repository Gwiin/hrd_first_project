F1키 -> git clone -> Gwiin/hrd_first_project 선택 -> 새창에서 열기(선택)

[깃허브 프로젝트](https://miro7923.github.io/uno%20mas/teamwork-by-github/)



## 각자 브랜치 운영 방식 (1번부터)

1. 기준 브랜치
- `main`: 배포용
- `develop`: 팀 통합 개발용
- 개인 작업: `feature/이름-작업` (예: `feature/minsu-login`)

2. 작업 시작 루틴 (각자)
```bash
git switch develop
git pull origin develop
git switch 본인브랜치명
```

3. 작업 중 루틴 (충돌 줄이기)
```bash
git fetch origin
git switch 본인브랜치명
git merge origin/develop
```

4. 기능 단위로 자주 PR
- 작업을 너무 크게 쪼개지 말고 기능 단위로 `develop`에 PR
- 리뷰 후 머지

5. 통합 직전/최종 배포 전
- `develop`에서 통합 테스트
- 문제 없으면 `main`으로 PR 후 배포
