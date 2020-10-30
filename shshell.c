#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>


void showCommand();
void execute(char** argv);
char* r_line();
char* parseLine(char* line);
char* builtInCmd[] = {"exit","cd","status"};

int main()
{	
	char* arr;
	showCommand();
	arr = r_line();
	parseLine(arr);
	return 0;
}


//Takes in a buffer and creates tokens for arguements. Will be used to compare with buildInCMD
char* parseLine(char* line)
{
	
	char** arg = malloc(512  * sizeof(char*));
	char* token = strtok(line," ");
	int i = 0;
	while(token != NULL)
	{
		arg[i] = token;
		token = strtok(NULL," ");
		printf("%s\n",arg[i]);
		i++;
	}	
	
	return *arg;
}
//Reads in the line and stores it in a buffer then returns it.
char* r_line()
{
	char* bLine = NULL;
	ssize_t buffer = 2046;
	
	if(getline(&bLine,&buffer,stdin) != -1 )
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

	
