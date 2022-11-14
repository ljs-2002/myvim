#include "edit.h"

bool __fs_get_stat_info(char path[],char d_name[],struct stat *info){
    char completename[250];
    __fs_strcat_path(completename,path,d_name);
    if(lstat(completename,info)==-1){
        free(info);
        errprint("get file info fail!\n",completename);
        return true;
    }
    return false;
}

void errprint(char *s1, char *s2)
{
	fprintf(stderr, "Error: %s", s1);
	perror(s2);
}

void __fs_edit_rnode_insert_ch(rn_p rp, char ch)
{
    int cur = rp->current;
    if (cur != rp->len - 1)
    {
        int ind;
        for (ind = rp->len; ind > cur + 1; ind--)
        {
            rp->ch[ind] = rp->ch[ind - 1];
        }
    }
    rp->ch[++cur] = ch;
    rp->current++;
    rp->len++;
}

void __fs_edit_rnode_delete_ch(rn_p rp)
{
    int cur = rp->current;
    int ind;
    for (ind = cur + 1; ind < rp->len; ind++)
    {
        rp->ch[ind - 1] = rp->ch[ind];
    }
    rp->ch[rp->len - 1] = '\0';
    rp->current--;
    rp->len--;
}

void __fs_edit_delete_row(fh_p fp, rn_p rp)
{
    if (rp->prior != NULL)
    { //不是第一行
        rp->prior->next = rp->next;
        if (rp->next != NULL)
            rp->next->prior = rp->prior;

        fp->current_line = rp->prior;
    }
    else
    {
        if (rp->next == NULL)
        { //若前后都为空说明是第一行并且只有一行，则不删除当前行。
            return;
        }
        else
        { //否则说明是第一行并且后面还有，将后面的节点作为第一行
            fp->first_line = rp->next;
            fp->current_line = rp->next;
            rp->next->prior = NULL;
        }
    }
    fp->rows--;
    __fs_edit_free_rnode(rp);
    return;
}

bool __fs_edit_built_fhp(fh_p *fp, int fd)
{
    char buf[__EDIT_READ_BUF_SIZE];
    unsigned long read_size;
    unsigned long i;
    int words = 0;
    rn_p rp = (*fp)->first_line, rp_next;

    while ((read_size = read(fd, buf, __EDIT_READ_BUF_SIZE)) > 0)
    {
        for (i = 0ul; i < read_size; i++)
        {
            if (buf[i] == '\n' || buf[i]=='\r')
            {
                goto change;
            }
            __fs_edit_rnode_insert_ch(rp, buf[i]);
            if (__IS_ZHCN(buf[i]))
            {
                __fs_edit_rnode_insert_ch(rp, buf[++i]);
                __fs_edit_rnode_insert_ch(rp, buf[++i]);
            }
            words++;
        change:
            if (buf[i] == '\n'|| buf[i]=='\r')
            {
                __fs_edit_init_rnode(&rp_next, rp, NULL, rp->row + 1);
                rp->current = -1;
                rp = rp_next;
            }
        }
        if (read_size < __EDIT_READ_BUF_SIZE)
        {
            rp->current = -1;
            break;
        }
    }
    (*fp)->rows = rp->row + 1;
    (*fp)->words = words;
    if (read_size == -1)
    {
        errprint("read file fail!\n", (*fp)->name);
        return true;
    }
    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        errprint("lseek file fail!\n", (*fp)->name);
        return true;
    }
    return false;
}

char __fs_edit_in_table(char c, int size, char tofind[],char toreturn[]){
    int i;
    for(i=0;i<size;i++){
        if(c==tofind[i]){
            return toreturn[i];
        }
    }
    return 0x0;
}