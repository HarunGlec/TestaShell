#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h> 
#include <string.h>
#include <signal.h>

#define LINE_BUFSIZE 1024 // define input line size
#define SWITCH_BUFSIZE 64 // define command size
#define SEPERATOR " \t\r\n\a" // define seperator between command and parameters

char *readLine(FILE* file);
char **parser(char *line);
int backgroundJob(char** arg_list);
int runProgram(char **args);
int execute(char **args);
int backgroundCheck(char **args);
int historyRead();
void historyWrite(char *line);

char history[20][255]={0}; // shell history
int counter=0; 
FILE *p; // shell history file
char historyFilename[255];

void sig_handler (int signum)
{	
	/*
	//when received SIG QUIT(Ctrl+\) signal print history
 	*/
 	
 	int k=0;
 	printf("\n");
	for(k=0;k<20;k++)
	{
		printf("%s",history[k]);
	}
}

int main (int argc, char* argv[]) 
{
	char *line;
	char **arguments;
	int status;
	
	char *scriptName ;
	FILE* file ; 
	int firstLine=1;

    
	if (signal(SIGQUIT, sig_handler) == SIG_ERR)
  		printf("\ncan't catch SIGQUIT\n");
	
	historyRead(); // Read history from file
	
	if (argc==1)
	{
		// Running interactively
		do {
			printf("Prompt > ");
			line = readLine(stdin); // Read user input
			historyWrite(line); // Write inputs to history
			arguments = parser(line); // Parse user input as command and paremeters
			status = execute(arguments); // Execute command.

			counter++; // for history		
			free(line); // free space of inputs
			free(arguments);
		} while (status);
	}else if(argc==2)
	{
		// Running non-interactively, Read commands from file
		scriptName = argv[1];
		file = fopen(scriptName, "r"); 
		while(!feof(file))
		{
			line = readLine(file);
			if(firstLine)
			{
				free(line);
				firstLine=0;
				continue;
			}
			historyWrite(line);
			arguments = parser(line);
			status = execute(arguments);
			
			counter++;		
			free(line);
			free(arguments);
		}
		fclose(file);
		*arguments="exit";
		execute(arguments);
	}
} 
char *readLine(FILE* file)
{
	/*
	// Functions return user inputs from file as char* 
	// if it is in interactive mode take stdin as argument
	// e.g readline(stdin)
	*/
	
	int buffersize = LINE_BUFSIZE;
	int position = 0;
  	char *buffer = malloc(sizeof(char) * buffersize);
  	int c;

	if (!buffer) 
	{
		fprintf(stderr, ">> allocation error\n");
		exit(EXIT_FAILURE);
	}

	while (1) 
	{
		// Read a character
		c = getc(file);
		
		// If user hits EOF, replace it with a null character and return.
		if (c == EOF || c == '\n') 
		{
		  	buffer[position] = '\0';
		  	return buffer;
		} else 
		{
		  	buffer[position] = c;
		}
		
		position++;

		// If user has exceeded the buffer, reallocate.
		if (position >= buffersize) 
		{
			buffersize += LINE_BUFSIZE;
		  	buffer = realloc(buffer, buffersize);
		  	if (!buffer) 
		  	{
				fprintf(stderr, ">> allocation error\n");
				exit(EXIT_FAILURE);
		  	}
		}
	}
}

char **parser(char *line)
{
	/*
	// Parse user input line and return char**
	// parameter char *line 
	// e.g parser("ls -alh")
	*/
	
	int bufsize = SWITCH_BUFSIZE;
	int position = 0;
	char **switches = malloc(bufsize * sizeof(char*));
	char *aswitch;

	if (!switches) 
	{
		fprintf(stderr, ">> allocation error\n");
		exit(EXIT_FAILURE);
	}

	aswitch = strtok(line, SEPERATOR);
	
	while (aswitch != NULL) 
	{
		switches[position] = aswitch;
		position++;

		if (position >= bufsize) 
		{
			bufsize += SWITCH_BUFSIZE;
		  	switches = realloc(switches, bufsize * sizeof(char*));
		  	if (!switches) 
		  	{
				fprintf(stderr, ">> allocation error\n");
				exit(EXIT_FAILURE);
		  	}
		}

		aswitch = strtok(NULL, SEPERATOR);
	}
	
	switches[position] = NULL;
	return switches;

}

int backgroundJob(char** arg_list)
{
	/*
	// Function execute the program in background and return 1 if succes.
	// e.g backgroundJob(["ls","-alh"])
	*/
	
	pid_t child_pid; 
	
	// Duplicate this process.  
	child_pid = fork (); 
	
	if (child_pid != 0) 
		// This is the parent process.  
		return child_pid; 
	else 
	{
		// Now execute PROGRAM, searching for it in the path.  
		execvp (arg_list[0],  arg_list); 
		// The execvp  function returns only if an error occurs. 
		fprintf (stderr,  "an error occurred in execvp\n"); 
	} 
	return 1;
}

int runProgram(char **args)
{
	/*
	//Function execute the program and return 1 if succes.
	//e.g runProgram(["ls","-alh"])
	*/
	
	pid_t pid; 
	pid_t wpid;
	int status;

	pid = fork();
	if (pid == 0) 
	{
		// Child process
		if (execvp(args[0], args) == -1) 
		{
		  	perror("Error when executing process!");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) 
	{
		// Error forking
		perror("Forking error occured!");
	} else 
	{
		// Parent process
		do 
		{
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}
int execute(char **args)
{
	/*
	// Function decide the execution process of command. Background, system call, built-in vs. 
	// e.g execute(["ls","-alh"])
	*/
	
	int bg;
	
	if (args[0] == NULL) // An empty command 
	{
    	return 1;
  	}
  	else if(strcmp(args[0], "cd") == 0)
  	{
  		// Built-in function 'cd'
  		
  		if (args[1] == NULL) 
  		{
    		fprintf(stderr, "You must declare the directory for \"cd\"\n");
  		} else {
    		if (chdir(args[1]) != 0) 
    		{
      			fprintf(stderr,"Something went wrong!!");
    		}
  		}
  		return 1;
  	}
  	else if(strcmp(args[0], "author") == 0)
  	{
  		// Built-in function 'author'
  		
  		fprintf(stdout,"\t---Testa v1.0---\n**Harun GULEC**\n**Muhammed Ali BURSALİ**\n");
  	}
  	else if(strcmp(args[0], "exit") == 0)
  	{
  		// Built-in function 'exit'
  		// Save history to file and quit
  		int k;
  		
  		p=fopen(historyFilename,"w");
		for(k=0;k<20;k++)
		{
			fprintf(p,"%s",history[k]);
		}
	   	fclose(p);
  		
  		return 0;
  	}else{
  		
  		// If it is not a built-in function call system function.
  	
  		bg=backgroundCheck(args);
  		if(bg)
  		{
  			args[bg]=NULL;
  			backgroundJob(args);
  		}else{
  			runProgram(args);
  		}
  	}
  	
}

int backgroundCheck(char **args)
{ 
	/*
	// Check the arguments have '&' or not to run in background 
	*/
	
	int i=0;
	while(args[i]!=NULL)
	{
		i++;
	}
	if (strcmp(args[i-1],"&")==0)
	{
		return i-1;
	}else{
		return 0;
	}	
}

int historyRead()
{
	/*
	// Read history file into memory
	*/
	
	char *user=getenv("USER");
	char ch;
	int j=0;
	
	strcat(historyFilename,"/home/");
	strcat(historyFilename,user);
	strcat(historyFilename,"/testa_history");
	
	if( access( historyFilename, F_OK ) != -1 ) {
    	// file exists
    	p = fopen(historyFilename, "r+");
	} else {
    	// file doesn't exist
		p = fopen(historyFilename, "w");
		fclose(p);
		return 0;		
	}
	
	ch = fgetc(p);
	while(ch != EOF)
	{   	
    	history[counter][j]=ch;
    	j++;
    	if(ch == '\n')
    	{	
          	counter++;
          	j=0;
        }
        ch = fgetc(p);
	} 
	fclose(p);
	return 0;
}
void historyWrite(char *line)
{
	/*
	// Write command to history
	*/
	
	int k;
	strcat(line,"\n");
	
	if(line[0]=='\n')
	{
		return;
	}
	
	if (counter>=20)
	{
		for(k=0;k<19;k++)
		{
			strcpy(history[k],history[k+1]);
		} 
		strcpy(history[19],line);	
	}else{
		strcpy(history[counter],line);
	}
}
