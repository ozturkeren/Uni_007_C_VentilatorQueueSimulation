//Eren Öztürk METU NCC 213 A2
//queue.c

/*This function initializes the queue*/
struct QueueRecord *CreateQueue()
{
    printf("\nCan come \n");
	struct QueueRecord *thelist;

    thelist=(struct QueueRecord *)malloc(sizeof(struct QueueRecord));
    if (thelist == NULL){
        printf("Out of memory!\n");
        exit(1);
    }
    else
        MakeEmptyQueue(thelist);

    return thelist;
}

/*This function sets the queue size to 0, and creates a dummy element
and sets the front and rear point to this dummy element*/
void MakeEmptyQueue(Queue q)
{
	q->front= (struct Node*) malloc(sizeof(struct Node));//Didn't compile as q->front= (struct Node*) malloc(sizeof(struct Node));
    if(q->front == NULL){
        printf("front out of memory!\n");
        exit(1);
    }
    q->front->next = NULL;
    q->rear = q->front;
    q->size = 0;
}

/*Shows if the queue is empty*/
int IsEmptyQueue(Queue q)
{
	if(q->front->next == NULL)
        return 1;
    else
        return 0;
}

/*Returns the queue size*/
int QueueSize(Queue q)
{
	return q->size;
}

/*Enqueue - Adds a new element to the queue, simply creates a node and
adds it to the rear of the queue*/
void Enqueue(int patientType, int arrivalTime, int serviceTime, int serviceStartTime, int ventilatorID, char patientGender, char patientAgeGroup, Queue q)
{
    struct Node* temporary;
    temporary = (struct Node*) malloc(10*sizeof(struct Node));//Didn't compile as temporary = (struct Node*) malloc(sizeof(struct Node));
    temporary->patientType = patientType;
    temporary->arrivalTime = arrivalTime;
    temporary->serviceTime = serviceTime;
    temporary->serviceStartTime = serviceStartTime;
    temporary->ventilatorID = ventilatorID;
    temporary->patientGender = patientGender;
    temporary->patientAgeGroup = patientAgeGroup;
    temporary->next = NULL;

    if (q->front->next == NULL ){
        q->front->next = temporary;
        q->rear = temporary;
    }

    else{
        q->rear->next = temporary;
        q->rear = temporary;
    }
    q->size++;
}

void EnqueueFromQueue(struct Node *frontOfList, Queue q)
{
    struct Node* temporary;
    temporary = (struct Node*) malloc(10*sizeof(struct Node));//Didn't compile as temporary = (struct Node*) malloc(sizeof(struct Node));
    temporary = frontOfList;
    temporary->next = NULL;

    if (q->front->next == NULL ){
        q->front->next = temporary;
        q->rear = temporary;
    }


    else{
        struct Node *checker;

        checker = q->front;

        for (;checker != NULL; checker = checker->next)
        {
            if(temporary->patientType > checker->next->patientType){//(S)evere > Moderate(D) > (M)ild
                temporary->next = checker->next;
                checker->next = temporary;
            }
            else if(checker == q->rear){//end of queue
                q->rear->next = temporary;
                q->rear = temporary;
            }
        }
    }
    q->size++;
}

/*Dequeue - Removes a node from the queue, basically removes a node from
the front of the queue*/
void Dequeue(Queue q)
{
	struct Node* deletedNode;
    deletedNode = (struct Node*) malloc(sizeof(struct Node));

    deletedNode= q->front->next;
    q->front->next= q->front->next->next;
    free(deletedNode);
    q->size--;
}

/*Returns the value stored in the front of the queue*/
struct Node *FrontOfQueue(Queue q)
{
	if (!IsEmptyQueue(q))
		return q->front->next;
	else
	{
		printf("The queue is empty\n");
		exit(1);
	}
}

/*Returns the value stored in the rear of the queue*/
struct Node *RearOfQueue(Queue q)
{
	if (!IsEmptyQueue(q))
		return q->rear;
	else
	{
		printf("The queue is empty\n");
		exit(1);
	}
}

/*Displays the content of the queue*/
void DisplayQueue(Queue q)
{
	struct Node *print;

	print = q->front->next;

	for (;print != NULL; print = print->next)
	{
	    printf("\nPatient: \t");
        printf("%d ", print->patientType);
        printf("%d\t", print->arrivalTime);
        printf("%d    ", print->serviceTime);
        printf("%d  ", print->serviceStartTime);
        printf("%d  ", print->ventilatorID);
        printf("%c  ", print->patientGender);
        printf("%c \n", print->patientAgeGroup);
	}
}

void MakeListFree(Queue q){
    while(q->front->next!= NULL){
        struct Node* deletedNode;
        deletedNode = (struct Node*) malloc(sizeof(struct Node));

        deletedNode= q->front->next;
        q->front->next= q->front->next->next;
        free(deletedNode);
        q->size--;
    }
    free(q);
}
