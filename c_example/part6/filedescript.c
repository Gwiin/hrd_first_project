#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    int fd;
    char *path = "//home/jyhwang0631/jyhwang0631/c_example/part6/";
    char fPath[100];
    sprintf(fPath, "%s%s", path, "/test.dat");
    fd = open(fPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        printf("파일을 열수 없다.\n");
    write(fd, "이것은 파일로 저장되는 예시 데이터 입니다.!", 62);
    close(fd);
    return 0;
}