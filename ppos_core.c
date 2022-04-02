#include "ppos.h"
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

#define STACKSIZE 64*1024

task_t* mainTask; //tarefa main
task_t* currentTask;  //tarefa atual

int currentId = 0; //id atual

void ppos_init (){

    setvbuf (stdout, 0, _IONBF, 0);

    //ajustando variáveis para a tarefa main
    mainTask = (task_t*)malloc(sizeof(task_t));
    mainTask->id = currentId;
    mainTask->prev = NULL;
    mainTask->next = NULL;
    getcontext(&(mainTask->context));
    currentTask = mainTask; //define main como a tarefa atual
    currentId++;
    return;
}

int task_create (task_t *task, void (*start_func)(void *), void *arg){

    getcontext(&(task->context));

    //ajustes para o contexto
    char *stack;
    stack = malloc (STACKSIZE);

    if (stack)
    {
        task->context.uc_stack.ss_sp = stack ;
        task->context.uc_stack.ss_size = STACKSIZE ;
        task->context.uc_stack.ss_flags = 0 ;
        task->context.uc_link = 0 ;
    }
    else{
        return -1;
    }

    makecontext(&(task->context), (void*)(*start_func), 1, arg);

    task->id = currentId;
    currentId++;

    //se a tarefa main for a única tarefa existente até então, o apontamento é feito para main.
    if (mainTask->next == NULL){
        mainTask->next = task;
        task->prev = mainTask;
    }
    else { //caso contrário, o apontamento será para a tarefa atual
        task->prev = currentTask;
        currentTask->next = task;
    }
    return task->id;
}

//troca para o contexto de main
void task_exit (int exit_code){
    task_switch(mainTask);
}

//retorna o id da tarefa atual
int task_id () {
    return currentTask->id;
}

int task_switch (task_t *task){

    //antes de mudar o contexto, altera os ponteiros para definir que *task será a nova tarefa atual
    task->prev = currentTask;
    currentTask->next = task;
    currentTask = task;

    swapcontext(&(currentTask->prev->context), &(currentTask->context));
    return 0;
}