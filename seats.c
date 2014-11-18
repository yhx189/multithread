#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "seats.h"
#include "semaphore.h"


seat_t* seat_header = NULL;

char seat_state_to_char(seat_state_t);
m_sem_t *s;
int standby_num = 0;
int *standby ;//= (int *)malloc(sizeof(int)*8);


void list_seats(char* buf, int bufsize)
{
    seat_t* curr = seat_header;
    int index = 0;
    while(curr != NULL && index < bufsize+ strlen("%d %c,"))
    {
        int length = snprintf(buf+index, bufsize-index, 
                "%d %c,", curr->id, seat_state_to_char(curr->state));
        if (length > 0)
            index = index + length;
        curr = curr->next;
    }
    if (index > 0)
        snprintf(buf+index-1, bufsize-index-1, "\n");
    else
        snprintf(buf, bufsize, "No seats not found\n\n");
}

void view_seat(char* buf, int bufsize,  int seat_id, int customer_id, int customer_priority)
{
    seat_t* curr = seat_header;
    /************* standby list *****************/
    seat_t * head = seat_header;
    int occ = 0;
    while(head != NULL){
    	if ( head->id == seat_id && head->state == PENDING)
		occ = 1;
        head = head ->next;
   }
    if (occ ){
//	sem_wait(s);
       standby[standby_num] = customer_id;
	standby_num++;
	printf("standby list size %d\n", standby_num);
	if (standby_num > 8)
		fprintf(stderr, "standby list oversize\n");
  //      sem_post(s);
    }
     /************* standby list *****************/
    while(curr != NULL)
    {
        if(curr->id == seat_id)
        {
            if(curr->state == AVAILABLE || (curr->state == PENDING && curr->customer_id == customer_id))
            {
                snprintf(buf, bufsize, "Confirm seat: %d %c ?\n\n",
                        curr->id, seat_state_to_char(curr->state));
                curr->state = PENDING;
                curr->customer_id = customer_id;
            }
            else
            {
                snprintf(buf, bufsize, "Seat unavailable\n\n");
            }
	//	printf("viewing seats\n");
            return;
        }
        curr = curr->next;
    }
    snprintf(buf, bufsize, "Requested seat not found\n\n");
    return;
}

void confirm_seat(char* buf, int bufsize, int seat_id, int customer_id, int customer_priority)
{
    seat_t* curr = seat_header;
    while(curr != NULL)
    {
        if(curr->id == seat_id)
        {
            if(curr->state == PENDING && curr->customer_id == customer_id )
            {
                snprintf(buf, bufsize, "Seat confirmed: %d %c\n\n",
                        curr->id, seat_state_to_char(curr->state));
                curr->state = OCCUPIED;
            }
            else if(curr->customer_id != customer_id )
            {
                snprintf(buf, bufsize, "Permission denied - seat held by another user\n\n");
            }
            else if(curr->state != PENDING)
            {
                snprintf(buf, bufsize, "No pending request\n\n");
            }

            return;
        }
        curr = curr->next;
    }
       snprintf(buf, bufsize, "Requested seat not found\n\n");
    
    return;
}

void cancel(char* buf, int bufsize, int seat_id, int customer_id, int customer_priority)
{
    printf("Cancelling seat %d for user %d\n", seat_id, customer_id);

    seat_t* curr = seat_header;
    while(curr != NULL)
    {
        if(curr->id == seat_id)
        {
            if(curr->state == PENDING && curr->customer_id == customer_id )
            {
                snprintf(buf, bufsize, "Seat request cancelled: %d %c\n\n",
                        curr->id, seat_state_to_char(curr->state));
                curr->state = AVAILABLE;
		
		
		/************* standby list *****************/
   		   if(standby_num > 0)
   		 {
    	//	sem_wait(s);
		printf("sssssssssssssssssss\n");
		//view_seat(buf, bufsize, seat_id, standby[0], customer_priority);
		standby_num--;
		int i;
		for(i=0; i < standby_num; i++)
				standby[i] = standby[i+1];
	//	sem_post(s);
    		}		

		 /************* standby list *****************/


            }
            else if(curr->customer_id != customer_id )
            {
                snprintf(buf, bufsize, "Permission denied - seat held by another user\n\n");
            }
            else if(curr->state != PENDING)
            {
                snprintf(buf, bufsize, "No pending request\n\n");
            }

            return;
        }
        curr = curr->next;
    }
    snprintf(buf, bufsize, "Seat not found\n\n");
       return;
}

void load_seats(int number_of_seats)
{
    seat_t* curr = NULL;
    int i;
    for(i = 0; i < number_of_seats; i++)
    {   
        seat_t* temp = (seat_t*) malloc(sizeof(seat_t));
        temp->id = i;
        temp->customer_id = -1;
        temp->state = AVAILABLE;
        temp->next = NULL;
        
        if (seat_header == NULL)
        {
            seat_header = temp;
        }
        else
        {
            curr-> next = temp;
        }
        curr = temp;
    }

    /**************stanby list  ************/
    s = (m_sem_t*)malloc(sizeof(m_sem_t));
    sem_init(s, 0);
    standby = (int *)malloc(sizeof(int)*8);
  /************* standby list *****************/
}

void unload_seats()
{
    seat_t* curr = seat_header;
    while(curr != NULL)
    {
        seat_t* temp = curr;
        curr = curr->next;
        free(temp);
    }
}

char seat_state_to_char(seat_state_t state)
{
    switch(state)
    {
        case AVAILABLE:
            return 'A';
        case PENDING:
            return 'P';
        case OCCUPIED:
            return 'O';
    }

    return '?';
}
