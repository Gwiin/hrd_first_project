F1키 -> git clone -> Gwiin/hrd_first_project 선택 -> 새창에서 열기(선택)

[깃허브 프로젝트](https://miro7923.github.io/uno%20mas/teamwork-by-github/)


브랜치 생성
git checkout -b 브랜치이름 (첫생성시)
git checkout 브랜치이름 (브랜치 이동)

git branch -a (내 브랜치 현황) 
```shell
git checkout -b 브랜치 이름 #첫 생성시
git checkout 브랜치이름 #해당 브랜치로 이동

git branch -a #현재 내 로컬 브랜치
```
git branch -a 했을때 자기 브랜치이름에 `*` 있다면 작업가능<br>
만약 `*main`이라면 `git checkout 자기브랜치이름` 해서 이동 해야함

```shell
git branch --set-upstream-to origin/main #main branch에 연동



```

## Git 브랜치 운영 규칙

- `main`: 배포용 브랜치 (직접 작업/커밋 금지)
- `develop`: 통합 개발 브랜치
- 개인 작업 브랜치: `feature/...`, `fix/...`
- 모든 작업은 개인 브랜치에서 진행 후 `develop`으로 PR

## 팀원 작업 루틴

### 1) 최초 1회 세팅

```bash
git clone https://github.com/Gwiin/hrd_first_project.git
cd hrd_first_project
git fetch origin
git switch develop   # 없으면: git switch -c develop --track origin/develop
```

### 2) 새 작업 시작 전 (항상)

```bash
git switch develop
git pull origin develop
git switch -c feature/작업명
# 또는
git switch -c fix/버그명
```

### 3) 작업 후 커밋/푸시

```bash
git add .
git commit -m "feat: 작업 내용"
git push -u origin feature/작업명   # 처음 1회
# 이후 같은 브랜치 추가 푸시
git push
```

### 4) PR 및 반영

1. GitHub에서 `feature/...` 또는 `fix/...` -> `develop` 으로 PR 생성
2. 리뷰 후 `develop`에 머지
3. 로컬 최신화

```bash
git switch develop
git pull origin develop
```

## 팀 공통 체크리스트

- 작업 시작 전 `develop` 최신화 (`git pull origin develop`)
- `main` 브랜치 직접 push 금지
- 개인 브랜치에서만 작업
- 머지는 PR로만 진행