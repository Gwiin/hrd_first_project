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