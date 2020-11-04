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




struct Command
{
	char* com;
	char* ar[512];
	bool bg;
	int inp;
	int out;
	bool bi;
	bool bo;

};



struct Node
{
	int pid;
	struct Node* next;
};




int main()
{

	processCMD();
	return 0;
}





// Takes out the < and/or > then replaces it with file commands then return number of arguements.
int io(struct Command *x)
{
	// loop through and find the < or > on else it****	
	if(x->bi ==1 && x->bo ==1)
	{

		x->ar[x->inp] = NULL;
		x->ar[x->out] = NULL;
	
		free(x->ar[x->inp]);
		free(x->ar[x->out]);


		x->ar[x->inp] = calloc(strlen(x->ar[x->inp + 1]) + 1 ,sizeof(char));
		strcpy(x->ar[x->inp],x->ar[x->inp+1]);

		x->ar[x->out-1] = calloc(strlen(x->ar[x->out + 1]) + 1 ,sizeof(char));

		strcpy(x->ar[x->out-1],x->ar[x->out+1]);

		
		x->ar[x->out+1] = NULL;
		free(x->ar[x->out+1]);
		
		return 3;

	}


	if( x->bi == 1 || x->bo == 1)
	{
		int i = 0;

		while(x->ar[i] != NULL)
		{

			if(strcmp(x->ar[i],"<") == 0 || strcmp(x->ar[i],">") == 0)
			{
				x->ar[i] = NULL;
				free(x->ar[i]);
				x->ar[i] = calloc(strlen(x->ar[i+1]) + 1 ,sizeof(char));
				strcpy(x->ar[i],x->ar[i+1]);
				x->ar[i+1] = NULL;
				free(x->ar[i+1]);
			
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
}




//Collects and stores last forground exit value or signal flag
void sta()
{
	
} 





//Changes back directory or moves to a new one ** need home envirement and check file in dir
void cd(struct Command *x)
{	

	//**** Need to get home envirement and check for file in dir
	char newFilePath[50];
	ssize_t buf = 2046;
	
	if(x->ar[1] == NULL)
	{	
		chdir("..");
		
	}
	else
	{	
		sprintf(newFilePath,"/%s",x->ar[1]);
		if(chdir(newFilePath) == -1)
		{
			perror("New Path");
		}
	}	
	
		
	
}




//Takes in the struct to check global commands and # then returns int for next step
int processBuilt(struct Command *x)
{	
	printf("%s\n", x->com);
	if(strcmp(x->com,builtInCmd[0] ) == 0)
	{	
		return 1;
	}

	if(strcmp(x->com,builtInCmd[1] ) == 0)
	{
		return 2;
	}

	
	if(strcmp(x->com,builtInCmd[2] ) == 0)
	{
		return 3;
	}

	if(strcmp(x->com,builtInCmd[3] ) == 0)
	{
		return 4;
	}	
	else
	{
		return 5;
	}	



}





//handles the sign number
void sighandler(int signum)
{
	char* message = "Caught SigInt, sleeping for 10 seconds\n";
	write(STDOUT_FILENO, message, 39);
	sleep(10);
}




void processCMD()
{	
	int pb = 0;	
	int counter = 0;
	struct Command *cmd;
	char* line;
	

	while(counter == 0)
	{	


		showCommand();
		line = r_line();
		cmd = parseLine(line);
		pb = processBuilt(cmd);
		
		if(pb == 5)
		{
			
			bg(cmd);

			if(io(cmd) == 0)
			{
				// fork run exe
			}

			else if(io(cmd) == 2)
			{
				//do in or out then fork ex
			}

			else if(io(cmd) == 3)
			{
				//do both fork then exe
			}

		}


		else if(pb == 4)
		{
			// Changes directory
			cd(cmd);

		}
		else if(pb == 3)
		{	
			// Status
			sta();
		}
		else if(pb == 2)
		{	
			// Exit, when fork save process ID like in a linked list. run thru kill process
			exit(1);
		}
		else if(pb == 1)
		{
			// # Symbol
			printf("");
		}
		
	}
		
}


//Takes in a buffer and creates tokens for arguements. Will be used to compare with buildInCMD
struct Command *parseLine(char* line)
{
	
	struct Command *currCommand = malloc(sizeof(struct Command));
	char* token = strtok(line," ");
	currCommand->com = calloc(strlen(token)+1,sizeof(char));
	strcpy(currCommand->com,token);
	int s = strlen(currCommand->com);
	currCommand->com[s-1] = 0;
	int i = 0;
	size_t n;
	while(token != NULL)
	{	
	
		currCommand->ar[i] = calloc(strlen(token)+1,sizeof(char));	
		strcpy(currCommand->ar[i], token);

		//checks input and output then sets boolean and index
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
    perror("Exec failure!");
    exit(1);
  }
}

	
