 //   Its a producer-consumer problem

#include<iostream>
#include<queue>
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>
#include<stdio.h>

using namespace std;

//sem_t mutex1;
int min_customer_to_be_serviced = 20;
pthread_mutex_t mutex1;
bool counter_state;//1 implies counter is open and 0 implies counter is closed
queue<int> wait_q;
int sp, q_max, count_to_customer_id = 0;

void *service_personel_runner(void* args)
    {
    	int pid = *((int *)args);
        while(1)
        { 
        	if(counter_state == true)
        	{	
	            //Critical section for consumer
		        //sem_wait(&mutex1); //down on semaphore (entry into critical section)
		        pthread_mutex_lock(&mutex1);  
	            if(!wait_q.empty())
	            { 	
		            int served_customer_id = wait_q.front();
		            wait_q.pop();
		            cout<<"Customer with customer id "<<served_customer_id<<" got severd"<<" by service personel "<<pid<<endl;
	            } 
	            else
	            {
	            	cout<<"\x1B[31m \nWaiting queue is empty!!! \x1B[0m"<<endl;
	            	cout<<"Service personel "<<pid<<" goes to sleep"<<endl<<endl;
	            	counter_state = false;
	            }
	            //sem_post(&mutex1);//up on semaphore (exit for the critical section)
		        pthread_mutex_unlock(&mutex1);
		    }
	        usleep(300);
        } 
        return 0;   
    }

void *customer_runner(void* args)
    {
    	while(1)
    	{	
	        if(count_to_customer_id<min_customer_to_be_serviced)
	        {
	            //Critical section for producer
	            //sem_wait(&mutex1); //down on semphore (entry into critical section)
	            pthread_mutex_lock(&mutex1); 
	            count_to_customer_id++;
	            int new_customer_id = count_to_customer_id;
	            if(wait_q.size()<q_max)
	            {  
	                wait_q.push(new_customer_id);
	                cout<<"\x1B[34m \nNew customer with customer id "<<new_customer_id<<" entered into the queue \x1B[0m"<<endl;  
	            }
	            else
	            {
	                cout<<"\x1B[31m \nQueue is full, New customer with customer id "<<new_customer_id<<" can't enter into the queue!!! \x1B[0m"<<endl<<endl;
	                count_to_customer_id--; 
	            }
	            if(wait_q.size() == 1)
	            {
	            	cout<<"Customer with customer id "<<new_customer_id<<" is waking up service personel..."<<endl<<endl;
	            	counter_state = true;        //wake-up call for service personel
	            }
	            //sem_post(&mutex1); //up on semaphore (exit from the critcal section)
	            pthread_mutex_unlock(&mutex1);
	            usleep(100);
	        } 
	        else
	        	count_to_customer_id++;
        }
        return 0;   
    }


int main(int argc, char** argv)
{
	do
	{
		cout<<"Enter no. of service personel (1/2) : ";
		cin>>sp;
	    cout<<"Enter queue capacity : ";
	    cin>>q_max;
	}while(sp != 1 && sp != 2 && cout<<"\nNot a valid no. of service personels\n");    

    //sem_init(&mutex1, 0, 1);
    pthread_mutex_init(&mutex1, NULL);

    pthread_t sp1, sp2, cust;
    int log_tid_sp1 = 1, log_tid_sp2 = 2;

    cout<<"\n\n\t\t\t \x1B[32m \x1B[18m \x1B[1m \x1B[4m SERVICE COUNTER IS OPEN AND READY FOR SERVICE \x1B[0m"<<endl<<endl;
    cout<<"Service personel(s) are sleeping"<<endl;

    pthread_create(&sp1, NULL, service_personel_runner, &log_tid_sp1);
    if(sp == 2)	
        pthread_create(&sp2, NULL, service_personel_runner, &log_tid_sp2);

    pthread_create(&cust, NULL, customer_runner, NULL);
    while(!(count_to_customer_id>min_customer_to_be_serviced && wait_q.size()==0 && counter_state == false));
    usleep(500);
	/*while(clos == false)
	{
		pthread_mutex_lock(&mutex1);
		if(count_to_customer_id>20 && wait_q.size()==0 && counter_state == false)
			clos = true;
		pthread_mutex_unlock(&mutex1);
	}*/
	/*pthread_join(sp1, NULL);
	pthread_join(cust, NULL);
	if(sp == 2)
		pthread_join(sp2, NULL);*/
	cout<<"\n\n\t\t\t \x1B[31m \x1B[18m \x1B[1m \x1B[4m SERVICE COUNTER CLOSED \x1B[0m"<<endl<<endl;
	return 0;
}