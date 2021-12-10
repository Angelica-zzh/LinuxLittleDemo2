/*
 * @Author: 21181214207
 * @Date: 2021-12-08 03:44:26
 * @LastEditors: 21181214207
 * @LastEditTime: 2021-12-10 02:53:18
 * @FilePath: /demo2/myshell.c
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <pwd.h>

#define SIZE 128
#define TRUE 1
#define FALSE 0

char username[SIZE];
char hostname[SIZE];
char path[SIZE];
char commands[SIZE][SIZE];

//获取当前工作目录
void getCurWD(){
    char* result = getcwd(path,SIZE);
    if(!result){
        printf("获取当前工作目录失败");
        exit(0);
    }
}

//获取主机名
void getHostName(){
    gethostname(hostname,SIZE);
}
//h获取登录用户名
void getUserName(){
    struct passwd* pwd = getpwuid(getuid());
    strcpy(username,pwd->pw_name);
}
//分割命令
int splitCommands(char command[SIZE]){
    int num = 0;
    int i,j;
    int len =  strlen(command);
    //将分割出来的命令放入数组
    for(i=0,j=0;i<len;++i){
        if(command[i] != ' '){
			commands[num][j++] = command[i];   
        }else{
            if(j !=0 ){
                commands[num][j] = '\0';
                num++;
                j = 0;
            }
        }
    }
    if (j != 0)
    {
        commands[num][j] = '\0';
        //
        num++;
    }
    return num;
}
//发送退出命令
void callExit(){
    pid_t pid = getpid();
    if(kill(pid,SIGTERM) == -1){
        printf("\e[31mexit error\e[0m");
        exit(-1);
    }
}

//更新当前工作目录
void getWorkDir(){
    char* ret = getcwd(path,SIZE);
    if(ret == NULL){
        printf("\e[31merror: 系统更新当前目录失败\n\e[0m");
        exit(-1);
    }
}

//cd命令
void callCd(int commandNum){
    if(commandNum<2){
        printf("\e[31merror: 少参数\n\e[0m");
        
    }else if(commandNum > 2){
        printf("\e[31merror: 多参数\n\e[0m");
    }else{
        int ret = chdir(commands[1]);
        if(ret) printf("\e[31merror: 错误路径\n\e[0m");
        getWorkDir();
    }
}
//执行外部指令
void callCommand(int commandNum){
    //创建子进程
    pid_t pid = fork();
    if(pid == -1){
        printf("\e[31merror: fork失败\n\e[0m");
    }else if(pid == 0){
        char* comm[SIZE];
        for(int i = 0;i<commandNum;++i){
            comm[i] = commands[i];
        }
        comm[commandNum] = NULL;
        execvp(comm[0],comm);
        exit(0);
    }else{
        int status;
        waitpid(pid,&status,0);
        // 读取子进程的返回码
        int err = WEXITSTATUS(status); 
        // 返回码不为0，意味着子进程执行出错，用红色字体打印出错信息
		if (err) { 
			printf("\e[31;1mError: %s\n\e[0m", strerror(err));
		}
    }
}
int main(){
    getCurWD();
    getHostName();
    getUserName();

    //启动myshell
    //输入参数
    char argv[SIZE];
    //主体部分
    while(TRUE)
    {
        printf("\e[34m%s@%s:\e[0m%s\e[34m $ \e[0m",username,hostname,path);
        fgets(argv,SIZE,stdin);
        int len = strlen(argv);
        if(len != SIZE){
            argv[len-1] = '\0';
        }
        int commandNum = splitCommands(argv);
        
        if(commandNum != 0){
            //内部解释命令
            if(strcmp(commands[0],"exit") == 0){
                //exit
                callExit();
            }else if(strcmp(commands[0],"cd") == 0){
                //cd
                callCd(commandNum);

            }else{
                //执行外部命令
                callCommand(commandNum);
            }

        }
    }
    
    
}