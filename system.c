/******************************************************************************
	Copyright (C), 2012-2014, technology. Co., Ltd.
	File name:		system.c
	Author:			jinchong
	Version:		1.0
	Date: 			2012/09/20
	Description:	实现字符串操作基本函数
	Others:			
 ******************************************************************************/
/*
* 修改记录
*
* $Log:$
*
*/

#include "base.h"

/******************************************************************************
	Function:		sys_system
	Description:	系统调用
	Input:
		cmd_string	- 待执行的命令
	Output:			无
	Return:
		RC_SUCCESS	- 处理成功
		RC_FAILURE	- 处理失败
	Others:         <其它说明>
 ******************************************************************************/
int sys_system(char *cmd_string)
{
	int		ret;
	void	(*wasCld)(), (*wasChld)();

	wasCld  = signal(SIGCLD, SIG_DFL);
	wasChld = signal(SIGCHLD, SIG_DFL);
	ret = system(cmd_string);
	signal(SIGCLD, wasCld);
	signal(SIGCHLD, wasChld);
	return ret;
}

/******************************************************************************
	Function:		sys_system
	Description:	检查是否存在程序名的进程
	Input:
		program_name	--待查程序名 
	Output:				--无
	Return:
		TRUE			--存在
		FALSE			--不存在
	Others:         <其它说明>
 ******************************************************************************/
int program_is_exist(char *program_name)
{
	int count = 0;
	char cmd[1024] = {0}, line[200];
	char pid[20], print[256] = {0};
	void  (*was_cld)(), (*was_chld)();
	FILE *fp;
	pid_t this_pid = getpid();


	if(strrchr(program_name, '/') == NULL)
		sprintf(cmd, "ps -a -o pid -o comm | grep ' %s' | grep -v ' ps'", program_name);
	else
		sprintf(cmd, "ps -a -o pid -o comm | grep ' %s' | grep -v ' ps'", strrchr(program_name, '/') + 1);

	was_cld  = signal(SIGCLD, SIG_DFL);
    was_chld = signal(SIGCHLD, SIG_DFL);
	if((fp = popen(cmd, "r")) != NULL)
    {
		while(fgets(line, 100, fp))
		{
    		if(strlen(line) < 3)
				continue;
			memset(pid, 0, sizeof(pid));
			sscanf(line, "%s", pid);
			if(this_pid == atoi(pid))
				continue;
			strcat(print, " ");
			strcat(print, pid);
			count++;
		}
		pclose(fp);

		if(count)
		{
			fprintf(stdout, "%s is running have %d pid %s\n",program_name, count, print);
			return TRUE;
		}
	}
	signal(SIGCLD, was_cld);
	signal(SIGCHLD, was_chld);
	return FALSE;
}
