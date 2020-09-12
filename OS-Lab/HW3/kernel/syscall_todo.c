#include <linux/syscall_todo.h>

//debug function//
void print_todo_node(struct list_head *list_node){
    todo_node *print_node = list_entry(list_node, todo_node, todo_list);
    printk("     description of size %d is: ", print_node->description_size);
    int i;
    for(i=0; i < print_node->description_size; i++)
        printk("%c", (print_node->description)[i]);
    printk(".\n");
    printk("     status: %d\n", print_node->status);
}

void print_todo_list(struct list_head *head_node){
    struct list_head *iter_todo_list;
    list_for_each(iter_todo_list, head_node) {
        print_todo_node(iter_todo_list);
        printk("\n\n");
    }
}
//debug function//


//check if legal pid - exists and its me or my descendants, handle ERSCH
task_t* helper_check_legal_pid(pid_t pid){
    task_t* dest_task = find_task_by_pid(pid); //returns task_struct by PID
    if(dest_task == NULL) return NULL;
    task_t *cur_check_task = dest_task;
    task_t *prev_check_task = NULL;
    while(cur_check_task != NULL && cur_check_task != prev_check_task && cur_check_task != current) {
        prev_check_task = cur_check_task;
        cur_check_task = cur_check_task->p_pptr; //walk up to parents from dest_task till i hit myself or hit null
    }
    if(cur_check_task == current) return dest_task;//has permissions
    else return NULL;
}

void add_in_place(todo_node* new_node, task_t *dest_task){
    list_t *iter_todo_list;
    todo_node* curr_node;
    int i = 0;
    list_for_each(iter_todo_list, &(dest_task->firstTodo)) {
        curr_node = list_entry(iter_todo_list, todo_node, todo_list);
        if(curr_node->deadline > new_node->deadline) {
            list_add_tail(&(new_node->todo_list), &(curr_node->todo_list));
            return;
        }
    }
    list_add_tail(&(new_node->todo_list), &(dest_task->firstTodo));
}

int sys_add_TODO(pid_t pid, const char *TODO_description, ssize_t description_size, time_t TODO_deadline) {
    if(TODO_description == NULL || description_size < 1 || TODO_deadline < CURRENT_TIME) return -EINVAL;
    task_t *dest_task = helper_check_legal_pid(pid); //returns task_struct by PID
    if(dest_task == NULL) return -ESRCH;
    //allocate memory for new node in queue and init it
    todo_node *new_node = (todo_node*)kmalloc(sizeof(todo_node), GFP_KERNEL);
    if(new_node == NULL) return -ENOMEM;
    new_node->description = (char*)kmalloc(sizeof(char)*description_size, GFP_KERNEL);
    if(new_node->description == NULL) {
        kfree(new_node);
        return -ENOMEM;
    }
    new_node->status = 0;
    new_node->description_size = description_size;
    new_node->deadline = TODO_deadline;
    if(copy_from_user(new_node->description, TODO_description, description_size)) return -EFAULT;
    add_in_place(new_node, dest_task);
    return 0;
}

ssize_t sys_read_TODO(pid_t pid, int TODO_index, char *TODO_description, time_t* TODO_deadline, int* status){
    task_t *dest_task = helper_check_legal_pid(pid); //returns task_struct by PID
    if(dest_task == NULL) return -ESRCH;
    struct list_head *iter_todo_list;
    int list_pos = 0;
    //find todo_node in index on queue
    list_for_each(iter_todo_list, &(dest_task->firstTodo)) {
        list_pos++;
        if(list_pos == TODO_index) break; //found index in queue
    }
    if(list_pos != TODO_index) return -EINVAL; //index not in range of queue
    todo_node *target_node = list_entry(iter_todo_list, todo_node, todo_list);
    if(target_node == NULL) return -EINVAL;//list entry failed
    //check target pid queue node validity
    if(target_node->description == NULL) return -EINVAL;
    if(copy_to_user(TODO_description, target_node->description, target_node->description_size*sizeof(char))) return -EFAULT;
    if(copy_to_user(status, &(target_node->status), sizeof(target_node->status))) return -EFAULT;
    if(copy_to_user(TODO_deadline, &(target_node->deadline), sizeof(target_node->deadline))) return -EFAULT;
    return target_node->description_size;
}

int sys_mark_TODO(pid_t pid, int TODO_index, int status){
    task_t *dest_task = helper_check_legal_pid(pid); //returns task_struct by PID
    if(dest_task == NULL) return -ESRCH;
    struct list_head *iter_todo_list;
    int list_pos = 0;
    list_for_each(iter_todo_list, &(dest_task->firstTodo)) {//find list_head place index on queue
        list_pos++;
        if(list_pos == TODO_index) break; //found index in queue
    }
    if(list_pos != TODO_index) return -EINVAL; //index not in range of queue
    todo_node *target_node = list_entry(iter_todo_list, todo_node, todo_list);
    target_node->status = status;
    return 0;
}

int sys_delete_TODO(pid_t pid, int TODO_index){
    task_t *dest_task = helper_check_legal_pid(pid);
    if(dest_task == NULL) return -ESRCH;
    struct list_head *iter_todo_list;
    int list_pos = 0;
    list_for_each(iter_todo_list, &(dest_task->firstTodo)) {//find list_head place index on queue
        list_pos++;
        if(list_pos == TODO_index) break; //found index in queue
    }
    if(list_pos != TODO_index) return -EINVAL; //index not in range of queue
    todo_node *target_node = list_entry(iter_todo_list, todo_node, todo_list);
    list_del(&(target_node->todo_list));
    //free allocated memory of node
    kfree(target_node->description);
    kfree(target_node);
    return 0;
}



