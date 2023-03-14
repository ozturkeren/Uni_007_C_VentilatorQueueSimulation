//Eren Öztürk METU NCC 213 A2
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "queue.h"
#include "queue.c"

struct structOfInput
{
    int numberOfPatients, numberOfVentilators, maxArrivalTime, maxServiceTime;
};

struct structOfPatients
{
    int patientType;
	int arrivalTime;
	int serviceTime;
	int serviceStartTime;
	int ventilatorID;
	char patientGender;
	char patientAgeGroup;
};

struct structOfVentilator
{
    int availability;
};

void parseInput(struct structOfInput*, char **);
Queue createPatientList(struct structOfInput*);
Queue initialiseSimulator(struct structOfVentilator*, struct structOfInput*);
void newPatient(Queue, Queue);
void servePatient(Queue, int*, int*, int*, int*, int*, int*, int*, int*);
void reportStatistics(struct structOfInput*, int*, int*, int*, int*, int*, int*, int*, int*, int*, int, int, int);

int main(int argc, char ** argv)
{
    if (argc == 1){
        printf("Please enter the numbers!\n");
        return 0;
    }
    srand(time(NULL));
    //Taking integers from the User and implementing them
    struct structOfInput *Inputs;
    Inputs = (struct structOfInput *) malloc(sizeof(struct structOfInput));

    parseInput(Inputs, argv);

    //Patient List Creating
    Queue arrivalTimeList;
    /*arrivalTimeList = CreateQueue();
    MakeEmptyQueue(arrivalTimeList);*/

    arrivalTimeList = createPatientList(Inputs);


    struct structOfVentilator *ventilator;
    ventilator = (struct structOfVentilator*)malloc(Inputs->numberOfVentilators * sizeof(struct structOfVentilator*));

    if(ventilator == (struct structOfVentilator*) NULL){
        printf("Allocation of list is failed!\n");
        exit(1);
    }

    Queue ventilatorQueue;

    ventilatorQueue = initialiseSimulator(ventilator, Inputs);

    int timeCounter=1, ventilatorCounter, maxWaitedTime=0, sumOfWaitedTime=0;

    int *numberOfSevere=0, *numberOfModerate=0, *numberOfMild=0, *numberOfPeopleUseVentilators, *numberOfMales=0, *numberOfFemales=0, *numberOfYoung=0, *numberOfAdult=0, *numberOfElder=0;

    numberOfPeopleUseVentilators = (int *) malloc (Inputs->numberOfVentilators * sizeof(int));
    int counter;
    for(counter=0; counter<Inputs->numberOfVentilators; counter++){
        numberOfPeopleUseVentilators[counter]=0;
    }
    //Start Of Simulation
    for(; !IsEmptyQueue(arrivalTimeList) || !IsEmptyQueue(ventilatorQueue); timeCounter++){
        for(ventilatorCounter=0; ventilatorCounter<Inputs->numberOfVentilators; ventilatorCounter++){
            if(ventilator[ventilatorCounter].availability == timeCounter){//Service time passed time for ventilator become available
                ventilator[ventilatorCounter].availability = 0;
            }
        }

        if(arrivalTimeList->front->next->arrivalTime <= timeCounter){
            newPatient(arrivalTimeList, ventilatorQueue);
        }

        for(ventilatorCounter=0; ventilatorCounter<Inputs->numberOfVentilators; ventilatorCounter++){
            if(ventilator[ventilatorCounter].availability == 0 && !IsEmptyQueue(ventilatorQueue)){//if ventilator is available and has someone in queue
                ventilatorQueue->front->next->serviceStartTime = timeCounter;
                ventilatorQueue->front->next->ventilatorID = ventilatorCounter;
                numberOfPeopleUseVentilators[ventilatorCounter]++;

                sumOfWaitedTime = sumOfWaitedTime + ventilatorQueue->front->next->serviceStartTime - ventilatorQueue->front->next->arrivalTime;//sum of waited time += waited time
                if((ventilatorQueue->front->next->serviceStartTime - ventilatorQueue->front->next->arrivalTime) >maxWaitedTime){
                    maxWaitedTime = ventilatorQueue->front->next->serviceStartTime - ventilatorQueue->front->next->arrivalTime;
                }
                ventilator[ventilatorCounter].availability = ventilatorQueue->front->next->serviceTime + timeCounter; //Will busy till service time passes
                servePatient(ventilatorQueue, numberOfSevere, numberOfModerate, numberOfMild, numberOfMales, numberOfFemales, numberOfYoung, numberOfAdult, numberOfElder);
            }
        }
    }


    //waited time = service start time - arrive time
    reportStatistics(Inputs, numberOfSevere, numberOfModerate, numberOfMild, numberOfMales, numberOfFemales, numberOfYoung, numberOfAdult, numberOfElder, numberOfPeopleUseVentilators, timeCounter, maxWaitedTime, sumOfWaitedTime);

    //Making list free
    MakeListFree(arrivalTimeList);
    if(IsEmptyQueue(arrivalTimeList)){
        free(arrivalTimeList);
    }
    MakeListFree(ventilatorQueue);
    if(IsEmptyQueue(ventilatorQueue)){
        free(ventilatorQueue);
    }

    return 0;
}

void parseInput(struct structOfInput *inputs, char **argv){

    inputs->numberOfPatients=(int)atof(argv[1]);
    inputs->numberOfVentilators=(int)atof(argv[2]);
    inputs->maxArrivalTime= (int)atof(argv[3]);
    inputs->maxServiceTime= (int)atof(argv[4]);

}

Queue createPatientList(struct structOfInput *Inputs){
    struct structOfPatients *unsortedPatientList;
    unsortedPatientList = (struct structOfPatients*)malloc(Inputs->numberOfPatients * sizeof(struct structOfPatients*));
    if(unsortedPatientList == (struct structOfPatients*) NULL){
        printf("Allocation of list is failed!\n");
        exit(1);
    }

    int counterOfPatients, ageGroup;
    for(counterOfPatients=0; counterOfPatients<Inputs->numberOfPatients; counterOfPatients++){
        unsortedPatientList[counterOfPatients].patientType = (rand()%3) +1;
        unsortedPatientList[counterOfPatients].arrivalTime = rand()%(Inputs->maxArrivalTime) +1;//Can't be zero and the max is the number which the user's gave
        unsortedPatientList[counterOfPatients].serviceTime = rand()%(Inputs->maxServiceTime) +1;//Can't be zero and the max is the number which the user's gave
        unsortedPatientList[counterOfPatients].serviceStartTime = 0;
        unsortedPatientList[counterOfPatients].ventilatorID = 0;
        if(rand()%2 == 0)
            unsortedPatientList[counterOfPatients].patientGender = 'M';//Male
        else
            unsortedPatientList[counterOfPatients].patientGender = 'F';//Female
        ageGroup = rand()%3;
        if(ageGroup == 0)
            unsortedPatientList[counterOfPatients].patientAgeGroup = 'Y';//Young
        else if(ageGroup == 1)
            unsortedPatientList[counterOfPatients].patientAgeGroup = 'A';//Adult
        else
            unsortedPatientList[counterOfPatients].patientAgeGroup = 'E';//Elderly
    }//List created unsorted


    int patientTypeBackup, arrivalTimeBackup, serviceTimeBackup, insertionSortCounter;
    char patientGenderBackup, patientAgeGroupBackup;
    for(counterOfPatients=0; counterOfPatients<(Inputs->numberOfPatients -1); counterOfPatients++){//Sorting based on arrival time
        insertionSortCounter = counterOfPatients+1;

        if(unsortedPatientList[insertionSortCounter-1].arrivalTime > unsortedPatientList[insertionSortCounter].arrivalTime){//Insertion sort
            do{
                patientTypeBackup = unsortedPatientList[insertionSortCounter-1].patientType;
                arrivalTimeBackup = unsortedPatientList[insertionSortCounter-1].arrivalTime;
                serviceTimeBackup = unsortedPatientList[insertionSortCounter-1].serviceTime;
                patientGenderBackup = unsortedPatientList[insertionSortCounter-1].patientGender;
                patientAgeGroupBackup = unsortedPatientList[insertionSortCounter-1].patientAgeGroup;

                unsortedPatientList[insertionSortCounter-1].patientType = unsortedPatientList[insertionSortCounter].patientType;
                unsortedPatientList[insertionSortCounter-1].arrivalTime = unsortedPatientList[insertionSortCounter].arrivalTime;
                unsortedPatientList[insertionSortCounter-1].serviceTime = unsortedPatientList[insertionSortCounter].serviceTime;
                unsortedPatientList[insertionSortCounter-1].patientGender = unsortedPatientList[insertionSortCounter].patientGender;
                unsortedPatientList[insertionSortCounter-1].patientAgeGroup = unsortedPatientList[insertionSortCounter].patientAgeGroup;

                unsortedPatientList[insertionSortCounter].patientType = patientTypeBackup;
                unsortedPatientList[insertionSortCounter].arrivalTime = arrivalTimeBackup;
                unsortedPatientList[insertionSortCounter].serviceTime = serviceTimeBackup;
                unsortedPatientList[insertionSortCounter].patientGender = patientGenderBackup;
                unsortedPatientList[insertionSortCounter].patientAgeGroup = patientAgeGroupBackup;

                insertionSortCounter--;
            }while(unsortedPatientList[insertionSortCounter-1].arrivalTime > unsortedPatientList[insertionSortCounter].arrivalTime && insertionSortCounter!=0);
        }
    }
    for(counterOfPatients=0; counterOfPatients<Inputs->numberOfPatients; counterOfPatients++){//Printing Patients as sorted
        printf("\nPatient %d: \t", counterOfPatients+1);
        printf("%d ", unsortedPatientList[counterOfPatients].patientType);
        printf("%d\t", unsortedPatientList[counterOfPatients].arrivalTime);
        printf("%d    ", unsortedPatientList[counterOfPatients].serviceTime);
        printf("%d  ", unsortedPatientList[counterOfPatients].serviceStartTime);
        printf("%d  ", unsortedPatientList[counterOfPatients].ventilatorID);
        printf("%c  ", unsortedPatientList[counterOfPatients].patientGender);
        printf("%c \n", unsortedPatientList[counterOfPatients].patientAgeGroup);
    }
    unsortedPatientList[1].patientType=2;

    /*Creating a linked list based on arrival time*/
    Queue arrivalTimeList;
    arrivalTimeList = CreateQueue();
    /*unsortedPatientList[1].patientType is changing, but I haven't understood why. Furthermore, after I change it here, it changes again.*/
    MakeEmptyQueue(arrivalTimeList);
    for(counterOfPatients=0; counterOfPatients<Inputs->numberOfPatients; counterOfPatients++){//Printing Patients as sorted
        printf("\nPatient %d: \t", counterOfPatients+1);
        printf("%d ", unsortedPatientList[counterOfPatients].patientType);
        printf("%d\t", unsortedPatientList[counterOfPatients].arrivalTime);
        printf("%d    ", unsortedPatientList[counterOfPatients].serviceTime);
        printf("%d  ", unsortedPatientList[counterOfPatients].serviceStartTime);
        printf("%d  ", unsortedPatientList[counterOfPatients].ventilatorID);
        printf("%c  ", unsortedPatientList[counterOfPatients].patientGender);
        printf("%c \n", unsortedPatientList[counterOfPatients].patientAgeGroup);
    }

    for(counterOfPatients=0; counterOfPatients<Inputs->numberOfPatients; counterOfPatients++){
        Enqueue(unsortedPatientList[counterOfPatients].patientType, unsortedPatientList[counterOfPatients].arrivalTime, unsortedPatientList[counterOfPatients].serviceTime, unsortedPatientList[counterOfPatients].serviceStartTime, unsortedPatientList[counterOfPatients].ventilatorID, unsortedPatientList[counterOfPatients].patientGender, unsortedPatientList[counterOfPatients].patientAgeGroup, arrivalTimeList);
    }
    DisplayQueue(arrivalTimeList);
    return arrivalTimeList;
}

Queue initialiseSimulator(struct structOfVentilator *ventilator, struct structOfInput *Inputs){

    int ventilatorCounter;//creating an integer array to keep the availability of the ventilators
    for(ventilatorCounter=0; ventilatorCounter < Inputs->numberOfVentilators; ventilatorCounter++){
        ventilator[ventilatorCounter].availability = 0;
    }//array created

    Queue ventilatorQueue;//creating an empty queue

	ventilatorQueue = CreateQueue();
    MakeEmptyQueue(ventilatorQueue);

    return ventilatorQueue;//queue created
}

void newPatient(Queue arrivalList, Queue ventilatorQueue){

    EnqueueFromQueue(arrivalList->front->next, ventilatorQueue);
    Dequeue(arrivalList);
}

void servePatient(Queue ventilatorQueue, int *numberOfSevere, int *numberOfModerate, int *numberOfMild, int *numberOfMales, int *numberOfFemales, int *numberOfYoung, int *numberOfAdult, int *numberOfElder){

    if(ventilatorQueue->front->next->patientType == 3){//Taking the number of severe people
        *numberOfSevere++;
    }
    else if(ventilatorQueue->front->next->patientType == 2){//Taking the number of moderate people
        *numberOfModerate++;
    }
    else {//Taking the number of mild people
        *numberOfMild++;
    }


    if(ventilatorQueue->front->next->patientGender == 'M'){//Taking the number of males
        *numberOfMales++;
    }
    else if(ventilatorQueue->front->next->patientGender == 'F'){//Taking the number of females
        *numberOfFemales++;
    }


    if(ventilatorQueue->front->next->patientAgeGroup == 'Y'){//Taking the number of young people
        *numberOfYoung++;
    }
    else if(ventilatorQueue->front->next->patientAgeGroup == 'A'){//Taking the number of moderate people
        *numberOfAdult++;
    }
    else if(ventilatorQueue->front->next->patientAgeGroup == 'E') {//Taking the number of mild people
        *numberOfElder++;
    }



    Dequeue(ventilatorQueue);
}


void reportStatistics(struct structOfInput *Inputs, int *numberOfSevere, int *numberOfModerate, int *numberOfMild, int *numberOfMales, int *numberOfFemales, int *numberOfYoung, int *numberOfAdult, int *numberOfElder, int *numberOfPeopleUseVentilators, int completionTime, int maxWaitedTime,int sumOfWaitedTime){
    printf("****************Report*****************\n");
    printf("The number of ventilators: %d\n", Inputs->numberOfVentilators);
    printf("The number of patients: %d\n", Inputs->numberOfPatients);
    printf("Number of patients for each patient type:\n");
    printf("\t Severe: %d \n\t Moderate: %d \n\t Mild: %d \n", *numberOfSevere, *numberOfModerate, *numberOfMild);
    printf("Number of patients for each ventilator:");
    int counter;
    for(counter=0;counter<Inputs->numberOfVentilators;counter++){
        printf("\tVentilator %d: %d\n", counter+1, numberOfPeopleUseVentilators[counter]);
    }
    printf("Completion Time: %d\n", completionTime);
    printf("Average time spent in the queue: %.2lf\n",(double)sumOfWaitedTime/(double)Inputs->numberOfPatients);
    printf("Maximum waiting time: %d\n", maxWaitedTime);
    if(*numberOfMales > *numberOfFemales){
        printf("Most gender usage: Male\n");
    }
    else if(*numberOfMales < *numberOfFemales){
        printf("Most gender usage: Female\n");
    }
    else
        printf("Most gender usage: Both Equal\n");

    if((*numberOfElder > *numberOfAdult)&&(*numberOfElder > *numberOfYoung))
        printf("Most age usage: Elder\n");
    else if((*numberOfAdult >= *numberOfYoung)&&(*numberOfAdult >= *numberOfElder))
        printf("Most age usage: Adult\n");
    else if((*numberOfYoung >= *numberOfAdult)&&(*numberOfYoung >= *numberOfElder))
        printf("Most age usage: Young\n");
}
