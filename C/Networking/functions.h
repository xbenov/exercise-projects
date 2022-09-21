#include <ctype.h>
#include <dirent.h>
#include <pwd.h>
#include <math.h>

#define BUFF 100
#define FILENAME "temp"
#define MAX_BUF 1024
#define INT_SIZE_BUF 6
#define PID_LIST_BLOCK 32
#define UP_TIME_SIZE 10

//funkcia na zistenie ci je string cislo
int check_if_number (char *str){
  	int i;
  	for (i=0; str[i] != '\0'; i++)
    		if (!isdigit(str[i]))
      			return 0;
  	return 1;
}
 

char *getUserName(int uid)
{
  struct passwd *pw = getpwuid(uid);
  if (pw)
  {
    return pw->pw_name;
  }

  return "";
}
 
//funkcia pre prikaz ps
void ps(){
	DIR *dirp;
	FILE *fp;
	struct dirent *entry;
	char path[MAX_BUF], read_buf[MAX_BUF],temp_buf[MAX_BUF];
	char uid_int_str[INT_SIZE_BUF]={0},*line;
	size_t len=0;
	long Hertz=sysconf(_SC_CLK_TCK);

	dirp = opendir ("/proc/");
	if (dirp == NULL){
		perror ("Fail");
		exit(0);
	} 
	  
	printf("PID\tTTY\tTIME\t\tCMD\n");
	
	while ((entry = readdir (dirp)) != NULL){
		//ak je nazov cislo je to proces
	    	if (check_if_number (entry->d_name)){
			strcpy(path,"/proc/");
			strcat(path,entry->d_name);
			strcat(path,"/stat");
			fp=fopen(path,"r");		//otvorenie stat o procese
			getline(&line,&len,fp);		//nacitanie riadku s informaciami
			char comm[10],state;		//premenne pre informacie
			unsigned int flags;
			int pid,ppid,pgrp,session,tty_nr,tpgid;
			unsigned long minflt,cminflt,majflt,cmajflt,utime,stime;
			unsigned long long starttime;
			long cutime,cstime,priority,nice,num_threads,itreavalue;
			
			//nacitanie jednotlivych informacii z riadku
			sscanf(line,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld  %ld %llu",&pid,comm,&state,&ppid,&pgrp,&session,&tty_nr,&tpgid,&flags,&minflt,&cminflt,&majflt,&cmajflt,&utime,&stime,&cutime,&cstime,&priority,&nice,&num_threads,&itreavalue,&starttime);
			
			//cas procesu
			unsigned long total_time=utime+stime;
			total_time=total_time+(unsigned long)cutime+(unsigned long)cstime;
		
			strcpy (path, "/proc/");
			strcat (path, entry->d_name);
			strcat (path, "/comm");
			 
		      	fp = fopen (path, "r");	
		      	if (fp != NULL){
				fscanf (fp, "%s", read_buf);	//zistenie command procesu
				fclose(fp);
		      	}
			
			//samotne vypisanie informacii
		      	fprintf(stdout,"%s\t%d\t%.2lu:%.2lu:%.2lu\t%s\n",entry->d_name,tty_nr,(total_time/Hertz)/3600,((total_time/Hertz)%3600)/60,((total_time/Hertz)%3600)%60,read_buf);
		     	 
		    }
	  } 
	printf("\n");
	closedir (dirp);
}

//funckia pre odoslanie suboru
int send_file(int server_socket,char *nazovSuboru){	
        ssize_t len;
        int fd;
        int sent_bytes = 0;
        char file_size[BUFF];
        struct stat file_stat;
        off_t offset;
        int remain_data;
	
	//otvorenie suboru
        fd = open(nazovSuboru, O_RDONLY);
        if (fd == -1){
                fprintf(stderr, "Error opening file --> %s", strerror(errno));

                exit(EXIT_FAILURE);
        }

        //ziskanie file stats
        if (fstat(fd, &file_stat) < 0){
                fprintf(stderr, "Error fstat --> %s", strerror(errno));

                exit(EXIT_FAILURE);
        }
	
	//ziskanie velkosti suboru
	sprintf(file_size, "%d", (int)file_stat.st_size);

        //poslanie velkosti suboru
        len = send(server_socket, file_size, sizeof(file_size), 0);
        if (len < 0){
              fprintf(stderr, "Error on sending file size --> %s", strerror(errno));

              exit(EXIT_FAILURE);
        }

        offset = 0;
        remain_data = file_stat.st_size;
	
        //posielanie suboru
        while (((sent_bytes = sendfile(server_socket, fd, &offset, BUFF)) > 0) && (remain_data > 	0))             
         remain_data -= sent_bytes;

	return 1;
}
//vypisanie zoznamu prikazov na obrazovku
void help(){
	printf("help - vypisanie prikazov\ninfo - vypisanie informacii o stroji\nmake [file] - vypisanie unikatnych riadkov v subore a ich pocet\nps - vypise vsetky procesy (ako ps -A)\nquit - ukoncenie spojenia\nhalt - ukoncenie programu\n\n");
}

//funkcia na zobrazenie informacii o stroji
void info(){
	FILE *fp;
	struct rusage usage;
	struct timeval time_kerel, time_proces;
	time_t seconds;
	int k;
	char os_name[50] = " ";
	unsigned int * time = (unsigned int *)malloc(sizeof(unsigned int));
	
    	getrusage(RUSAGE_SELF, &usage);
    	time_kerel = usage.ru_stime;
    	time_proces = usage.ru_utime;
    	
	//ziskanie nazvu operacneho systemu
    	fp = popen("lsb_release -ds", "r");
    	fgets(os_name, 50, fp);
    	pclose(fp);

	//ziskanie aktualneho datumu a casu
	asm("movl $0x0d,%%eax;"
    	    "int $0x80"
   	   :
    	   :"b"(time)
   	   :"%eax"
    	);
	seconds = *time;
	
	printf("Aktualny datum a cas je: %s", ctime(&seconds));
	printf("Memory usage: %ld KB\n", usage.ru_maxrss);
    	printf("CPU usage: ");
    	printf("%ld.%ld\n", (time_kerel.tv_sec + time_proces.tv_sec), (time_kerel.tv_usec + 			time_proces.tv_usec));
	printf("OS: %s\n", os_name);	
}

//funkcia na vykonavanie prikazov
int execute_command(char *command) {
    	if (strcmp(command, "info") == 0)
        	info();
	else if (strcmp(command, "ps") == 0)
		ps();
    	else if (strcmp(command, "halt") == 0)
        	return 0;
    	else if (strcmp(command, "help") == 0) {
        	help();
    	}
    	else if (strcmp(command, "quit") == 0) {
        	return -2;
    	}
    	else if (strcmp(command, "make") == 0) {
        	return 2;
    	}
    	else if (strcmp(command, "") == 0) {
        	return 1;
    	}
    	else {
		printf("Nerozpoznany prikaz -> %s\nPouzivam ako prikaz shellu ...\n", command);
		system(command);
        	return -1;
    	}
    	return 1;
}

//vypisanie unikatnych riadkov a ich pocet zo suboru
void make(char *file_name){
	//zobrazi unikatne riadky s poctom ich opakovani v subore
	char prikaz[50];
	char remove[50];
	strcpy(remove, "rm ");
	strcpy(remove+3, file_name);
    	strcpy(prikaz, "sort ");
	strcpy(prikaz+5, file_name);
	strcpy(prikaz+5+strlen(file_name), " | uniq -c");
    	system(prikaz);
	system(remove);
}

//funkcia na prijatie suboru
int recieve_file(int client_socket){	
        ssize_t len;
        struct sockaddr_in remote_addr;
        char buffer[BUFF];
	char file_name[50] = {'\0'};
        int file_size;
        FILE *received_file;
        int remain_data = 0;
	
        //prijatie velkosti suboru
        recv(client_socket, buffer, BUFF, 0);
        file_size = atoi(buffer);

        received_file = fopen(FILENAME, "w");
        if (received_file == NULL){
                fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));

                exit(EXIT_FAILURE);
        }

        remain_data = file_size;
	
	//zapisovanie prijatych dat do suboru
        while ((remain_data > 0) && ((len = recv(client_socket, buffer, BUFF, 0)) > 0)){
                fwrite(buffer, sizeof(char), len, received_file);
                remain_data -= len;
        }
        fclose(received_file);
	
	//volanie make na prijaty subor
	make(FILENAME);

	return 1;
}

//funkcia na zistenie argumentov pri spustani
int handle_arguments(char *s, char *t, char *l, char *c, char *d, int argc, char **argv){
    int i;
    for (i = 1; i < argc; i++){
        if (strcmp(argv[i], "-s") == 0)
            *s = 1;
        else if (strcmp(argv[i], "-t") == 0)
            *t = 1;
        else if (strcmp(argv[i], "-l") == 0)
            *l = 1;
        else if (strcmp(argv[i], "-d") == 0)
            *d = 1;
        else if (strcmp(argv[i], "-c") == 0)
            *c = 1;
        else {
            return 1;
        }
    }
    return 0;
}
