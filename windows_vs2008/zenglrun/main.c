#define ZL_EXP_OS_IN_WINDOWS //���ڸ���zenglǶ��ʽ�ű����Ե�ǰ��ִ�л�����windowsϵͳ����ֹ���ֱ������linuxϵͳ���Բ�����ZL_EXP_OS_IN_LINUX����ΪĬ�Ͼ���linuxģʽ
#include "zengl_exportfuns.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>

#define STRNULL '\0'

typedef struct{
	char str[50];
	int ptrIndex;	//str���ڴ���з����ָ�������
	int size;
	int count;
	int cur;
}ReadStr_Type; //�ַ�����̬���飬���ڴ���û����ն��������Ϣ

FILE * debuglog;
ReadStr_Type ReadStr;
int random_seed=0;

/*
	���û����������д�뵽ReadStrȫ���ַ�����̬�����С�
*/
void makeReadStr(char ch)
{
	if(ReadStr.cur < 45)
	{
		ReadStr.str[ReadStr.cur++] = ch;
		ReadStr.str[ReadStr.cur] = STRNULL;
		ReadStr.count++;
	}
}

/*
	��ȡ��һ���û����������ݺ���Ҫ��ReadStr�ַ�����̬������α�ͼ���������Ϊ0�������´λ�ȡ��������ʱ���ܵõ���ȷ�����ݣ��������ܵ���һ���������ݵ�Ӱ�졣 
*/
void freeReadStr()
{
	ReadStr.cur = 0;
	ReadStr.count = 0;
}

ZL_EXP_INT main_userdef_info(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount)
{
	fprintf(debuglog,"%s",infoStrPtr);
	return 0;
}

ZL_EXP_INT main_userdef_compile_error(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount)
{
	printf("%s",infoStrPtr);
	return 0;
}

ZL_EXP_INT main_userdef_run_info(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount)
{
	fprintf(debuglog,"%s",infoStrPtr);
	return 0;
}

ZL_EXP_INT main_userdef_run_print(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount)
{
	printf("%s\n",infoStrPtr);
	return 0;
}

ZL_EXP_INT main_userdef_run_error(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount)
{
	printf("%s",infoStrPtr);
	return 0;
}

/*builtinģ�麯��*/
ZL_EXP_VOID main_builtin_printf(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
{
	ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
	ZL_EXP_INT i;
	if(argcount < 1)
		zenglApi_Exit(VM_ARG,"printf�����Ĳ���������Ϊ��");
	for(i=1;i<=argcount;i++)
	{
		zenglApi_GetFunArg(VM_ARG,i,&arg);
		switch(arg.type)
		{
		case ZL_EXP_FAT_INT:
			printf("%d",arg.val.integer);
			break;
		case ZL_EXP_FAT_FLOAT:
			printf("%.16g",arg.val.floatnum);
			break;
		case ZL_EXP_FAT_STR:
			printf("%s",arg.val.str);
			break;
		default:
			zenglApi_Exit(VM_ARG,"printf������%d������������Ч��Ŀǰֻ֧���ַ��������������������͵Ĳ���",i);
			break;
		}
	}
	return;
}

/*readģ�麯��*/
ZL_EXP_VOID main_builtin_read(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
{
	char ch;
	ch = getchar();
	while(ch!='\n')
	{
		makeReadStr(ch);
		ch = getchar();
	}
	if(ReadStr.count == 0)
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,"",0,0);
	else
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,ReadStr.str,0,0);
	freeReadStr();
}

/*bltRandomģ�麯��*/
ZL_EXP_VOID main_builtin_random(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
{
	time_t t;
	if(random_seed == 0) //��һ��ʹ��ʱ����Ϊ������ӡ�
	{
		srand((unsigned) time(&t));
		random_seed = rand();
	}
	else //����ʱ��ʹ����һ�����ɵ��������Ϊ�������
	{
		srand(random_seed);
		random_seed = rand();
	}
	zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,random_seed,0);
}

/*arrayģ�麯�������ڴ���zengl�ű��Ķ�̬����*/
ZL_EXP_VOID main_builtin_array(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
{
	ZENGL_EXPORT_MEMBLOCK memblock = {0};
	ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
	ZL_EXP_INT i;
	if(argcount == 0) //���array����û���������򴴽�һ��Ĭ�ϴ�С��δ��ʼ��������
	{
		zenglApi_CreateMemBlock(VM_ARG,&memblock,0);
		zenglApi_SetRetValAsMemBlock(VM_ARG,&memblock);
	}
	else if(argcount >= 1) //������˲�����ʹ�ò�����Ϊ�����ĳ�ʼֵ
	{
		zenglApi_CreateMemBlock(VM_ARG,&memblock,0);
		for(i=1;i<=argcount;i++)
		{
			zenglApi_GetFunArg(VM_ARG,i,&arg);
			switch(arg.type)
			{
			case ZL_EXP_FAT_INT:
			case ZL_EXP_FAT_FLOAT:
			case ZL_EXP_FAT_STR:
			case ZL_EXP_FAT_MEMBLOCK:
			case ZL_EXP_FAT_ADDR:
			case ZL_EXP_FAT_ADDR_LOC:
			case ZL_EXP_FAT_ADDR_MEMBLK:
				zenglApi_SetMemBlock(VM_ARG,&memblock,i,&arg);
				break;
			default:
				zenglApi_Exit(VM_ARG,"array������%d������������Ч",i);
				break;
			}
		}
		zenglApi_SetRetValAsMemBlock(VM_ARG,&memblock);
	}
	else
		zenglApi_Exit(VM_ARG,"array�����쳣����������С��0");
}

/*�ݹ��ӡ��������Ϣ*/
ZL_EXP_VOID main_print_array(ZL_EXP_VOID * VM_ARG,ZENGL_EXPORT_MEMBLOCK memblock,ZL_EXP_INT recur_count)
{
	ZL_EXP_INT size,i,j;
	ZENGL_EXPORT_MOD_FUN_ARG mblk_val = {ZL_EXP_FAT_NONE,{0}};
	zenglApi_GetMemBlockInfo(VM_ARG,&memblock,&size,ZL_EXP_NULL);
	for(i=1;i<=size;i++)
	{
		mblk_val = zenglApi_GetMemBlock(VM_ARG,&memblock,i);
		switch(mblk_val.type)
		{
		case ZL_EXP_FAT_INT:
		case ZL_EXP_FAT_FLOAT:
		case ZL_EXP_FAT_STR:
		case ZL_EXP_FAT_MEMBLOCK:
			for(j=0;j<recur_count;j++)
				printf("  ");
			break;
		}
		switch(mblk_val.type)
		{
		case ZL_EXP_FAT_INT:
			printf("[%d] %d\n",i-1,mblk_val.val.integer);
			break;
		case ZL_EXP_FAT_FLOAT:
			printf("[%d] %.16g\n",i-1,mblk_val.val.floatnum);
			break;
		case ZL_EXP_FAT_STR:
			printf("[%d] %s\n",i-1,mblk_val.val.str);
			break;
		case ZL_EXP_FAT_MEMBLOCK:
			printf("[%d] <array or class obj type> begin:\n",i-1);
			main_print_array(VM_ARG,mblk_val.val.memblock,recur_count+1);
			printf("[%d] <array or class obj type> end\n",i-1);
			break;
		}
	}
}

/*bltPrintArrayģ�麯������ӡ�����е�Ԫ��*/
ZL_EXP_VOID main_builtin_print_array(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
{
	ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
	if(argcount < 1)
		zenglApi_Exit(VM_ARG,"bltPrintArray������������Ϊ�գ�����ָ��һ������������Ϊ����");
	zenglApi_GetFunArg(VM_ARG,1,&arg);
	if(arg.type != ZL_EXP_FAT_MEMBLOCK)
		zenglApi_Exit(VM_ARG,"bltPrintArray�����Ĳ�����������������");
	main_print_array(VM_ARG,arg.val.memblock,0);
}

/*bltTestAddrģ�麯��(��������)�����Բ�������*/
ZL_EXP_VOID main_builtin_test_addr(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
{
	ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
	if(argcount < 2)
		zenglApi_Exit(VM_ARG,"bltTestAddr��������������������");
	zenglApi_GetFunArgInfo(VM_ARG,1,&arg);
	if(arg.type != ZL_EXP_FAT_ADDR && 
		arg.type != ZL_EXP_FAT_ADDR_LOC &&
		arg.type != ZL_EXP_FAT_ADDR_MEMBLK
		)
		zenglApi_Exit(VM_ARG,"��һ�����������Ǳ��������ã��������Ա�����ã�����������Ա������");
	zenglApi_GetFunArg(VM_ARG,1,&arg); //��ȡ��һ��������ֵ��zenglApi_GetFunArg���Եݹ����ã��ҵ����õı�����ֵ
	if(arg.type != ZL_EXP_FAT_STR)
		zenglApi_Exit(VM_ARG,"bltTestAddr����Ŀǰֻ�ܽ����ַ�����Ϊ����");
	printf("the value of first arg is %s\n",arg.val.str);
	zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ������������øò������ַ���ֵ���õ�һ���������õı���
	if(arg.type != ZL_EXP_FAT_STR)
		zenglApi_Exit(VM_ARG,"bltTestAddr����Ŀǰֻ�ܽ����ַ�����Ϊ����");
	zenglApi_SetFunArg(VM_ARG,1,&arg);
}

/*bltSetArrayģ�麯����ʹ�õ�2������3���Ȳ��������õ�һ��������Ӧ�������е�Ԫ��*/
ZL_EXP_VOID main_builtin_set_array(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
{
	ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
	ZENGL_EXPORT_MEMBLOCK memblock = {0};
	ZL_EXP_INT i;
	if(argcount < 2)
		zenglApi_Exit(VM_ARG,"bltSetArray��������������������");
	zenglApi_GetFunArg(VM_ARG,1,&arg);
	if(arg.type != ZL_EXP_FAT_MEMBLOCK)
		zenglApi_Exit(VM_ARG,"bltSetArray������һ����������������������������ߵ���������");
	memblock = arg.val.memblock;
	for(i=2;i<=argcount;i++)
	{
		zenglApi_GetFunArg(VM_ARG,i,&arg);
		zenglApi_SetMemBlock(VM_ARG,&memblock,i-1,&arg);
	}
}

ZL_EXP_VOID main_userdef_module_init(ZL_EXP_VOID * VM_ARG);

/*bltLoadScriptģ�麯�����½�һ������������ز�ִ��ĳ���ű�*/
ZL_EXP_VOID main_builtin_load_script(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
{
	ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
	ZL_EXP_CHAR * scriptName = ZL_EXP_NULL;
	ZENGL_EXPORT_VM_MAIN_ARGS vm_main_args = {main_userdef_info , 
											  main_userdef_compile_error,
											  main_userdef_run_info,
											  main_userdef_run_print,
											  main_userdef_run_error,
											  main_userdef_module_init,
											  ZL_EXP_CP_AF_IN_DEBUG_MODE | 
											  ZL_EXP_CP_AF_OUTPUT_DEBUG_INFO};
	zenglApi_GetFunArg(VM_ARG,1,&arg); //��ȡ��һ������Ϊ�ű���
	if(arg.type != ZL_EXP_FAT_STR)
		zenglApi_Exit(VM_ARG,"bltLoadScript������һ�����������ַ���������Ҫ���صĽű��ļ���");
	scriptName = arg.val.str;
	zenglApi_Load(scriptName,&vm_main_args); 
}

/*bltGetZLVersionģ�麯������ȡ��ǰzengl�汾����Ϣ���ַ�����ʽ*/
ZL_EXP_VOID main_builtin_get_zl_version(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
{
	ZL_EXP_CHAR version[20] = {0};
	sprintf(version,"%d.%d.%d",ZL_EXP_MAJOR_VERSION,ZL_EXP_MINOR_VERSION,ZL_EXP_REVISION);
	zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,version,0,0);
}

/*bltGetExtraDataģ�麯������ȡ�û���������*/
ZL_EXP_VOID main_builtin_get_extraData(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
{
	ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
	ZL_EXP_CHAR * extraName;
	if(argcount != 1)
		zenglApi_Exit(VM_ARG,"bltGetExtraData����������һ������");
	zenglApi_GetFunArg(VM_ARG,1,&arg); //��ȡ��һ������Ϊ�ű���
	if(arg.type != ZL_EXP_FAT_STR)
		zenglApi_Exit(VM_ARG,"bltGetExtraData������һ�������������ַ�������ʾ������������");
	extraName = arg.val.str;
	zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(ZL_EXP_CHAR *)zenglApi_GetExtraData(VM_ARG,extraName),0,0);
}

/*sdlģ�麯��*/
ZL_EXP_VOID main_sdl_init(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
{
	printf("sdl init argcount is %d\n",argcount);
	return;
}

ZL_EXP_VOID main_builtin_module_init(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT moduleID)
{
	zenglApi_SetModFunHandle(VM_ARG,moduleID,"printf",main_builtin_printf);
	zenglApi_SetModFunHandle(VM_ARG,moduleID,"read",main_builtin_read);
	zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltRandom",main_builtin_random);
	zenglApi_SetModFunHandle(VM_ARG,moduleID,"array",main_builtin_array);
	zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltPrintArray",main_builtin_print_array);
	zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltTestAddr",main_builtin_test_addr);
	zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSetArray",main_builtin_set_array);
	zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltLoadScript",main_builtin_load_script);
	zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetZLVersion",main_builtin_get_zl_version);
	zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetExtraData",main_builtin_get_extraData);
}

ZL_EXP_VOID main_sdl_module_init(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT moduleID)
{
	zenglApi_SetModFunHandle(VM_ARG,moduleID,"sdlInit",main_sdl_init);
}

ZL_EXP_VOID main_userdef_module_init(ZL_EXP_VOID * VM_ARG)
{
	zenglApi_SetModInitHandle(VM_ARG,"builtin",main_builtin_module_init);
	zenglApi_SetModInitHandle(VM_ARG,"sdl",main_sdl_module_init);
}

void main_exit(void * VM,char * err_format,...)
{
	va_list arg;
	va_start(arg,err_format);
	vprintf(err_format,arg);
	va_end(arg);
	zenglApi_Close(VM);
	#ifdef ZL_EXP_OS_IN_WINDOWS
		system("pause");
	#endif
	exit(-1);
}

/**
	�û�����ִ����ڡ�
*/
/*void main(int argc,char * argv[])
{
	int len = 0;
	ZENGL_EXPORT_VM_MAIN_ARGS vm_main_args = {main_userdef_info , 
											  main_userdef_compile_error,
											  main_userdef_run_info,
											  main_userdef_run_print,
											  main_userdef_run_error,
											  main_userdef_module_init,
											  ZL_EXP_CP_AF_IN_DEBUG_MODE | 
											  ZL_EXP_CP_AF_OUTPUT_DEBUG_INFO};
	if(argc < 2)
	{
		printf("usage: %s <filename> ... (�÷�����Ӧ���ǳ��������ļ�����ѡ���������ʽ���ļ���ͨ������.zlc��β��Ҳ������.zl��β)\n",argv[0]);
		#ifdef ZL_EXP_OS_IN_WINDOWS
			system("pause");
		#endif
		exit(-1);
	}

	printf("compiling(������)...\n");
	debuglog = fopen("main_debuglogs.txt","w+");
	if(zenglApi_Load(argv[1],&vm_main_args) == -1) //����ִ��zengl�ű�
	{
		printf("���󣺱���<%s>ʧ�ܣ���鿴debuglogs.txt��������ԭ��\n",argv[1]);
		#ifdef ZL_EXP_OS_IN_WINDOWS
			system("pause");
		#endif
		exit(-1);
	}
	fclose(debuglog);
	printf("compile finished(�������)\n");

	#ifdef ZL_EXP_OS_IN_WINDOWS
		system("pause");
	#endif
}*/

/**
	�û�����ִ����ڡ�
*/
void main(int argc,char * argv[])
{
	int len = 0;
	int testint;
	double testdouble;
	char * teststr = 0;
	ZL_EXP_INT builtinID,sdlID;
	ZL_EXP_VOID * VM;

	if(argc < 2)
	{
		printf("usage: %s <filename> ... (�÷�����Ӧ���ǳ��������ļ�����ѡ���������ʽ���ļ���ͨ������.zlc��β��Ҳ������.zl��β)\n",argv[0]);
		#ifdef ZL_EXP_OS_IN_WINDOWS
			system("pause");
		#endif
		exit(-1);
	}

	printf("compiling(������)...\n");
	debuglog = fopen("main_debuglogs.txt","w+");
	VM = zenglApi_Open();
	zenglApi_SetFlags(VM,(ZENGL_EXPORT_VM_MAIN_ARG_FLAGS)(ZL_EXP_CP_AF_IN_DEBUG_MODE | ZL_EXP_CP_AF_OUTPUT_DEBUG_INFO));
	zenglApi_SetHandle(VM,ZL_EXP_VFLAG_HANDLE_COMPILE_INFO,main_userdef_info);
	zenglApi_SetHandle(VM,ZL_EXP_VFLAG_HANDLE_RUN_INFO,main_userdef_run_info);
	zenglApi_SetHandle(VM,ZL_EXP_VFLAG_HANDLE_RUN_PRINT,main_userdef_run_print);
	//zenglApi_SetHandle(VM,ZL_EXP_VFLAG_HANDLE_MODULE_INIT,main_userdef_module_init); //Ҳ�����ڴ˴�����ģ���ʼ�����
	if((builtinID = zenglApi_SetModInitHandle(VM,"builtin",main_builtin_module_init)) == -1)
		main_exit(VM,"����useģ����ʧ��:%s",zenglApi_GetErrorString(VM));

	if((sdlID = zenglApi_SetModInitHandle(VM,"sdl",main_sdl_module_init)) == -1)
		main_exit(VM,"����useģ����ʧ��:%s",zenglApi_GetErrorString(VM));

	if(zenglApi_SetModFunHandle(VM,builtinID,"bltTest",main_builtin_printf) == -1)
		main_exit(VM,"����ģ�麯��ʧ��:%s",zenglApi_GetErrorString(VM));

	if(zenglApi_SetExtraData(VM,"name","my name is zengl") == -1)
		main_exit(VM,"���ö�������ʧ��:%s",zenglApi_GetErrorString(VM));

	if(zenglApi_SetExtraData(VM,"val","my val is zengl too") == -1)
		main_exit(VM,"���ö�������ʧ��:%s",zenglApi_GetErrorString(VM));

	if(zenglApi_Run(VM,argv[1]) == -1) //����ִ��zengl�ű�
		main_exit(VM,"���󣺱���<%s>ʧ�ܣ�%s\n",argv[1],zenglApi_GetErrorString(VM));

	if((teststr = zenglApi_GetValueAsString(VM,"glmytest")) == ZL_EXP_NULL)
		main_exit(VM,"��ȡ����glmytestʧ�ܣ�%s\n",zenglApi_GetErrorString(VM));

	if(zenglApi_GetValueAsInt(VM,"i",&testint) == -1)
		main_exit(VM,"��ȡ����iʧ�ܣ�%s\n",zenglApi_GetErrorString(VM));

	if(zenglApi_GetValueAsDouble(VM,"floatnum",&testdouble) == -1)
		main_exit(VM,"��ȡ����floatnumʧ�ܣ�%s\n",zenglApi_GetErrorString(VM));

	printf("the value of glmytest in test.zl is %s , i is %d , floatnum is %.16g\n",teststr,testint,testdouble);

	zenglApi_Reset(VM);

	builtinID = zenglApi_SetModInitHandle(VM,"builtin",main_builtin_module_init);
	//zenglApi_SetModFunHandle(VM,0,"printf",main_builtin_printf);
	if(zenglApi_Run(VM,"test2.zl") == -1) //����ִ��zengl�ű�
		main_exit(VM,"���󣺱���<test2.zl>ʧ�ܣ�%s\n",zenglApi_GetErrorString(VM));

	zenglApi_Reset(VM);

	zenglApi_Push(VM,ZL_EXP_FAT_INT,0,1415,0);

	zenglApi_Push(VM,ZL_EXP_FAT_STR,"test second arg",0,0);

	if(zenglApi_Call(VM,argv[1],"init","clsTest") == -1) //����ִ��zengl�ű�����
		main_exit(VM,"���󣺱���<test fun call>ʧ�ܣ�%s\n",zenglApi_GetErrorString(VM));

	zenglApi_Close(VM);

	fclose(debuglog);
	printf("compile finished(�������)\n");

	#ifdef ZL_EXP_OS_IN_WINDOWS
		system("pause");
	#endif
}