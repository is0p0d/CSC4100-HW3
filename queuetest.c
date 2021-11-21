#include<stdio.h>

typedef unsigned int    uint32; //setting these up because it would be
typedef unsigned short  uint16; //confusing to see int short and char being used
typedef unsigned char   uint8;  //to set up an idt entry
//#define NULL ((void* )0) //null ptr
#define TRUE 1
#define FALSE 0

//PCB structs
//===========================================================================
struct ProcessControlBlock_struct
{
    int* ESP;
    int PID;
}__attribute__((packed));
typedef struct ProcessControlBlock_struct PCB;

//PCB function prototypes
//===========================================================================
PCB* pcb_alloc(int pcbIndex);
int create_process(uint32 processEntry);

//queue structs
//===========================================================================
struct pcb_qNode
{
    PCB* val;
    struct pcb_qNode* next;
}__attribute__((packed));
typedef struct pcb_qNode pcb_qNode;

struct pcb_queue_list
{
    int count;
    pcb_qNode* front;
    pcb_qNode* rear;
}__attribute__((packed));
typedef struct pcb_queue_list pcb_queue;

//queue function prototypes
//===========================================================================
void pcb_queue_init(pcb_queue* queuePtr);
void qNode_enQueue(pcb_qNode* nodePtr, pcb_queue* queuePtr);
void qNode_deQueue(pcb_qNode* nodePtr, pcb_queue* queuePtr);
void pcb_enQueue(PCB* pcbVal, pcb_qNode* nodePtr, pcb_queue* queuePtr);
PCB* pcb_deQueue(pcb_queue* queuePtr);
pcb_qNode* nodeAlloc(int nodeIndex);

int row = 0;
int col = 0;
int num_processes = 0;
int num_pid = 0;
int num_stack = 0;
PCB PCBpool[5];
pcb_qNode nodePool[5];

int main()
{
    printf("Initializing readyqueue\n");
    PCB* currentPCB;
    PCB* PCBptrs[5] = {NULL};
    pcb_queue readyQueue;
    pcb_qNode* nodePtrs[5]={NULL};
    pcb_queue_init(&readyQueue);
    printf("INIT DONE! \n");
    printf("====================================================\n");
    
    PCBptrs[num_processes] = pcb_alloc(num_processes);
    PCBptrs[num_processes]->PID=100;
    printf("PID %d Assigned\n", PCBptrs[num_processes]->PID);
    nodePtrs[num_processes] = nodeAlloc(num_processes);
    pcb_enQueue(PCBptrs[num_processes],nodePtrs[num_processes],&readyQueue);
    num_processes++;
    printf("====================================================\n");
    
    PCBptrs[num_processes] = pcb_alloc(num_processes);
    PCBptrs[num_processes]->PID=101;
    printf("PID %d Assigned\n", PCBptrs[num_processes]->PID);
    nodePtrs[num_processes] = nodeAlloc(num_processes);
    pcb_enQueue(PCBptrs[num_processes],nodePtrs[num_processes],&readyQueue);
    num_processes++;
    printf("====================================================\n");    
    
    PCBptrs[num_processes] = pcb_alloc(num_processes);
    PCBptrs[num_processes]->PID=102;
    printf("PID %d Assigned\n", PCBptrs[num_processes]->PID);
    nodePtrs[num_processes] = nodeAlloc(num_processes);
    pcb_enQueue(PCBptrs[num_processes],nodePtrs[num_processes],&readyQueue);
    num_processes++;    
    printf("====================================================\n");
    
    PCBptrs[num_processes] = pcb_alloc(num_processes);
    PCBptrs[num_processes]->PID=103;
    printf("PID %d Assigned\n", PCBptrs[num_processes]->PID);
    nodePtrs[num_processes] = nodeAlloc(num_processes);
    pcb_enQueue(PCBptrs[num_processes],nodePtrs[num_processes],&readyQueue);
    num_processes++;
    printf("====================================================\n");
    
    PCBptrs[num_processes] = pcb_alloc(num_processes);
    PCBptrs[num_processes]->PID=104;
    printf("PID %d Assigned\n", PCBptrs[num_processes]->PID);
    nodePtrs[num_processes] = nodeAlloc(num_processes);
    pcb_enQueue(PCBptrs[num_processes],nodePtrs[num_processes],&readyQueue);
    printf("====================================================\n");

    printf("Absolute State of readyQueue: Front: %p, Rear: %p\n",readyQueue.front, readyQueue.rear);
    printf("====================================================\n");

    for(int i = 0; i < num_processes; i++)
    {
        currentPCB = pcb_deQueue(&readyQueue);
        printf("Proccess #%d, with PID %d\n",i, currentPCB->PID);
        printf("====================================================\n");
    }




}

void pcb_queue_init(pcb_queue* queuePtr)
{
    queuePtr->count = 0;
    queuePtr->front = NULL;
    queuePtr->rear = NULL;
}
void qNode_enQueue(pcb_qNode* nodePtr, pcb_queue* queuePtr)
{
    printf("enQueuing %p\n", nodePtr);
    if(nodePtr == NULL)
    {
        //println("!!ERROR: Attempt to push a null node.");
        printf("!!ERROR: Attempt to push a null node.\n");
    }
    else if (queuePtr->front == NULL)
    {
        queuePtr->front = nodePtr;
        queuePtr->rear = nodePtr;
    }
    else
    {
        queuePtr->rear->next = nodePtr;
        queuePtr->rear = nodePtr;
        printf("else qNode_enQueue next val address: %p\n", queuePtr->rear->next);
    }
    printf("enQUEUE || Front: %p, Rear: %p\n",queuePtr->front, queuePtr->rear);
}
void qNode_deQueue(pcb_qNode* nodePtr, pcb_queue* queuePtr)
{
    printf("qNode_deQueue passed queue address: %p\n", queuePtr);
    if (queuePtr->front == NULL)
    {
        printf("!!ERROR: Attmept to remove node from an empty queue.\n");
    }
    else
    {
        nodePtr = queuePtr->front;
        queuePtr->front = nodePtr->next;
        if (queuePtr->front = NULL)
        {
            queuePtr->rear = NULL;
        }
    }
    printf("deQUEUE || Front: %p, Rear: %p\n",queuePtr->front, queuePtr->rear);
}
void pcb_enQueue(PCB* pcbVal, pcb_qNode* nodePtr, pcb_queue* queuePtr)
{
    
    nodePtr->val = pcbVal;
    nodePtr->next = NULL;
    qNode_enQueue(nodePtr, queuePtr);
}
PCB* pcb_deQueue(pcb_queue* queuePtr)
{
    printf("pcd_deQueue passed queue address: %p\n", queuePtr);
    pcb_qNode* tempNode;
    qNode_deQueue(tempNode, queuePtr);
    return tempNode->val;
}
pcb_qNode* nodeAlloc(int nodeIndex)
{
    printf("node location: %p\n", &nodePool[nodeIndex]);
    return &nodePool[nodeIndex];
}
//PCB function prototypes
//===========================================================================
// int create_process(uint32 processEntry)
// {

// }
PCB* pcb_alloc(int pcbIndex)
{
    printf("pcb location: %p\n", &PCBpool[pcbIndex]);
    return &PCBpool[pcbIndex];
}