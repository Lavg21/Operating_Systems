#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

unsigned int cod = 29410;

int main()
{

    if(access("RESP_PIPE_29410", 0) == 0)
    {
        unlink("RESP_PIPE_29410");
    }

    if(mkfifo("RESP_PIPE_29410", 0600) != 0)
    {
        printf("ERROR\ncannot create the response pipe\n");
    }

    int request_pipe = -1;
    request_pipe = open("REQ_PIPE_29410", O_RDONLY);

    if(request_pipe == -1)
    {
        printf("ERROR\ncannot open the request pipe\n");
    }

    int response_pipe = -1;
    response_pipe = open("RESP_PIPE_29410", O_WRONLY);

    int ok_connect = 1;

    unsigned int length_connect = strlen("CONNECT");
    if (write(response_pipe, &length_connect, 1) == -1)
    {

        ok_connect = 0;
    }

    if (write(response_pipe, "CONNECT", length_connect) == -1)
    {

        ok_connect = 0;
    }

    if (ok_connect)
    {
        printf("SUCCESS\n");
    }


    while(1)
    {

        unsigned int length_comanda;
        read(request_pipe, &length_comanda, 1);

        char* comanda = (char*)malloc(length_comanda * sizeof(char));
        read(request_pipe, comanda, length_comanda);

        if(strcmp(comanda, "PING") == 0)
        {

            write(response_pipe, &length_comanda, 1);
            write(response_pipe, "PING", 4);

            write(response_pipe, &length_comanda, 1);
            write(response_pipe, "PONG", 4);

            write(response_pipe, &cod, sizeof(cod));
        }

        if(strcmp(comanda, "CREATE_SHM") == 0)
        {

            unsigned int shm_nr;
            read(request_pipe, &shm_nr, sizeof(shm_nr));

            int shm_fd = shm_open("/PKphALQE", O_CREAT | O_RDWR, 0664);
            ftruncate(shm_fd, sizeof(char));

            int ok = 1;
            if(shm_fd < 0)
            {

                ok = 0;
            }


            unsigned int length_create_shm = strlen("CREATE_SHM");

            if (ok)
            {

                write(response_pipe, &length_create_shm, 1);
                write(response_pipe, "CREATE_SHM", length_create_shm);

                unsigned int length_success = strlen("SUCCESS");

                write(response_pipe, &length_success, 1);
                write(response_pipe, "SUCCESS", length_success);
            }

            else
            {

                write(response_pipe, &length_create_shm, 1);
                write(response_pipe, "CREATE_SHM", length_create_shm);

		unsigned int length_error = strlen("ERROR");

                write(response_pipe, &length_error, 1);
                write(response_pipe, "ERROR", length_error);
            }
        }


        if (strcmp(comanda, "MAP_FILE") == 0)
        {

            int file_name_size;
            read(request_pipe, &file_name_size, sizeof(file_name_size));

            char* file_name = (char*)malloc(file_name_size * sizeof(char));

            read(request_pipe, file_name, file_name_size);

            file_name[file_name_size] = '\0';

            int ok = 1;

            int file = open(file_name, O_RDONLY);
            if (file == -1)
            {

                ok = 0;
            }

            else
            {

                off_t size = lseek(file, 0, SEEK_END);
                lseek(file, 0, SEEK_SET);

                char* result = (char*) mmap(NULL, size, PROT_READ, MAP_PRIVATE, file, 0);

                if (result == (void*) -1)
                {

                    ok = 0;
                }
            }

            write(response_pipe, &length_comanda, 1);
            write(response_pipe, "MAP_FILE", length_comanda);

            if (ok)
            {
		unsigned int length_success = strlen("SUCCESS");

                write(response_pipe, &length_success, 1);
                write(response_pipe, "SUCCESS", length_success);
            }

            else
            {
		unsigned int length_error = strlen("ERROR");

                write(response_pipe, &length_error, 1);
                write(response_pipe, "ERROR", length_error);
            }
        }

        if(strcmp(comanda, "EXIT") == 0)
        {

            free(comanda);

            close(response_pipe);
            close(request_pipe);

            unlink("RESP_PIPE_29410");

            return 0;
        }


        free(comanda);

        return 0;
    }

}
