/*
 * @Author: LinJiasheng
 * @Date: 2022-11-10 22:01:13
 * @LastEditors: LinJiasheng
 * @LastEditTime: 2022-11-13 15:54:09
 * @Description: 
 * 
 * Copyright (c) 2022 by LinJiasheng, All Rights Reserved. 
 */
#ifndef __FS_EDIT__
#define __FS_EDIT__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>

//将path和d_name拼接成路径 path/d_name的形式存放在next_path中
#define __fs_strcat_path(next_path,path,d_name) do{\
            strcpy(next_path,path); \
            if(d_name!=NULL){\
                strcat(next_path,"/"); \
                strcat(next_path,d_name);\
            }\
        }while(0);

#define __EDIT_READ_BUF_SIZE 256
#define __EDIT_LINE_MAX (256*3)

#define __UP 65
#define __DOWN 66
#define __RIGHT 67
#define __LEFT 68

#define __CUR_UP(x) printf("\033[%dA",(x));fflush(stdout);
#define __CUR_DOWN(x) printf("\033[%dB",(x));fflush(stdout);
#define __CUR_RIGHT(x) printf("\033[%dC",(x));fflush(stdout);
#define __CUR_LEFT(x) printf("\033[%dD",(x));fflush(stdout);
#define __CUR_SAVE printf("\033[s");fflush(stdout);
#define __CUR_RESTOR printf("\033[u");fflush(stdout);
#define __CUR_SET(x,y) printf("\033[%d;%dH",(x),(y));fflush(stdout);
#define __CUR_RESET printf("\033[H");
#define __CUR_CLEAR printf("\033[2J");

#define __CUR_INIT do{\
            __CUR_CLEAR;\
            fflush(stdout);\
            __CUR_RESET;\
            fflush(stdout);\
        }while(0);

#define __IS_ZHCN(c) ((c)<0||(c)>127)

struct _row_node{
    //上一行
    struct _row_node *prior;
    //下一行
    struct _row_node *next;
    //这一行的字符
    char* ch;
    //指针当前位置
    int current;
    //本行有几个字符
    int len;
    //第几行
    int row;
};

struct _file_header{
    //文件名
    char *name;
    //文件描述符
    int fd;
    //第一行
    struct _row_node *first_line;
    //当前行
    struct _row_node *current_line;
    //总字数
    int words;
    //总行数
    int rows;
};
//行节点
typedef struct _row_node rnode;
typedef rnode * rn_p;
//文件头
typedef struct _file_header fh;
typedef fh * fh_p;

static char tofind[]={'(','[','\"','{','\''};
static char toreturn[]={')',']','\"','}','\''};

void __fs_edit_init_rnode(rn_p *_rnp, rn_p prior, rn_p next, int row);
void __fs_edit_free_rnode(rn_p rp);
void __fs_edit_row_walk(rn_p rp, int row);
void __fs_edit_init_fh(fh_p *_fp, char name[], int fd);
void __fs_edit_free_fh(fh_p fp);
void __fs_edit_rnode_insert_ch(rn_p rp, char ch);
void __fs_edit_rnode_delete_ch(rn_p rp);
void __fs_edit_delete_row(fh_p fp, rn_p rp);
bool __fs_edit_built_fhp(fh_p *fp, int fd);
void errprint(char *s1, char *s2);
bool __fs_get_stat_info(char path[],char d_name[],struct stat *info);
char __fs_edit_in_table(char c, int size, char tofind[],char toreturn[]);
#endif