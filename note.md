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


## 브랜치 운영 가이드

### 1. 브랜치 구조
- `main`: 최종 배포용 브랜치
- `develop`: 팀 개발 내용을 모으는 브랜치
- `feature/이름-작업`: 각자 기능 작업 브랜치

예시
- `feature/gwiin-docs`
- `feature/hjy-sensor`
- `feature/dog0-ui`

개인 브랜치는 반드시 `develop` 브랜치에서 생성해서 사용한다.

### 2. 처음 세팅할 때

개인 브랜치는 `develop` 기준으로 생성한다.

```bash
git switch develop
git pull origin develop
git switch -c 브랜치이름  #생성시만 -c 
git push -u origin 브랜치이름 #첫 push 할때만 -u 하면 됨
```

예시

```bash
git switch develop
git pull origin develop
git switch -c gwiin
git push -u origin gwiin
```

### 3. 매일 작업 시작할 때
먼저 `develop`을 최신화하고, 그 다음 내 브랜치로 이동한다.

```bash
git switch develop
git pull origin develop
git switch 브랜치이름
```

### 4. 내 브랜치에 develop 최신 내용 반영하기
다른 팀원의 작업이 `develop`에 반영되었으면 내 브랜치에도 가져와야 충돌을 줄일 수 있다.

```bash
git fetch origin
git switch 브랜치이름
git merge origin/develop
```

충돌이 나면 파일을 수정한 뒤 아래 순서로 마무리한다.

```bash
git add .
git commit
```

### 5. 내 브랜치에서 작업 후 커밋, 푸시하기

```bash
git switch 브랜치이름
git add .
git commit -m "작업 내용"
git push
```

처음 푸시할 때만 `-u` 옵션이 필요하고, 그 이후에는 `git push`만 하면 된다.

### 6. 내 브랜치 내용을 develop에 반영하기
가장 권장하는 방법은 GitHub에서 PR을 만드는 것이다.

순서
- 내 브랜치에서 작업 후 `git push`
- GitHub에서 `feature/본인이름-작업명` -> `develop` 으로 PR 생성
- 리뷰 후 merge

로컬에서 직접 반영해야 한다면 아래 순서로 진행한다.

```bash
git switch develop
git pull origin develop
git merge 브랜치이름
git push origin develop
```

주의
- `git push origin develop`만 바로 하는 방식은 사용하지 않는다.
- 먼저 `develop` 브랜치로 이동한 뒤 merge 하고 push 해야 한다.

### 7. develop 내용을 main에 반영하기
배포할 때는 `develop`에서 테스트를 마친 뒤 `main`으로 반영한다.

```bash
git switch main
git pull origin main
git merge develop
git push origin main
```

실제 운영에서는 `develop` -> `main`도 PR 방식으로 진행하는 것을 권장한다.

### 8. 팀 공통 규칙
- 개인 작업은 `main`이 아니라 `develop`에서 분기한다.
- 작업 중간중간 `develop` 최신 내용을 내 브랜치에 반영한다.
- 기능이 끝나면 개인 브랜치에서 바로 `main`으로 가지 않고 `develop`으로 먼저 합친다.
- 배포 직전에는 `develop`에서 테스트 후 `main`으로 반영한다.
- 브랜치 이름은 가능하면 `feature/이름-작업명` 형식으로 통일한다.

### 9. 가장 자주 쓰는 명령어 요약

작업 시작

```bash
git switch develop
git pull origin develop
git switch 브랜치이름
```

develop 최신 내용 가져오기

```bash
git fetch origin
git switch 브랜치이름
git merge origin/develop
```

작업 후 푸시

```bash
git add .
git commit -m "작업 내용"
git push
```

develop 반영

```bash
git switch develop
git pull origin develop
git merge 브랜치이름
git push origin develop
```
