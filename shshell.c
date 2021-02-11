#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/stat.h>

void sighandler(int);
void processCMD();
void showCommand();
void execute(char** argv);
char* r_line();
struct Command  *parseLine(char* line);
char* builtInCmd[] = {"#","exit","status","cd"};
int processBuilt(struct Command *x);
void cd(struct Command *x);
void sta();
void bg(struct Command *x);
int io(struct Command *x);
void processIO(struct Command *x,int i);
void signalFunc();
void toggleFG(int);
int globalFG;
char* str_replace(char* x,char[],char* z);

void newProcess(struct Command *x);


struct Command
{
	char* com;
	char* ar[512];
	bool bg;
	int inp;
	int out;
	bool bi;
	bool bo;
	char* input;
	char* output;

};




//Signal handlers initialized
int main()
{
	struct sigaction SIGINT_action = {0},SIGTSTP_action = {0}, ignore_action = {0};

	SIGTSTP_action.sa_handler = toggleFG;
	sigfillset(&SIGTSTP_action.sa_mask);
	SIGTSTP_action.sa_flags = SA_RESTART;
	sigaction(SIGTSTP,&SIGTSTP_action,NULL);	



	processCMD();


	return 0;
}





void newProcess(struct Command *x)
{
	pid_t child = -5;
	pid_t child2 = -5;

	int j = 0;
	while(x->ar[j] != NULL)
	{

		j++;
	}
	int len = strlen(x->ar[j-1]);

	if(x->ar[j-1][len-1] == '\n')
	{

		x->ar[j-1][len-1] = '\0';
	}

	int childStatus;
	int childStatus2;
	child = fork();
	switch(child)
	{
		case -1:
			perror("fork() failed!\n");
			fflush(stdout);
			exit(1);
			break;
	
		case 0:
		//	printf("Child PID : %d\n", getpid());
			
			if(x->bg == true)
			{
				execute(x->ar);
				sleep(10);	
			}
			else
			{
				execute(x->ar);
			
			}

			break;	
	

		default:
		
		if(x->bg == true)
		{

			printf("Background process started with PID %d\n", child);
			fflush(stdout);
			
			if(WIFEXITED(childStatus))
			{
				printf("Child exited normally with status %d\n", child,WEXITSTATUS(childStatus));
				fflush(stdout);
			}

			


		 }

		else
		{

			child = waitpid(child,&childStatus,0);
			break;

		}
		
		
	}	

	//both run here
}





// Replaces $$ with PID and keeps chars that arent $$
char* str_replace(char* x,char* y,char* z)
{
	char buffer[1024] = { 0 };
        char *insert_point = &buffer[0];
        const char *tmp = x;
        size_t needle_len = strlen(y);
        size_t repl_len = strlen(z);

	while(1)
	{
		const char*p =strstr(tmp,y);

		if (p == NULL) 
		{
           	 	strcpy(insert_point, tmp);
          	 	break;
      		}


		memcpy(insert_point, tmp, p - tmp);
       		insert_point += p - tmp;

		memcpy(insert_point, z, repl_len);
        	insert_point = insert_point + repl_len;

		tmp = p + needle_len;
	}

	strcpy(x,buffer);
	return x;
  
}



//handles the sign number
void sighandler(int signum)
{
	char* message = " Signal 2 Caught\n";
	write(STDOUT_FILENO, message, 39);
	fflush(stdout);
}





//FG handler catches signal then changes global var so & is ignored
void toggleFG(int signum)
{
	if (globalFG == 1)
 	{
		char* message = " Foreground only mode EXIT\n:";
 		write(STDOUT_FILENO, message, 28);
		globalFG = 0;
		fflush(stdout);
 
	}
	else
	{
		char* message = " Foreground only mode ACTIVATED\n:";
		write(STDOUT_FILENO, message, 33);
		globalFG = 1;
		fflush(stdout);
 	}
}






//takes in  integer to process input or out
void proccessIO(struct Command *x, int i)
{
	char* def = "/dev/null";
	if(i == 3)
	{
      
	 
		 int sourceFD = open(x->input, O_RDONLY);
 		 if (sourceFD == -1)
		 { 
      			perror("error"); 
    	 	 }

		  int result = dup2(sourceFD, 0);
	 	 if (result == -1) 
	         { 
  			  perror("error"); 
		 }
	

		


		int targetFD = open(x->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  		if (targetFD == -1) 
		{ 
   			 perror("error)"); 
 		 }
  		 result = dup2(targetFD, 1);
 		if (result == -1) 
		{ 
    			perror("error"); 
		}
	}
	

	else if(x->output == NULL)
	{


	 int sourceFD = open(x->input, O_RDONLY);
 		 if (sourceFD == -1)
		 { 
      			perror("error"); 
      		
  		}

		  int result = dup2(sourceFD, 0);
	 	 if (result == -1) 
		{ 
  			  perror("error"); 
		
   			 
		  }

	}


	else if(x->input == NULL)
	{


		int targetFD = open(x->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  			if (targetFD == -1) 
			{ 
   				 perror("error"); 
   				 exit(1); 
 			 }
  			int result = dup2(targetFD, 1);
 			if (result == -1) 
			{ 
    				perror("error"); 
	    			exit(2); 
			  }


	}



}





// Takes out the < and/or > then replaces it with file commands then return number of arguements.
int io(struct Command *x)
{
	int j = 0;	
	int i = 0;
	char* star;


	if(x->bi ==1 && x->bo ==1)
	{


		x->input= calloc(strlen(x->ar[x->inp+1])+1,sizeof(char));
		x->output= calloc(strlen(x->ar[x->out+1])+1,sizeof(char));

		x->output = x->ar[x->out+1];
		x->input = x->ar[x->out+1];
		x->ar[x->inp] = NULL;
		x->ar[x->out] = NULL;



	
		x->ar[x->out+1] = NULL;
		x->ar[x->inp+1] = NULL;
		
		return 3;

	}



	else if( x->bi == 1 || x->bo == 1)
	{
		i = 0;

		while(x->ar[i] != NULL)
		{
			if(strcmp(x->ar[i],">") == 0)
			{
				x->ar[i] = NULL;
				x->output= calloc(strlen(x->ar[i+1])+1,sizeof(char));
				x->output = x->ar[i+1];
				x->ar[i+1] = NULL;
				free(x->ar[i]);
				free(x->ar[i+1]);
				x->input = NULL;
				break;
		
			}



			if(strcmp(x->ar[i],"<") == 0)
			{
				x->ar[i] = NULL;
				x->input = calloc(strlen(x->ar[i+1])+1,sizeof(char));
				x->input = x->ar[i+1];
				x->ar[i+1] = NULL;
				free(x->ar[i]);
				free(x->ar[i+1]);
				x->output = NULL;
				break;
		
			}




			i++;
		}		
	
		return 2;

	}

	else
	{
		return 0;
	}
}




//checks the background by comparing last argument in array to &
void bg(struct Command *x)
{	

	char s [50];
	int j = 0;
	int i = 0;
	while(x->ar[i] != NULL)
	{
		i++;
	}
	strcpy(s,x->ar[i-1]);
	j = strlen(s);
	s[j-1] = 0;	
	if(strcmp(s,"&")== 0)
	{	
		x->bg = true;
		x->ar[i-1] = NULL;
		free(x->ar[i-1]);
	}
	if(globalFG == 1)
	{
		x->bg = false;
	}
}




//Collects and stores last forground exit value or signal flag
void sta(int c)
{

	if (WIFEXITED(childExitMethod) != 0) {
		printf("exit value %d\n", WEXITSTATUS(childExitMethod));
		fflush(stdout);
	}
	if (WIFSIGNALED(childExitMethod) != 0 ) {
		printf("terminated by signal %d\n", WTERMSIG(childExitMethod));
		fflush(stdout);
	}
	return;
}
	


 





//Changes back to home envirment directory or moves to a new one 
void cd(struct Command *x)
{	

	char newFilePath[250];
	ssize_t buf = 2046;
	
	if(x->ar[1] == NULL)
	{	
		strcpy(newFilePath,getenv("HOME"));
		if(chdir(newFilePath) == -1)
		{
			perror("error");
		}
		
	}
	else
	{	
		int l = strlen(x->ar[1]);
		x->ar[1][l-1] = '\0';
		sprintf(newFilePath,"/%s",x->ar[1]);
		
		printf("%s Working dir", newFilePath);
		if(chdir(newFilePath) == -1)
		{
			perror("error");
			exit(1);
		}
	}	
	
		
	
}




//Takes in the struct to check global built in commands and # , then returns int to process built in
int processBuilt(struct Command *x)
{
	char str[50];
	
	int len = strlen(x->com);
	strcpy(str, x->com);
	len = strlen(str);

	if(str[len-1] == '\n')
	{
		 str[len-1] = 0;
	}
	if(strcmp(str,builtInCmd[0] ) == 0)
	{	
		return 1;
	}

	if(strcmp(str,builtInCmd[1] ) == 0)
	{
		return 2;
	}

	
	if(strcmp(str,builtInCmd[2] ) == 0)
	{
		return 3;
	}

	if(strcmp(str,builtInCmd[3] ) == 0)
	{
		return 4;
	}	
	else
	{
		return 5;
	}	



}




//Runs shell
void processCMD()
{	

	
	int pb = 0;	
	int counter = 0;
	struct Command *cmd;
	char* line;	
	globalFG = 0;
	char* kill[50];
	int c = 0;

	while(counter == 0)
	{	

		char pin[10];
		sprintf(pin,"%d",getpid());
	//	printf("%s", pin);
		
		showCommand();
		line = r_line();
		
		if(strlen(line) != 0)
		{
			
			
			cmd = parseLine(line);
			pb = processBuilt(cmd);
			bg(cmd);
			fflush(stdout);

			if(pb == 5)
			{
				int i = io(cmd);
				if(i == 0)
				{
					newProcess(cmd);
				}

				else if(i == 2)
				{
					proccessIO(cmd,2);
					newProcess(cmd);
				}

				else if(i == 3)
				{	
					proccessIO(cmd,3);
					newProcess(cmd);
				}

			}


			else if(pb == 4)
			{
				cd(cmd);

			}
			else if(pb == 3)
			{	
				sta();
			}
			else if(pb == 2)
			{	
				exit(1);
			}
			else if(pb == 1)
			{
				printf("");
			}

		}
		
	}
		
}


//Takes in a buffer and creates tokens for arguements. Com will be used to compare with buildInCMD
//Didthe first malloc because I mis understood direction 

struct Command *parseLine(char* line)
{
	
	struct Command *currCommand = malloc(sizeof(struct Command));
	char* token = strtok(line," ");
	currCommand->com = calloc(strlen(token)+1,sizeof(char));
	strcpy(currCommand->com,token);
	int i = 0;
	size_t n;
	
	// for $$
	char substr[]= "$$";
	int p = getpid();
	char* pp;
	pp = calloc(20,sizeof(char));
	sprintf(pp,"%d",p);
	int substr_size = strlen(pp);
	char* ptr;
	char* ptr2;


	while(token != NULL)
	{	
	
		currCommand->ar[i] = calloc(strlen(token)+1,sizeof(char));	
		strcpy(currCommand->ar[i], token);

		ptr = calloc(strlen(currCommand->ar[i])+100, sizeof(char));
		strcpy(ptr,currCommand->ar[i]);
		ptr2 = strstr(ptr,substr);	
	
		if(strcmp(currCommand->ar[i], "<") == 0)
		{
			currCommand->inp = i;
			currCommand->bi = true;
		}
		else if(strcmp(currCommand->ar[i], ">") == 0)
		{
			currCommand->out = i;
			currCommand->bo = true;
	
		}
		else if(ptr2 != NULL)
		{	
			currCommand->ar[i] = NULL;
			free(currCommand->ar[i]);
		
			str_replace(ptr,substr,pp);
			
			currCommand->ar[i] = calloc(strlen(ptr)+1,sizeof(char));
			strcpy(currCommand->ar[i],ptr);
			
			ptr = NULL;
			free(ptr);

		}
		
		token = strtok(NULL," ");	
		i++;
	}
	
	return currCommand;
}





//Reads in the line and stores it in a buffer then returns it.
char* r_line()
{
	char* bLine = NULL;
	ssize_t buffer = 2046;
	if(getline(&bLine,&buffer,stdin) == -1 )
	{
		perror("reading line");
	}

	return bLine;
		
}



//clears screen and shows prompt
void showCommand()
{

  static int shellDescriptor = 0;
  if(shellDescriptor == 0)
  {
    system("clear");
    printf("***** Jarrod's Shell *****\n");
    shellDescriptor = 1;
  }
    printf(":");

}


//executes function
void execute(char** argv)
{
  if (execvp(*argv, argv) < 0)
  {
    printf("Command not found\n");
	fflush(stdout);
    exit(1);
  }
}

	
