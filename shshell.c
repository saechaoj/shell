#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>

void sighandler(int);
void processCMD();
void showCommand();
void execute(char** argv);
char* r_line();
struct Command  *parseLine(char* line);
int cd(char** arg);
int shExit(char** arg);
int shStatus(char** arg);
char* builtInCmd[] = {"#","exit","status","cd"};
int processBuilt(struct Command *x);
void cd();
void ex();
void sta();

struct Command
{
	char* com;
	char* ar[512];
	bool bg;
	bool inp;
	bool out;
};


int main()
{	
	processCMD();
	printf("We made it here!");
	return 0;
}


//Takes in the struct to check global commands and # then returns int for next step
int processBuilt(struct Command *x)
{	

	struct Command *temp = x;
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

	return 5;


}


void sighandler(int signum)
{
	printf("Signal %d was caught\n",signum);
}


int shExit(char** arg)
{
	return 0;	

}
int cd(char** arg)
{
	if(builtInCmd[1] == NULL)
	{
		fprintf(stderr,": cd function not recognized\n");
	}

	return 1;
}





void processCMD()
{	

	int counter = 0;
	struct Command *cmd;
	char* line;
	while(counter == 0)
	{
		showCommand();
		line = r_line();
		cmd  = parseLine(line);
		processBuilt(cmd);	
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
		s = strlen(currCommand->ar[i]);
		currCommand->ar[i][s-1] = 0;
		if(strcmp(currCommand->ar[i], "<") == 0)
		{
			currCommand->inp = true;
		}
		if(strcmp(currCommand->ar[i], ">") == 0)
		{
			currCommand->out = true;
		}
		
		if(strcmp(currCommand->ar[i], "&") == 0)
		{	
			currCommand->bg = true;
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
void showCommand()
{
  // clears the screen to show the prompt of the shell
  static int shellDescriptor = 0;
  if(shellDescriptor == 0)
  {
    system("clear");
    printf("***** Jarrod's Shell *****\n");
    shellDescriptor = 1;
  }
    printf(":");

}
void execute(char** argv)
{
  if (execvp(*argv, argv) < 0)
  {
    perror("Exec failure!");
    exit(1);
  }
}

	
