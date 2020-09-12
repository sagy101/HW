#ifndef SYSCALL_TODO_H
#define SYSCALL_TODO_H

#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm-i386/uaccess.h>
#include <linux/list.h>

typedef struct _todo_node{
    struct list_head todo_list;
    char *description;
    int description_size;
    int status;
} todo_node;

int sys_add_TODO(pid_t pid, const char *TODO_description, ssize_t description_size);
ssize_t  sys_read_TODO(pid_t pid, int TODO_index, char *TODO_description, ssize_t description_size, int* status);
int sys_mark_TODO(pid_t pid, int TODO_index, int status);
int sys_delete_TODO(pid_t pid, int TODO_index);


#endif //SYSCALL_TODO_H
