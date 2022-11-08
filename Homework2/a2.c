#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include "a2_helper.h"

typedef struct paramstruct {

	int process, index;

} paramstruct;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

int numar = 0, inchise = 0;
int ok = 0;

void* thread_fn_p7(void* args) {

	paramstruct* arg = (paramstruct*) args;

	// Thread-ul T7.4 trebuie sa inceapa inainte ca T7.2 sa inceapa si trebuie sa se termine dupa terminarea lui T7.2

	if (arg->index == 2)
		pthread_mutex_lock(&mutex1);

	info(BEGIN, arg->process, arg->index);
	
	if (arg->index == 4) {
		
		pthread_mutex_unlock(&mutex1);
		pthread_mutex_lock(&mutex2);
	}

	info(END, arg->process, arg->index);

	if (arg->index == 2)
		pthread_mutex_unlock(&mutex2);
	
	return NULL;
}

void* thread_fn_p3(void* args) {

	paramstruct* arg = (paramstruct*) args;

	info(BEGIN, arg->process, arg->index);
	info(END, arg->process, arg->index);

	return NULL;
}

void* thread_fn_p6(void* args) {

	// Threadul T6.1 trebuie sa se incheie inainte ca threadul T7.5 sa porneasca,
	// dar threadul T6.4 nu poate incepe decat dupa ce T7.5 s-a terminat.

	paramstruct* arg = (paramstruct*) args;

	info(BEGIN, arg->process, arg->index);

	info(END, arg->process, arg->index);

	return NULL;
}

int main(int argc, char** argv)
{
   
    init();

    info(BEGIN, 1, 0);

    pid_t p2 = fork();

    if (p2 == -1)
    {

        perror("Eroare la crearea procesului fiu P2!\n");
        return 1;
    }
    else if (p2 == 0)
    {
        // suntem pe P2
        info(BEGIN, 2, 0);
	info(END, 2, 0);
    }

    else
    {
        // suntem inapoi pe P1
        // waitpid(p2, NULL, 0);

        pid_t p3 = fork();

        if (p3 == -1)
        {
            perror("Eroare la crearea procesului fiu P3!\n");
            return 1;
        }

        else if (p3 == 0)
        {
            // suntem pe P3

            info(BEGIN, 3, 0);

		pthread_t tids[46];
		paramstruct param[46];

		for (int i = 0; i < 46; i++) {
			    
			param[i].index = i + 1;
			param[i].process = 3;

			pthread_create(&tids[i], NULL, thread_fn_p3, &param[i]);
		}

		for (int i = 0; i < 46; i++)
			pthread_join(tids[i], NULL);

            pid_t p4 = fork();

            if (p4 == -1)
            {

                perror("Eroare la crearea procesului fiu P4!\n");
                return 1;
            }

            else if (p4 == 0)
            {
                // suntem pe P4
		info(BEGIN, 4, 0);
		info(END, 4, 0);
            }

            else
            {
                // suntem inapoi pe P3
                waitpid(p4, NULL, 0);
		info(END, 3, 0);
            }

        }

        else
        {
            // suntem inapoi pe parinte

            pid_t p5 = fork();

            if (p5 == -1)
            {
                perror("Eroare la crearea procesului fiu P5!\n");
                return 1;
            }

            else if (p5 == 0)
            {
                // suntem pe P5

		info(BEGIN, 5, 0);

                pid_t p6 = fork();

                if (p6 == -1)
                {

                    perror("Eroare la crearea procesului fiu P5!\n");
                    return 1;
                }

                else if (p6 == 0)
                {
                    // suntem pe P6

		    info(BEGIN, 6, 0);

			pthread_t tids[5];
		    paramstruct param[5];

		    for (int i = 0; i < 5; i++) {
		    
				param[i].index = i + 1;
				param[i].process = 6;
				pthread_create(&tids[i], NULL, thread_fn_p6, &param[i]);
			}

			for (int i = 0; i < 5; i++)
				pthread_join(tids[i], NULL);

	  	    info(END, 6, 0);
                }

                else
                {
                    // suntem inapoi pe P5

                    waitpid(p6, NULL, 0);
		    info(END, 5, 0);
                }

            }

            else
            {
                // suntem inapoi pe parinte


                pid_t p7 = fork();

                if (p7 == -1)
                {

                    perror("Eroare la crearea procesului fiu P7!\n");
                    return 1;
                }
                else if (p7 == 0)
                {
                    // suntem pe P7
		    info(BEGIN, 7, 0);
		
			pthread_mutex_lock(&mutex1);
			pthread_mutex_lock(&mutex2);
		 
		    pthread_t tids[5];
		    paramstruct param[5];

		    for (int i = 0; i < 5; i++) {
		    
				param[i].index = i + 1;
				param[i].process = 7;
				pthread_create(&tids[i], NULL, thread_fn_p7, &param[i]);
			}

			for (int i = 0; i < 5; i++)
				pthread_join(tids[i], NULL);


		    info(END, 7, 0);
                }


                else
                {
                    // suntem inapoi pe P1

                    waitpid(p2, NULL, 0);
                    waitpid(p3, NULL, 0);
                    waitpid(p5, NULL, 0);
                    waitpid(p7, NULL, 0);
		    info(END, 1, 0);
                }


            }
        }
    }

    return 0;
}
