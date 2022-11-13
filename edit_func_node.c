/*
 * @Author: LinJiasheng
 * @Date: 2022-11-13 00:53:17
 * @LastEditors: LinJiasheng
 * @LastEditTime: 2022-11-13 15:54:28
 * @Description: 
 * 
 * Copyright (c) 2022 by LinJiasheng, All Rights Reserved. 
 */
#include "edit.h"

void __fs_edit_init_rnode(rn_p *_rnp, rn_p prior, rn_p next, int row)
{
    *_rnp = (rn_p)malloc(sizeof(rnode));
    rn_p rnp = *_rnp;
    rnp->row = row;
    rnp->prior = prior;
    if (prior != NULL)
    {
        prior->next = rnp;
    }
    rnp->next = next;
    if (next != NULL)
    {
        next->prior = rnp;
    }
    rnp->ch = (char *)malloc(__EDIT_LINE_MAX * sizeof(char));
    memset(rnp->ch, '\0', __EDIT_LINE_MAX);
    rnp->current = -1;
    rnp->len = 0;
    return;
}

inline void __fs_edit_free_rnode(rn_p rp)
{
    free(rp->ch);
    free(rp);
    return;
}

void __fs_edit_row_walk(rn_p rp, int row)
{
    rn_p rp_next;
    do
    {
        rp_next = rp->next;
        if (rp->row == 0)
        {
            rp = rp_next;
            continue;
        }
        rp->row += row;
        rp = rp_next;
    } while (rp != NULL);
    return;
}

void __fs_edit_init_fh(fh_p *_fp, char name[], int fd)
{
    *_fp = (fh_p)malloc(sizeof(fh));
    fh_p fp = *_fp;
    fp->words = 0;
    fp->rows = 1;
    fp->name = name;
    fp->fd = fd;
    //初始化第一行的节点
    __fs_edit_init_rnode(&(fp->first_line), NULL, NULL, 0);
    fp->current_line = fp->first_line;
}

void __fs_edit_free_fh(fh_p fp)
{
    rn_p rp = fp->first_line, rp_next = rp->next;
    close(fp->fd);
    while (rp->next != NULL)
    {
        rp = rp_next->next;
        __fs_edit_free_rnode(rp_next);
        rp_next = NULL;
        rp_next = rp;
    }
    __fs_edit_free_rnode(fp->first_line);
    free(fp);
    return;
}