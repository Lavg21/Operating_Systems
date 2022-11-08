#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


// AFISARE ITERATIVA
int listDir(const char *path)
{
    // Declararea variabilelor necesare
    DIR* dir;
    struct dirent *entry;

    dir = opendir(path); // deschidem directorul

    // Verificare director
    if(dir == NULL)
    {
        printf("ERROR\ninvalid directory path\n"); // mesaj de eroare
        return -1;
    }
    else
    {
        printf("SUCCESS\n"); // mesaj de reusita
    }

    // Se citesc intrarile intr-o bucla
    while((entry = readdir(dir)) != NULL)
    {
        // Verificam directorul curent si anterior ca sa nu parcurgem la infinit
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            printf("%s/%s\n", path, entry->d_name); // afisam calea
        }
    }

    closedir(dir); // inchidere director

    return 0;
}

// AFISARE RECURSIVA
void listDirRec(char* path, int print_success)
{
    // Declararea variabilelor necesare
    DIR* dir = opendir(path); // deschidere director
    struct dirent *entry;
    struct stat statbuf;
    char filePath[512];

    entry = NULL;

    // Verificare director
    if(dir == NULL)
    {
        printf("ERROR\ninvalid directory path\n"); // mesaj de eroare
        return;
    }

    else
    {
        if(print_success == 1)
        {
            printf("SUCCESS\n"); // mesaj de reusita
            print_success = 0;
        }
    }

    // Se citesc intrarile intr-o bucla recursiva
    while((entry = readdir(dir)) != NULL)
    {
        // Verificam directorul curent si anterior ca sa nu parcurgem recursiv la infinit
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(filePath, 512, "%s/%s", path, entry->d_name); // constructia caii complete

            if(lstat(filePath, &statbuf) == 0)
            {
                printf("%s\n", filePath); // afisare

                if(S_ISDIR(statbuf.st_mode)) // verificare daca e director
                {
                    listDirRec(filePath, print_success); // apel recusiv
                }
            }
        }
    }

    closedir(dir); // inchidere director
}

// AFISARE TIP
void listDirExt(const char *path, const char *ext)
{
    // Declararea variabilelor necesare
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    struct stat statbuf;
    char filePath[512];

    dir = opendir(path); // deschidem directorul

    // Verificare director
    if(dir == NULL)
    {

        printf("ERROR\ninvalid directory path\n"); // mesaj de eroare
        return;

    }

    else
    {
        printf("SUCCESS\n"); // mesaj de reusita
    }

    // Se citesc intrarile intr-o bucla recursiva
    while((entry = readdir(dir)) != NULL)
    {
        // Verificam directorul curent si anterior ca sa nu parcurgem recursiv la infinit
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            if(strstr(entry->d_name, ext)) // verificam tipul
            {
                snprintf(filePath, 512, "%s/%s", path, entry->d_name); // constructia caii complete

                if(lstat(filePath, &statbuf) == 0)
                {
                    printf("%s\n", filePath); // afisam calea
                }
            }
        }
    }

    closedir(dir); // inchidere director
}

// AFISARE PERMISIUNI
void listDirPms(const char *path)
{
    // Declararea variabilelor necesare
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    struct stat statbuf;
    char filePath[512];

    dir = opendir(path); // deschidem directorul

    // Verificare director
    if(dir == NULL)
    {

        printf("ERROR\ninvalid directory path\n"); // mesaj de eroare
        return;

    }

    else
    {
        printf("SUCCESS\n"); // mesaj de reusita
    }

    // Se citesc intrarile intr-o bucla recursiva
    while((entry = readdir(dir)) != NULL)
    {
        // Verificam directorul curent si anterior ca sa nu parcurgem recursiv la infinit
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            //int verif = access(entry->d_name, F_OK);

            //printf("%s*\n", entry->d_name);
            //if(verif == 0)
            snprintf(filePath, 512, "%s/%s", path, entry->d_name); // constructia caii complete

            //int verif = access(filePath, W_OK); // apel de sistem care verifica permisiunea de write
            // fara access???
            //if(verif == 0) // verificare permisiune de write
            //{
                if(lstat(filePath, &statbuf) == 0)
                {
                // inca un if(statbuf.st_mode & S_IWUSR) ???
                    if(statbuf.st_mode & S_IWUSR)
                    {printf("%s\n", filePath); }// afisam calea
                }
            //}
        }
    }

    closedir(dir); // inchidere director
}

// IDENTIFICAREA SI PARSAREA FISIERELOR SF
int parseSF(char* path)
{
    // Declararea variabilelor necesare
    int fd;

    fd = open(path, O_RDONLY); // deschidere fisier

    // Verificare fisier
    if(fd == -1)
    {
        printf("Unable to open file!\n");
        return -1;
    }

    // Citire din fisier si verificare campuri

    char MAGIC[5]; // ca sa citeasca si '\0'
    read(fd, MAGIC, 4);

    if(strcmp(MAGIC, "stC0") != 0) // verificare camp MAGIC
    {
        printf("ERROR\nwrong magic\n"); // mesaj de eroare
        return -1;
    }

    int  HEADER_SIZE = 0, VERSION = 0, NO_OF_SECTIONS = 0;
    
    read(fd, &HEADER_SIZE, 2);
    read(fd, &VERSION, 4);

    if(!(VERSION >= 51 && VERSION <= 117)) // verificare camp VERSION
    {
        printf("ERROR\nwrong version\n"); // mesaj de eroare
        return -1;
    }

   read(fd, &NO_OF_SECTIONS, 1); 
    
    if(!(NO_OF_SECTIONS >= 6 && NO_OF_SECTIONS <= 11)) // verificare NO_OF_SECTIONS
    {
        printf("ERROR\nwrong sect_nr\n"); // mesaj de eroare
        return -1;
    }
    
    char NAME[18];
    int i, TYPE = 0, SIZE = 0, OFFSET = 0;
    // Intai parcurgem fara a afisa nimic, deoarece trebuie sa ne asiguram ca toate sectiunile sunt corecte
    for(i = 0; i < NO_OF_SECTIONS; i++)
    {
        read (fd, NAME, 18);
        NAME[18] = '\0';
        read (fd, &TYPE, 2);
        read (fd, &OFFSET, 4);
        read (fd, &SIZE, 4);

        // In cazul ca gasim un singur TYPE invalid, inseamna ca fisierul nu e valid
        if(TYPE != 73 && TYPE != 37 && TYPE !=  51 && TYPE != 92)
        {
            printf("ERROR\nwrong sect_types\n");
            return -1;
        }
    }

    // Asadar, intai vom parcurge ca sa verificam daca sunt corecte toate TYPE-urile, iar apoi
    // vom muta cursorul inapoi si vom citi cu afisare, stiind deja ca toate TYPE-urile sunt bune

    lseek(fd, 0, SEEK_SET);
    lseek(fd, 11, SEEK_CUR);

    printf("SUCCESS\n"); // mesaj de reusita

    // Afisari
    printf("version=%d\n", VERSION);
    printf("nr_sections=%d\n", NO_OF_SECTIONS);

    for(i = 0; i < NO_OF_SECTIONS; i++)
    {
        read(fd, NAME, 18);
        NAME[18]='\0';
        read(fd, &TYPE, 2);
        read(fd, &OFFSET, 4);
        read(fd, &SIZE, 4);

        printf("section%d: %s %d %d\n", i + 1, NAME, TYPE, SIZE);
    }

    return 0;
}

// IDENTIFICAREA SI PARSAREA FISIERELOR SF PENTRU FILTRAREA DUPA SECTIUNI
int parseSF_2(char* path)
{
    // Declararea variabilelor necesare
    int fd;

    fd = open(path, O_RDONLY); // deschidere fisier

    // Verificare fisier
    if(fd == -1)
    {
        return -1;
    }

    // Citire din fisier si verificare campuri

    char MAGIC[5]; // ca sa citeasca si '\0'
    read(fd, MAGIC, 4);

    if(strcmp(MAGIC, "stC0") != 0) // verificare camp MAGIC
    {
        return -1;
    }

    int  HEADER_SIZE = 0, VERSION = 0, NO_OF_SECTIONS = 0;
    
    read(fd, &HEADER_SIZE, 2);
    read(fd, &VERSION, 4);

    if(!(VERSION >= 51 && VERSION <= 117)) // verificare camp VERSION
    {
        return -1;
    }

   read(fd, &NO_OF_SECTIONS, 1); 
    
    if(!(NO_OF_SECTIONS >= 6 && NO_OF_SECTIONS <= 11)) // verificare NO_OF_SECTIONS
    {
        return -1;
    }
    
    char NAME[18];
    int i, TYPE = 0, SIZE = 0, OFFSET = 0;
    
    // Intai parcurgem fara a afisa nimic, deoarece trebuie sa ne asiguram ca toate sectiunile sunt corecte
    for(i = 0; i < NO_OF_SECTIONS; i++)
    {
        read (fd, NAME, 18);
        NAME[18] = '\0';
        read (fd, &TYPE, 2);
        read (fd, &OFFSET, 4);
        read (fd, &SIZE, 4);

        // In cazul ca gasim un singur TYPE invalid, inseamna ca fisierul nu e valid
        if(TYPE != 73 && TYPE != 37 && TYPE !=  51 && TYPE != 92)
        {
            return -1;
        }
    }

    // Asadar, intai vom parcurge ca sa verificam daca sunt corecte toate TYPE-urile, iar apoi
    // vom muta cursorul inapoi si vom citi, stiind deja ca toate TYPE-urile sunt bune

    lseek(fd, 0, SEEK_SET);
    lseek(fd, 11, SEEK_CUR);

    for(i = 0; i < NO_OF_SECTIONS; i++)
    {
        read(fd, NAME, 18);
        NAME[18]='\0';
        read(fd, &TYPE, 2);
        read(fd, &OFFSET, 4);
        read(fd, &SIZE, 4);

	if (SIZE > 1064) // verificarea campului SIZE conform cerintei
		return -1;
    }

    return 0;
}

// FILTRAREA DUPA SECTIUNI
void findAllSF(char* path, int print_success)
{
    // Declararea variabilelor necesare
    DIR* dir = opendir(path); // deschidere director
    struct dirent *entry;
    struct stat statbuf;
    char filePath[512];

    entry = NULL;

    // Verificare director
    if(dir == NULL)
    {
        printf("ERROR\ninvalid directory path\n"); // mesaj de eroare
        return;
    }

    else 
    {

	if(print_success == 1)
        {
            printf("SUCCESS\n"); // mesaj de reusita
            print_success = 0;
        }
    }

    // Se citesc intrarile intr-o bucla recursiva
    while((entry = readdir(dir)) != NULL)
    {
        // Verificam directorul curent si anterior ca sa nu parcurgem recursiv la infinit
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(filePath, 512, "%s/%s", path, entry->d_name); // constructia caii complete

            if(lstat(filePath, &statbuf) == 0)
            {
		// Verificam daca fisierul curent este un fisier SF corect, folosind functia parseSF_2
		if(parseSF_2(filePath) == 0)
                	printf("%s\n", filePath); // afisare

                if(S_ISDIR(statbuf.st_mode)) // verificare daca e director
                {
                    findAllSF(filePath, print_success); // apel recusiv
                }
            }
        }
    }

    closedir(dir); // inchidere director
}

int main(int argc, char **argv)
{

    // Verificare argumente
    if(argc < 2)
    {
        printf("Eroare\n");
        return 1;
    }


    // Verificare argumente
    if(argc >= 2)
    {
        // 2.2 AFISAREA VARIANTEI
        if(strcmp(argv[1], "variant") == 0)
        {
            printf("29410\n"); // se afiseaza numarul variantei
            return 0;
        }


        // 2.3 AFISAREA CONTINUTULUI
        int iterativ = 0; // variabila necesara pentru verificarea afisarii iterative si recursive

        if(strcmp(argv[1], "list") == 0) // daca primul parametru e "list"
        {
            // AFISARE ITERATIVA
            int lungime = strlen(argv[2]) - 5; // scadem numarul caracterelor verificate in if
            char* path = (char*)malloc(lungime * sizeof(char)); // alocam dinamic memorie ca sa il putem transmite functiei

            if(strstr(argv[2], "path=") && argc < 4) // daca al doilea argument e path si sunt singurele argumente
            {
                strcpy(path, argv[2] + 5); // retinem subsirul de dupa cel verificat mai sus

                listDir(path); // apel functie

                iterativ = 1; // marcam faptul ca s-a afisat iterativ
            }

            free(path); // dezalocare memorie
        }

	// AFISARE RECURSIVA
        if(iterativ == 0 && argc >= 4) // verificare afisare recursiva(pozitia parametrului "recursive")
        {
            if(strcmp(argv[2], "recursive") == 0) 
            {
                if(strstr(argv[3], "path=")) 
                {
                    int lungime = strlen(argv[3]) - 5; // scadem numarul caracterelor verificate in if
                    char* path = (char*)malloc(lungime * sizeof(char)); // alocam dinamic memorie ca sa il putem transmite functiei

                    strcpy(path, argv[3] + 5); // retinem subsirul de dupa cel verificat mai sus

                    listDirRec(path, 1); // apel functie

                    free(path); // dezalocare memorie
                }
            }

            else if(strcmp(argv[3], "recursive") == 0)
            {
                if(strstr(argv[2], "path="))
                {
                    int lungime = strlen(argv[2]) - 5; // alocam dinamic memorie ca sa il putem transmite functiei
                    char* path = (char*)malloc(lungime * sizeof(char)); // alocam dinamic memorie ca sa il putem transmite functiei

                    strcpy(path, argv[2] + 5); // retinem subsirul de dupa cel verificat mai sus

                    listDirRec(path, 1); // apel functie

                    free(path); // dezalocare memorie
                }
            }
        }

        // AFISARE TIP
        if(strstr(argv[2], "name_ends_with=") && strstr(argv[3], "path="))
        {
            int lg = strlen(argv[2]) - 15; // scadem numarul caracterelor verificate in if
            char *ext = (char*)malloc(lg*sizeof(char)); // alocam dinamic memorie ca sa il putem transmite functiei

            strcpy(ext, argv[2] + 15); // retinem subsirul de dupa cel verificat mai sus

            int lungime = strlen(argv[3]) - 5; // scadem numarul caracterelor verificate in if
            char *path = (char*)malloc(lungime*sizeof(char)); // alocam dinamic memorie ca sa il putem transmite functiei

            strcpy(path, argv[3] + 5); // retinem subsirul de dupa cel verificat mai sus

            listDirExt(path, ext); // apel functie

            // Dezalocare memorie
            free(ext);
            free(path);
        }

        // AFISARE PERMISIUNI
        if(strstr(argv[2], "has_perm_write") && strstr(argv[3], "path="))
        {
            int lungime = strlen(argv[3]) - 5; // scadem numarul caracterelor verificate in if
            char *path = (char*)malloc(lungime*sizeof(char)); // alocam dinamic memorie ca sa il putem transmite functiei

            strcpy(path, argv[3] + 5); // retinem subsirul de dupa cel verificat mai sus

            listDirPms(path); // apel functie

            free(path); // dezalocare memorie
        }

	// 2.4 IDENTIFICAREA SI PARSAREA FISIERELOR SF
        if(strcmp(argv[1], "parse") == 0 && strstr(argv[2], "path="))
        {
            int lungime = strlen(argv[2]) - 5; // scadem numarul caracterelor verificate in if
            char* path = (char*)malloc(sizeof(char) * lungime); // alocam dinamic memorie ca sa il putem transmite functiei

            strcpy(path, argv[2] + 5); // retinem subsirul de dupa cel verificat mai sus

            parseSF(path); // apel functie

            free(path); // dezalocare memorie
        }

	// 2.5

	// 2.6 FILTRAREA DUPA SECTIUNI
	if(strcmp(argv[1], "findall") == 0 && strstr(argv[2], "path="))
	{
	
		int lungime = strlen(argv[2]) - 5; // scadem numarul caracterelor verificate in if
            	char* path = (char*)malloc(sizeof(char) * lungime); // alocam dinamic memorie ca sa il putem transmite functiei

            	strcpy(path, argv[2] + 5); // retinem subsirul de dupa cel verificat mai sus

            	findAllSF(path, 1); // apel functie

            	free(path); // dezalocare memorie
	}

    }

    return 0;
}

