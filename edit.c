/*
 * @Author: LinJiasheng
 * @Date: 2022-11-09 22:48:28
 * @LastEditors: LinJiasheng
 * @LastEditTime: 2022-11-13 20:05:49
 * @Description:
 *
 * Copyright (c) 2022 by LinJiasheng, All Rights Reserved.
 */

#include "edit.h"

void __fs_edit_print_txt(fh_p fp)
{
    __CUR_SAVE;
    __CUR_INIT;
    rn_p row = fp->first_line, row_next = row->next;
    int i;
    while (row != NULL)
    {
        if (row->len == 0)
        {
            printf("%c", 0x0A);
        }
        for (i = 0; i < row->len; i++)
        {
            printf("%c", row->ch[i]);
        }
        fflush(stdout);
        row = row->next;
        if (row != NULL&&row->prior->len!=0)
        {
            printf("\n\r");
        }
    }
    __CUR_RESTOR;
    return;
}

bool __fs_edit_read_text(char filename[], fh_p *fp)
{
    bool exist = false;
    int fd;
    if (access(filename, F_OK) != -1)
    {
        exist = true;
        //文件类型和权限检查
        struct stat info;
        if (__fs_get_stat_info(filename, NULL, &info))
        {
            fprintf(stderr, "Error: get file info fail!\n");
            return true;
        }
        if (!S_ISREG(info.st_mode))
        {
            fprintf(stderr, "Error:%s isn't a file!\n", filename);
            return true;
        }
        if (access(filename, R_OK) == -1 || access(filename, W_OK) == -1)
        {
            fprintf(stderr, "Error:you don't have read or write access to %s\n", filename);
            return true;
        }
    }
    fd = open(filename, O_RDWR | O_CREAT);
    //将文件的内容全部读出来
    __fs_edit_init_fh(fp, filename, fd);
    if (exist)
    {
        if (__fs_edit_built_fhp(fp, fd))
        {
            fprintf(stderr, "Error:build file header fail!\n");
            return true;
        }
    }
    __CUR_INIT;
    return false;
}
void __fs_edit_save_txt(fh_p fp)
{
    char name[] = "temp_XXXXXX";
    close(fp->fd);
    int fd = mkstemp(name);
    if (fd == -1)
    {
        perror(name);
        return;
    }
    rn_p row = fp->first_line, row_next = row->next;
    int i;
    char buf[] = {'\n'};
    while (row != NULL)
    {
        write(fd, row->ch, row->len);
        sync();
        row = row->next;
        if (row != NULL)
        {
            write(fd, buf, 1);
        }
    }
    sync();
    unlink(fp->name);
    rename(name, fp->name);
    fp->fd = fd;
    return;
}

void __fs_edit_move_cur(fh_p fp, char c)
{
    rn_p cur_row = fp->current_line;
    int cur_col = cur_row->current;
    int next_len;
    switch (c)
    {
    case __UP:
    {
        if (cur_row->row != 0)
        {
            fp->current_line = cur_row->prior;
            cur_row->current = -1;
            __CUR_UP(1);
            next_len = fp->current_line->len;
            if (cur_col < next_len)
            {
                fp->current_line->current = cur_col;
            }
            else
            {
                fp->current_line->current = next_len - 1;
                __CUR_LEFT(cur_col - next_len + 1);
            }
        }
        else
        {
            fprintf(stdout, "\a");
            fflush(stdout);
        }
        break;
    }
    case __DOWN:
    {
        if (cur_row->row < fp->rows - 1)
        {
            fp->current_line = cur_row->next;
            cur_row->current = -1;
            __CUR_DOWN(1);
            next_len = fp->current_line->len;
            if (cur_col < next_len)
            {
                fp->current_line->current = cur_col;
            }
            else
            {
                fp->current_line->current = next_len - 1;
                __CUR_LEFT(cur_col - next_len + 1);
            }
        }
        else
        {
            fprintf(stdout, "\a");
            fflush(stdout);
        }
        break;
    }
    case __RIGHT:
    {
        if (cur_col < cur_row->len - 1)
        {
            if (__IS_ZHCN(cur_row->ch[cur_col]))
            {
                cur_row->current += 2;
                __CUR_RIGHT(1);
            }
            cur_row->current++;
            __CUR_RIGHT(1);
        }
        else
        {
            fprintf(stdout, "\a");
            fflush(stdout);
        }
        break;
    }
    case __LEFT:
    {
        if (cur_col > -1)
        {
            if (__IS_ZHCN(cur_row->ch[cur_col]))
            {
                cur_row->current -= 2;
                __CUR_LEFT(1);
            }
            cur_row->current--;
            __CUR_LEFT(1);
        }
        else
        {
            fprintf(stdout, "\a");
            fflush(stdout);
        }
        break;
    }
    default:
        break;
    }
}

void __fs_edit_delete(fh_p fp)
{
    rn_p rp = fp->current_line;
    int cur = rp->current;
    /*是否为行首
     *若是行首则将本行内容合并至上一行末尾
     *若不是行首，则删去当前字符
     */
    if (cur == -1)
    {
        if (rp->prior == NULL)
        {
            fprintf(stdout, "\a");
            fflush(stdout);
            return;
        }
        int prio_len = rp->prior->len;
        strcat(rp->prior->ch, rp->ch); //把本行的内容拼接到上一行末尾
        rp->prior->current = rp->prior->len - 1;
        rp->prior->len += rp->len;
        __fs_edit_delete_row(fp, rp);
        if (fp->current_line->row < fp->rows - 1)
            __fs_edit_row_walk(fp->current_line->next, -1);
        __fs_edit_print_txt(fp);
        __CUR_UP(1);
        if(prio_len!=0)
            __CUR_RIGHT(prio_len);
        return;
    }
    else
    {
        //中文和英文字符处理
        if (__IS_ZHCN(rp->ch[cur]))
        {
            __fs_edit_rnode_delete_ch(rp);
            __fs_edit_rnode_delete_ch(rp);
            __fs_edit_rnode_delete_ch(rp);
        }
        else
        {
            __fs_edit_rnode_delete_ch(rp);
        }
        fp->words--;
    }
    //处理显示
    __fs_edit_print_txt(fp);
    __CUR_LEFT(1);
    return;
}

void __fs_edit_insert(fh_p fp, char c)
{
    //先在fp中保存当前的字符
    if(c==9){//Tab替换为4个空格
        char _c=' ';
        int i;
        for(i=0;i<4;i++){
            __fs_edit_rnode_insert_ch(fp->current_line, _c);
        }
    }else{
        __fs_edit_rnode_insert_ch(fp->current_line, c);
    }
    //处理显示
    __fs_edit_print_txt(fp);
    if(c==9){
        __CUR_RIGHT(3);
    }
    __CUR_RIGHT(1);
}

void __fs_edit_newline(fh_p fp)
{
    rn_p row = fp->current_line;
    rn_p newline;
    __fs_edit_init_rnode(&newline, row, row->next, row->row + 1);
    int index = row->current,i;
    strcpy(newline->ch,&row->ch[index+1]);
    for(i=row->len-1;i>index;i--){
        row->ch[i]='\0';
    }
    newline->len=strlen(newline->ch);
    row->len=index+1;
    
    if(newline->next!=NULL){
        __fs_edit_row_walk(newline->next,1);
    }
    fp->current_line=newline;
    fp->rows++;
    __fs_edit_print_txt(fp);
    __CUR_SET(newline->row+1,0);
    return;
}

void __fs_edit(char filename[])
{
    fh_p fp;
    __fs_edit_read_text(filename, &fp);
    __fs_edit_print_txt(fp);
    //system(STTY_US TTY_PATH);
    system("stty raw");
    system("stty -echo");
    char c;
    while ((c = getchar()) != 3)
    {
        if (c == 27)
        { //方向键 or esc，目前先实现方向键
            if ((c = getchar()) != 91)
            {
                ungetc(c, stdin);
            }
            else
            {
                c = getchar();
                __fs_edit_move_cur(fp, c);
            }
            continue;
        }
        else if (c == 127)
        { //退格
            __fs_edit_delete(fp);
            continue;
        }
        else if (c == 19)
        {
            __fs_edit_save_txt(fp);
        }
        else if (c == 13)
        { //换行
            __fs_edit_newline(fp);
        }
        else if (0 < c < 127)
        {
            __fs_edit_insert(fp, c);
        }
    }
    //system(STTY_DEF TTY_PATH);
    system("stty echo");
    system("stty -raw");
    __fs_edit_free_fh(fp);
    __CUR_CLEAR;
}

int main(int argc,char*argv[])
{
    if(argc<2){
        fprintf(stderr,"too few arguments! useage: %s <filename>\n",argv[0]);
        return 0;
    }
    __fs_edit(argv[1]);
    exit(0);
}
