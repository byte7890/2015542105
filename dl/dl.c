#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

char type(mode_t);
char *perm(mode_t);
void printStat(char*,char*, struct stat*);
void direct(struct stat*);
/* 디렉터리 내의 파일 이름들을 리스트한다. */
int main(int argc, char **argv) 
{
	DIR *dp;
	char *dir;
	struct dirent *d;
	struct stat st;
	char path[BUFSIZ+1];
	
	if (argc == 2|| argc == 1) 
		dir = "."; 	// 현재 디렉터리를 대상으로 
	else dir = argv[2];

	if(argc == 1){
		if ((dp = opendir(dir)) == NULL) {  // 디렉터리 열기
			perror(dir);
			exit(1);
		}
		while ((d = readdir(dp)) != NULL)	// 디렉터리 내의 각 엔트리에 대해
			if(strcmp(d->d_name,".") == 0 || strcmp(d->d_name,"..") == 0)
				continue;
			else
				printf("%s \n", d->d_name);    // 파일 이름 프린트
		closedir(dp);
	}
	else if(strcmp(argv[1],"-a") == 0){
		if ((dp = opendir(dir)) == NULL) {  // 디렉터리 열기 
			perror(dir);
			exit(1);
		}
	
		while ((d = readdir(dp)) != NULL)  // 디렉터리 내의 각 엔트리에 대해 
			printf("%s \n", d->d_name);    // 파일 이름 프린트 
	
		closedir(dp);
	}
	else if(strcmp(argv[1],"-l") == 0){
		if ((dp = opendir(dir)) == NULL)  // 디렉토리 열기 
			perror(dir);

		while ((d = readdir(dp)) != NULL) {	// 디렉토리 내의 각 파일에 대해 
			sprintf(path, "%s/%s", dir, d->d_name); // 파일 경로명 만들기 
			if (lstat(path, &st) < 0) 	// 파일 상태 정보 가져오기  
				perror(path);
			else 
				printStat(path, d->d_name, &st);  // 상태 정보 출력
		}
		
		closedir(dp);
	}
	exit(0);
}

void printStat(char *pathname, char *file, struct stat *st) {

	printf("%5ld ", st->st_blocks);
	printf("%c%s ", type(st->st_mode), perm(st->st_mode));
	printf("%3ld ", st->st_nlink);
	printf("%s %s ", getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name);
	printf("%9ld ", st->st_size);
	printf("%.12d ", ctime(&st->st_mtime)+4);
	printf("%s\n", file);
}

/* 파일 타입을 리턴 */
char type(mode_t mode) {
	
	if (S_ISREG(mode)) 
		return('-');
	if (S_ISDIR(mode)) 
		return('d');
	if (S_ISCHR(mode)) 
		return('c');
	if (S_ISBLK(mode)) 
		return('b');
	if (S_ISLNK(mode)) 
		return('l');
	if (S_ISFIFO(mode)) 
		return('p');
	if (S_ISSOCK(mode)) 
		return('s');
}

/* 파일 허가권을 리턴 */
char* perm(mode_t mode) {
	int i;
	static char perms[10]; 

	strcpy(perms, "---------");
	
	for (i=0; i < 3; i++) {
		if (mode & (S_IREAD >> i*3)) 
			perms[i*3] = 'r';
		if (mode & (S_IWRITE >> i*3)) 
			perms[i*3+1] = 'w';
		if (mode & (S_IEXEC >> i*3)) 
			perms[i*3+2] = 'x';
	}
	return(perms);
}
