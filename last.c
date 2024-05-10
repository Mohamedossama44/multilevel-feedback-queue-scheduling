#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int id;
    int burst_time;
    int remaining_burst_time;
    int arrival_time;
} Process;
typedef struct {
    Process *processes;
    int front, rear, size, max_size,number ;
} Queue;
void initializeQueue(Queue *queue, int max_size,int number) {
    queue->processes = (Process *)malloc(max_size * sizeof(Process));
    queue->front = queue->rear = -1;
    queue->size = 0;
    queue->max_size = max_size;
    queue->number = number;
}

int isQueueEmpty(Queue *queue) {
    return queue->size == 0;
}

int isQueueFull(Queue *queue) {
    return queue->size == queue->max_size;
}
void enqueue(Queue *wqueue, Process process,Queue *main_queue) {
    if (!isQueueFull(main_queue)) {
        printf("process %d in to main queue%d\n",process.id,main_queue->number);
        if (isQueueEmpty(main_queue)) {
            main_queue->front = 0;
        }
        main_queue->rear = (main_queue->rear + 1) % main_queue->max_size;
        main_queue->processes[main_queue->rear] = process;
        main_queue->size++;
    } else {
        //main q is full
        printf("process %d in to waiting queue %d \n",process.id,wqueue->number);
        if (isQueueEmpty(wqueue)) {
            wqueue->front = 0;
        }
        wqueue->rear = (wqueue->rear + 1) % wqueue->max_size;
        wqueue->processes[wqueue->rear] = process;
        wqueue->size++;
        //printf("Queue is full. Cannot enqueue.\n");
    }
}
Process dequeue(Queue *queue) {
    Process process;
    if (!isQueueEmpty(queue)) {
        process = queue->processes[queue->front];
        if (queue->front == queue->rear) {
            queue->front = queue->rear = -1;
        } else {
            queue->front = (queue->front + 1) % queue->max_size;
        }
        queue->size--;
    } else {
        process.id = -1; // Indicates an invalid process
    }
    return process;
}
void executeProcess(Process *process, int quantum, int queue_number) {
    int execution_time = (process->remaining_burst_time < quantum) ? process->remaining_burst_time : quantum;
    process->remaining_burst_time -= execution_time;
    printf("Executing process %d for %d units in Queue %d\n", process->id, execution_time, queue_number);
}

int main() {
    srand(time(NULL));

    Queue   queue1, queue2, queue3;
    initializeQueue(&queue1, 10,1);
    initializeQueue(&queue2, 20,2);
    initializeQueue(&queue3, 30,3);
    Queue wqueue1, wqueue2, wqueue3;
    initializeQueue(&wqueue1,  100,1);
    initializeQueue(&wqueue2,  100,2);
    initializeQueue(&wqueue3,  100,3);

    double cpu_share_1 = 0.5;
    double cpu_share_2 = 0.3;
    double cpu_share_3 = 0.2;

    int quantum_1 = 5;
    int quantum_2 = 10;

    for (int i = 1; i <= 100; i++) {
        Process process;
        process.id = i;
        process.burst_time = rand() % 100 + 1;
        process.remaining_burst_time = process.burst_time;
        process.arrival_time=i*5;
        printf("Process %d arrived at time %d with burst time %d\n", process.id, process.arrival_time, process.burst_time);

        enqueue(&wqueue1, process,&queue1);
    }
     int current_time = 0;

while (!isQueueEmpty(&queue1) || !isQueueEmpty(&queue2) || !isQueueEmpty(&queue3) ||
       !isQueueEmpty(&wqueue1) || !isQueueEmpty(&wqueue2) || !isQueueEmpty(&wqueue3)) {
        while (!isQueueEmpty(&queue1) && current_time % 2 == 0) {
            Process current_process = dequeue(&queue1);
            executeProcess(&current_process, quantum_1, 1);
            if (current_process.remaining_burst_time > 0) {
                // Only move to the next queue if remaining time is greater than zero
                printf("Moving process %d from Queue 1 to Queue 2\n", current_process.id);
                enqueue(&wqueue2, current_process,&queue2);

            } else {
                printf("Process %d finished in Queue 1! Total time: %d units\n",
                       current_process.id, current_process.burst_time);
            }
            if (!isQueueEmpty(&wqueue1) && !isQueueFull(&queue1)) {
                Process waiting_process = dequeue(&wqueue1);
                enqueue(&wqueue1, waiting_process, &queue1);
                printf("Moving process %d from waiting Queue 1 to Queue 1\n", waiting_process.id);
            }
        }
        while (!isQueueEmpty(&queue2) && isQueueEmpty(&queue1) && isQueueEmpty(&wqueue1) && current_time % 3 == 0) {

            Process current_process = dequeue(&queue2);
            executeProcess(&current_process, quantum_2, 2);
            if (current_process.remaining_burst_time > 0) {
                if ( rand() % 2 == 0) {
                    printf("Moving process %d from Queue 2 to  Queue 1\n", current_process.id);
                    enqueue(&wqueue1, current_process,&queue1);
                } else {
                    printf("Moving process %d from Queue 2 to Queue 3\n", current_process.id);
                    enqueue(&wqueue3, current_process,&queue3);
                }
            } else {
                printf("Process %d finished in Queue 2! Total time: %d units\n",
                       current_process.id, current_process.burst_time);
            }
            if (!isQueueEmpty(&wqueue2) && !isQueueFull(&queue2)) {
                Process waiting_process = dequeue(&wqueue2);
                enqueue(&wqueue2, waiting_process, &queue2);
                printf("Moving process %d from waiting Queue 2 to Queue 2\n", waiting_process.id);
            }
        }

        while (!isQueueEmpty(&queue3) && isQueueEmpty(&queue1) && isQueueEmpty(&queue2)) {
            Process current_process = dequeue(&queue3);
            executeProcess(&current_process, current_process.remaining_burst_time, 3);
            printf("Process %d finished in Queue 3! Total time: %d units\n",
                   current_process.id, current_process.burst_time - current_process.remaining_burst_time);
            if (!isQueueEmpty(&wqueue3) && !isQueueFull(&queue3)) {
                Process waiting_process = dequeue(&wqueue3);
                enqueue(&wqueue3, waiting_process, &queue3);
                printf("Moving process %d from waiting Queue 3 to Queue 3\n", waiting_process.id);
            }
        }

        current_time++;
    }

    free(queue1.processes);
    free(queue2.processes);
    free(queue3.processes);
    free(wqueue1.processes);
    free(wqueue2.processes);
    free(wqueue3.processes);
    return 0;
}
