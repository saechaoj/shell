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
char* builtInCmd[] = {"#","exit","status","cd"};
int processBuilt(struct Command *x);
void cd(struct Command *x);
void sta();
void bg(struct Command *x);

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
	}
}
void sta()
{
	
} 

//Changes back directory or moves to a new one
void cd(struct Command *x)
{
	char newFilePath[50];
	ssize_t buf = 2046;
	
	if(x->ar[1] == NULL)
	{	

		chdir("..");
		
	}
	else
	{	
		//only checks first parameter
		sprintf(newFilePath,"/%s",x->ar[1]);
		chdir(newFilePath);	
	}	
	
		
	
}
//Takes in the struct to check global commands and # then returns int for next step
int processBuilt(struct Command *x)
{	
	printf("%s %s %s",x->ar[0],x->ar[1],x->ar[2]);
	struct Command *temp = x;
	if(strcmp(x->com,builtInCmd[0] ) == 0)
	{	
		return 1;
	}

	if(strcmp(x->com,builtInCmd[1] ) == 0)
	{
		exit(1);
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
		bg(cmd);
		cd(cmd);	
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

		if(strcmp(currCommand->ar[i], "<") == 0)
		{
			currCommand->inp = true;
			free(currCommand->ar[i]);
			i--;
		}
		else if(strcmp(currCommand->ar[i], ">") == 0)
		{
			currCommand->out = true;
			free(currCommand->ar[i]);
			i--;
	
		}
		
	//	else if(strcmp(currCommand->ar[i], "&") == 0)
	//	{	
	//		currCommand->bg = true;
	//		free(currCommand->ar[i]);
	//		i--;			
	//	}
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

	
