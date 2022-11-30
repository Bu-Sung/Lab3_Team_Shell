/*
쉘 프로그램 제작
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/resource.h>
#include <sys/types.h> 
#include <dirent.h>
#include <sys/stat.h>


#define FILE_SIZE 1024 //cat 사용시 파일 사이즈
#define BUF_SIZE 256
#define STR_ARR 50

#define STDIN 0
#define STDOUT 1
#define STDERR 2

int main()
{
    	  char buf[256];
    	  char *argv[STR_ARR]={NULL,};
    	  char str[STR_ARR];
    	  pid_t pid;
    	  int argc;

	  // 3. csh, bash 등에서처럼 인터럽트키 (SIGINT: Ctrl-C, SIGQUIT: Ctrl-Z) 가 동작하도록 프로그램을 수정하시오.
	  Signal_INT();
	  Signal_QUIT();

    	  
   	  while(1){

		argc = 0;
		
    	  	printf("\n[ My Shell Program ]\n\n");
    	  	printf("Shell> ");
    	  	fgets(str, sizeof(str), stdin);
    	  	str[strlen(str)-1] = '\0';
    	  	
    	  	char *temp = strtok(str, " ");
    	  	
    	  	while(temp != NULL)
    	  	{
    	  		argv[argc] = temp;
    	  		temp = strtok(NULL, " ");
    	  		argc++;
    	  	}
    	  	
		
    	  		// 1. "exit"를 치면 프로그램을 끝내도록 프로그램을 수정하시오.
			if(strcmp("exit", argv[0]) == 0)
			{
				printf("\n프로그램이 종료됩니다.\n\n");
				exit(1);
			}
			
			//ls();
			if(strcmp("ls", argv[0]) == 0)
			{
				ls();
			}


			//pwd();
			if(strcmp("pwd", argv[0]) == 0)
			{
				pwd();
			}


			//cd(argc, argv);
			if(strcmp("cd", argv[0]) == 0)
			{
				cd(argc, argv);
			}


			//new_dir(argv);
			if(strcmp("mkdir", argv[0]) == 0)
			{
				new_dir(argv);
			}


			//remove_dir(argv);
			if(strcmp("rmdir", argv[0]) == 0)
			{
				remove_dir(argv);
			}


			//cp(argc,argv);
			if(strcmp("cp", argv[0]) == 0)
			{
				cp(argc, argv);
			}


			//mv(argc,argv);
			if(strcmp("mv", argv[0]) == 0)
			{
				mv(argc,argv);
			}


			//rm(argv);
			if(strcmp("rm", argv[0]) == 0)
			{
				rm(argv);
			}


			//ln(argc,argv);
			if(strcmp("ln", argv[0]) == 0)
			{
				ln(argc, argv);
			}
    	 	
    	 	
    	 		//cat(argc,argv);
			if(strcmp("cat", argv[0]) == 0)
			{
				cat(argc, argv);
			}
			
			/*
			if(strcmp(">", argv[1]) == 0 || strcmp("<", argv[1]) == 0)
			{
				parse_redirect(argv);
			}
			*/
			
			
    	  }
    	  
}



// 2. csh, bash 등에서처럼 쉘 명령의 마지막에 ‘&’을 입력하면 백그라운드로 실행되도록 프로그램을 수정하시오.
void daemonize(char *cmd)
{

	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;

	umask(0) ; /* clear out file creation mask */

	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		perror("getrlimit");

	if ((pid = fork()) < 0)
		exit (1);
	else if (pid != 0)
		exit(0) ; /* The parent process exits */
	
	setsid() ; /* become session leader */

	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGHUP, &sa, NULL) < 0)
		perror("sigaction: can't ignore SIGHUP");
	if ((pid = fork()) < 0)
		exit (1);
	else if (pid != 0)
		exit(0) ; /* The parent process exits */
	
	chdir("/") ; /* change the working dir */

	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0); fd2 = dup(0);
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
	}

}


// 3. csh, bash 등에서처럼 인터럽트키 (SIGINT: Ctrl-C, SIGQUIT: Ctrl-Z) 가 동작하도록 프로그램을 수정하시오.
void handler_int(int signo){
    printf("\nSIGINT: Ctrl-C\n");
    pid_t pid_1 = getpid();
    kill(pid_1, SIGINT);
}

void handler_quit(int signo){
    printf("\nSIGQUIT: Ctrl-Z\n");
    pid_t pid_1 = getpid();
    kill(pid_1, SIGQUIT);
}

void Signal_INT(){
    struct sigaction s_int;

    // signal int
    s_int.sa_handler = handler_int; 
    sigfillset(&(s_int.sa_mask));
    sigaction(SIGINT, &s_int, NULL);
}

void Signal_QUIT(){
    struct sigaction s_quit;

    // signal quit
    s_quit.sa_handler = handler_quit;
    sigfillset(&(s_quit.sa_mask));
    sigaction(SIGTSTP, &s_quit, NULL);
}


/*
// 4. 파일 재지향 (>, <) 및 파이프(|) 기능이 가능하도록 프로그램을 수정하시오.

void parse_redirect(char* argv[])
{

    int cmdlen = strlen(argv);
    int fd, i;

    printf(argv[0]);
    printf("\n");
    printf(argv[1]);
    printf("\n");
    printf(argv[2]);
    printf("\n");

		
		if(strcmp("<", argv[1]) == 0)

			if( (fd = open(argv[2], O_RDONLY | O_CREAT, 0644)) < 0) 
            			printf("파일 열기에 실패하였습니다.");
			dup2(fd, STDIN_FILENO);
			close(fd);
			argv[1] = '\0';

        		
		if(strcmp(">", argv[1]) == 0)

			if( (fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
           			printf("파일 열기에 실패하였습니다.");
			dup2(fd, STDOUT_FILENO);
			close(fd);
			argv[1] = '\0';

     
    
}

*/

/*
// 5. ls, pwd, cd, mkdir, rmdir, ln, cp, rm, mv, cat 명령을 팀원이 공평하게 나누어 구현하시오.
*/

void ls(); //현재 디렉토리 파일 출력
void pwd(); //현재 디렉토리 경로 출력
void cd(int argc, char* argv[]); //디렉토리 위치 변경
void new_dir(); //새로운 디렉토리 생성
void remove_dir(char* argv[]); //디렉토리 삭제
void cp(int argc, char* argv[]);//파일 복사
void mv(int argc, char* argv[]);//파일 이동 및 파일 rename
void rm(char* argv[]); //파일 삭제
void ln(int argc, char* argv[]); //링크
void cat(int argc, char* argv[]); //cat


void cat(int argc, char* argv[]){


	int fd;


	long size;


        unsigned char buf[BUF_SIZE];


	if( argc < 2 || argc >2){


		printf("명령어 규칙에 맞지 않습니다.\n");


		exit(1);


	}


	if(argc == 2){ //파일 출력 ex) cat test.txt


		if( ( fd = open(argv[1], O_RDONLY)) < 0){ // 파일이 존재하지 않을 때


	                printf("파일이 존재하지 않습니다.\n");


	                exit(1);


	        }


		while (size = read(fd, buf, BUF_SIZE)){//파일 안 내용을 출력


		        write(1, buf, size);


		}


	}


	/* cat > 기능 미 완성


	}else if(argc == 3){


		printf("%d\n",strcmp(argv[2], ">"));


		if(strcmp(argv[2], ">")==0){ //argv[2]의 파일 명으로 새 파일 생성 후 EOF까지 입력 받아서 저장 ex) cat > test.txt


			FILE *fp; // FILE 포인터 변수 선언.


			printf("%s\n",argv[2]);


			if((fp = fopen(argv[2], "W"))==NULL){


				printf("파일 열기 오류입니다.");


				exit(1);


			}


			char c;


			while ((c = getchar()) != EOF) {


				putchar(c);


			}


			


		} else{ // > 이외에 명령어 입력시


			printf("'%s' 명령어가 존재하지 않습니다.", argv[1]);


			exit(1);


		}


	}*/


}





void ln(int argc, char* argv[]){


	char cmd; 


	char *src; 


	char *target; 


	if (argc < 3) { 


		fprintf(stderr, "Usage: file_link [l,u,s] ...\n"); 


		fprintf(stderr, " file_link l[ink] src target\n"); 


		fprintf(stderr, " file_link u[nlink] filename\n"); 


		fprintf(stderr, " file_link s[ymlink] src target\n"); 


		exit(1); 


	} 


	cmd = (char) *argv[1]; 


	if ( cmd == 'l' ) { 


		if (argc < 4) { 


			fprintf(stderr, "file_link l src target [link]\n"); 


			exit(1); 


		} 


		src = argv[2]; 


		target = argv[3]; 


		if (link(src, target) < 0) { 


			perror("link"); 


			exit(1); 


		} 


	} 


	else if (cmd == 's') { 


		if (argc < 4) { 


			fprintf(stderr, "file_link l src target [link]\n"); 


			exit(1); 


		} 


		src = argv[2]; 


		target = argv[3];


		if (symlink(src, target) < 0) { 


			perror("symlink"); 


			exit(1); 


		} 


	} 


	else if (cmd == 'u') { 


		src = argv[2]; 


		if (unlink(src) < 0) { 


			perror("unlink"); 


			exit(1); 


		} 


	} 


	else { 


		fprintf(stderr, "Unknown command...\n"); 


	}


}





void rm(char* argv[]){


	int success = remove(argv[1]); //파일 삭제 함수


	if(success == 0) {//성공시


		printf("파일을 삭제하였습니다.\n");


		ls(); //성공하여 없어진 파일 목록을 출력


	}


	else	//실패시


		printf("파일 삭제에 실패하였습니다.\n");


}





void mv(int argc, char* argv[]){ //파일 이동 및 파일 rename


	struct stat buf; 


	char *target; 


	char *src_file_name_only; 


	if (argc < 3) { 


		fprintf(stderr, "Usage: file_rename src target\n"); 


		exit(1); 


	} 


	// Check argv[1] (src) whether it has directory info or not. 


	if (access(argv[1], 0) < 0) { 


		fprintf(stderr, "%s not exists\n", argv[1]); 


		exit(1); 


	} 


	else { 


		char *slash = strrchr(argv[1], '/'); 


		src_file_name_only = argv[1]; 


		if (slash != NULL) { // argv[1] has directory info. 


			src_file_name_only = slash + 1; 


		} 


	}


	 


	// Make target into a file name if it is a directory 


	target = (char *)calloc(strlen(argv[2])+1, sizeof(char)); 


	strcpy(target, argv[2]); 


	if (access(argv[2], 0) == 0) { 


		if (lstat(argv[2], &buf) < 0) { 


			perror("lstat"); 


			exit(1); 


		} 


		else { 


			if (S_ISDIR(buf.st_mode)) { 


				free(target); 


				target = (char *) calloc(strlen(argv[1]) + strlen(argv[2]) + 2, sizeof(char)); 


				strcpy(target, argv[2]); 


				strcat(target, "/"); 


				strcat(target, src_file_name_only); 


			} 


		}


	}


	printf("source = %s\n", argv[1]); 


	printf("target = %s\n", target); 


	if (rename(argv[1], target) < 0) { 


		perror("rename"); 


		exit(1); 


	} 


	free(target);


}


void cp(int argc, char* argv[]){ //파일 복사 함수


	FILE *src; /* source file */


	FILE *dst; /* destination file */


	char ch;


	int count = 0;


	if (argc < 3) {


		printf("Usage: file_copy source_filedestination_file\n");


		exit(1);


	}	


	if ( (src = fopen(argv[1], "r")) == NULL ) {


		perror("fopen: src");


		exit(1);


	}


	if ( (dst = fopen(argv[2], "w")) == NULL ) {


		perror("fopen: dst");


		exit(1);


	}


	while ( !feof(src) ) {


		ch = (char) fgetc(src);


		if ( ch != EOF )


			fputc((int)ch, dst);


		count++;


	}


	fclose(src);


	fclose(dst);


}





void new_dir(char* argv[]){
  char str[100];
  char buf[256];
  if(getcwd(buf,256) == NULL){ //반환 실패시 NULL을 반환
     printf("현재 디렉토리 경로를 찾을 수 없습니다.");
     exit(1);
  }
  sprintf(str,"%s/%s",buf,argv[1]);
  printf("%s",str);
  int success = mkdir(str,0777);// 디렉토리 생성 함수(경로, 권한)
  if(success == -1) //디렉토리 생성 실패 반환 -1
     printf("디렉토리 생성에 실패하였습니다..\n");
  else if(success == 0){ //디렉토리 생성 성공 반환 0
     printf("디렉토리 생성에 성공하였습니다\n");
     ls(); //생성 후 리스트 출력
  }
}





void remove_dir(char* argv[]){


	int success;


	success = rmdir(argv[1]);//삭제 진행argv[]


	if(success==0){ //삭제 성공 시


		printf("%s가 삭제되었습니다.\n",argv[1]);


	}else


		printf("error\n");


	


}





void cd (int argc, char* argv[]){


	int success;


	char buf[256]; //변경된 디렉토리 주소 저장 변수


	if(argc==1) { //변경할 주소가 없을 때

		success = chdir(getenv("HOME")); //HOME 환경변수에 저장된 주소로 이동

		if(success != 0) //디렉토리 변경 실패 시


			printf("change failed\n");


		else if( success == 0){ //디렉토리 변경 성공 시


			getcwd(buf,256);

			printf("변경된 디렉토리 : %s\n", buf);

		}

	}


	else if(argc==2){ // 변경할 디렉토리 주소를 입력했을 때


		success = chdir(argv[1]);


		if(success != 0)


			printf("change failed\n");


		else if(success == 0){


			getcwd(buf,256);


			printf("변경된 디렉토리 : %s\n", buf); 


		}


	}


	else //인자가 3개 이상일 때


		printf("cd : %s",argv[1]);


}


void ls(){ //현재 디렉토리 파일 출력


	DIR *pdir; 


	struct dirent *pde; 


	char buf[256];//파일 경로 출력


	int i = 0;


	if(getcwd(buf,256) == NULL){ //반환 실패시 NULL을 반환


		printf("현재 디렉토리 경로를 찾을 수 없습니다.");


		exit(1);


	}


	pdir = opendir(buf); //파일 경로의 디렉토리 열기


	while ((pde = readdir(pdir)) != NULL) { 


		if(strcmp(pde->d_name,".")==0|| strcmp(pde->d_name,"..")==0){


			continue; // 파일 또는 폴더 이름이 . or ..일 경우 출력 안함 


		}


		printf("%10s ", pde->d_name); // 파일 또는 폴더 이름 출력


		if (++i % 5 == 0) //출력된 수가 5개 이상이면 엔터


			printf("\n"); 


		


	}


	printf("\n"); 


	closedir(pdir); 	


}





void pwd(){


	char buf[256];


	if(getcwd(buf,256) == NULL){ //반환 실패시 NULL을 반환


		printf("현재 디렉토리 경로를 찾을 수 없습니다.");


		exit(1);


	}


	printf("현재 파일 위치 : %s\n", buf);


}


/*


void cd( int argc, char* argv[]){


	char old_buf[256];//현재 파일 경로를 저장


	char new_buf[256];//변경할 파일 경로를 저장


	if(getcwd(old_buf,256) == NULL){ //반환 실패시 NULL을 반환


		printf("현재 디렉토리 경로를 찾을 수 없습니다.");


		exit(1);


	}


	


	//sprintf(new_buf,"%s%s", old_buf,new_path);


	chdir(new_buf);


}*/

