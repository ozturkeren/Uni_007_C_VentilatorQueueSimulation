//Eren Öztürk METU NCC 213 A2
//queue.h

/*A Node declaration to store a value and a link*/
struct Node
{
    int patientType;
	int arrivalTime;
	int serviceTime;
	int serviceStartTime;
	int ventilatorID;
	char patientGender;
	char patientAgeGroup;

	struct Node *next;
};

/*Queue Record that will store the following:
size: total number of elements stored in the list
front: it shows the front node of the queue (front of the queue)
rear: it shows the rare node of the queue (rear of the queue)*/
struct QueueRecord
{
	struct Node *front;   /* pointer to front of queue */
	struct Node *rear;    /* pointer to rear of queue */
	int size;             /* number of items in queue */
};
typedef struct QueueRecord *Queue;

struct QueueRecord *CreateQueue();
void MakeEmptyQueue(struct QueueRecord *);
int QueueSize(Queue);
void Enqueue(int, int, int, int, int, char, char, Queue);
void EnqueueFromQueue(struct Node*, Queue);
void Dequeue(Queue);
struct Node *FrontOfQueue(Queue);
struct Node *RearOfQueue(Queue);
int IsEmptyQueue(Queue);
void DisplayQueue(Queue);
void MakeListFree(Queue);
