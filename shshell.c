#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>




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
void execute(char* argv)
{
  if (execvp(*argv, argv) < 0)
  {
    perror("Exec failure!");
    exit(1);
  }
}

int main()
{
  pid_t spawnPid = -5;
  int childExitMethod = -5;
  spawnPid = fork();
  if (spawnPid == -1) //
    {
      perror("Hull Breach!\n");
      exit(1);
    }
  else if (spawnPid == 0) // Terminate the child process immediately
  {
    printf("CHILD: PID: %d, exiting!\n", spawnPid);
    exit(0);
  }
  printf("PARENT: PID: %d, waiting...\n", spawnPid);
  waitpid(spawnPid, &childExitMethod, 0);
  printf("PARENT: Child process terminated, exiting!\n");
  exit(0);


}
