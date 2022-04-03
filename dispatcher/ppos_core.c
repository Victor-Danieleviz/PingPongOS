#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "ppos.h"

#define TERMINADA 1

#define STACKSIZE 64*1024

task_t* mainTask; //tarefa main
task_t* currentTask;  //tarefa atual
task_t dispatcherTask; //tarefa do dispatcher
task_t* readyTasks; //tarefas prontas que estão na fila

int currentId = 0; //id atual
int userTasks = 0; //quantidade de tarefas a serem executadas

//retorna a próxima tarefa que deverá ser executada, através da política FCFS 
task_t* scheduler(){
    readyTasks = readyTasks->next;
    return readyTasks->prev;
}

//resguarda que todas as tarefas prontas tenham sido executadas antes da finalização do programa.
void dispatcher(){

    task_t* nextTask;
    while(userTasks > 0)
    {
        nextTask = scheduler(); //define a próxima tarefa a ser executada
        if(nextTask == NULL) break;;
        task_switch(nextTask);
        if(nextTask->status == TERMINADA){
            queue_remove ((queue_t**)&readyTasks, (queue_t*) nextTask);
            userTasks--;
            free(nextTask->context.uc_stack.ss_sp);
            free(nextTask->context.uc_link);
        }
    }
    task_exit(0);
}

//inicialização do sistema
void ppos_init (){

    setvbuf (stdout, 0, _IONBF, 0);

    //ajustando variáveis para a tarefa main
    mainTask = (task_t*)malloc(sizeof(task_t));
    mainTask->id = currentId;
    currentId++;
    mainTask->prev = NULL;
    mainTask->next = NULL;
    getcontext(&(mainTask->context));
    currentTask = mainTask; //define main como a tarefa atual
    
    //cria o dispatcher
    task_create(&dispatcherTask, dispatcher, 0);
}

int task_create (task_t *task, void (*start_func)(void *), void *arg){

    getcontext(&(task->context));

    //ajustes para o contexto
    char *stack;
    stack = malloc (STACKSIZE);

    if(stack)
    {
        task->context.uc_stack.ss_sp = stack ;
        task->context.uc_stack.ss_size = STACKSIZE ;
        task->context.uc_stack.ss_flags = 0 ;
        task->context.uc_link = 0 ;
    }
    else{
        return -1;
    }

    makecontext(&(task->context), (void*)start_func, 1, (void*)arg);
    task->next = NULL;
    task->prev = NULL;
    task->id = currentId;
    currentId++;

    //se a tarefa for dispatcher a função é retornada.
    if (task == &dispatcherTask) return task->id;

    //tarefa adicionada na fila com as outras tarefas prontas, e userTasks é incrementado.
    userTasks++;
    queue_append((queue_t**)&readyTasks, (queue_t*)task);
    return task->id;
}

//retorna o id da tarefa atual
int task_id () {
    return currentTask->id;
}

//troca a tarefa atual para a tarefa de task
int task_switch (task_t *task){
    if (task == NULL || currentTask == NULL) return -1;
    task_t* aux = currentTask;
    currentTask = task;
    swapcontext(&(aux->context), &(currentTask->context));
    return 0;
}

//Finaliza a tarefa atual:
//Se for dispatcher, volta para main
//Se não for, decrementa userTasks e volta para dispatcher.
void task_exit (int exit_code){
    if (currentTask == &dispatcherTask){
        task_switch(mainTask);
    }
    else{
        currentTask->status = TERMINADA;
        task_switch(&dispatcherTask);
    }
}

void task_yield(){
    task_switch(&dispatcherTask);
}
