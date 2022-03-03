/******************************************************************************
	Copyright (C), 2012-2014, technology. Co., Ltd.
	File name:		system.c
	Author:			jinchong
	Version:		1.0
	Date: 			2012/09/20
	Description:	ʵ���ַ���������������
	Others:			
 ******************************************************************************/
/*
* �޸ļ�¼
*
* $Log:$
*
*/

#include "base.h"

/******************************************************************************
	Function:		sys_system
	Description:	ϵͳ����
	Input:
		cmd_string	- ��ִ�е�����
	Output:			��
	Return:
		RC_SUCCESS	- ����ɹ�
		RC_FAILURE	- ����ʧ��
	Others:         <����˵��>
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
	Description:	����Ƿ���ڳ������Ľ���
	Input:
		program_name	--��������� 
	Output:				--��
	Return:
		TRUE			--����
		FALSE			--������
	Others:         <����˵��>
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
