#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>

//+
// File:	shell.c
//
// Pupose:	This program implements a simple shell program. It does not start
//		processes at this point in time. However, it will change directory
//		and list the contents of the current directory.
//
//		The commands are:
//		   cd name -> change to directory name, print an error if the directory doesn't exist.
//		              If there is no parameter, then change to the home directory.
//		   ls -> list the entries in the current directory.
//			      If no arguments, then ignores entries starting with .
//			      If -a then all entries
//		   pwd -> print the current directory.
//		   exit -> exit the shell (default exit value 0)
//				any argument must be numeric and is the exit value
//
//		if the command is not recognized an error is printed.
//-

#define CMD_BUFFSIZE 1024
#define MAXARGS 10

int splitCommandLine(char * commandBuffer, char* args[], int maxargs);
int doInternalCommand(char * args[], int nargs);
int doProgram(char * args[], int nargs);

//+
// Function:	main
//
// Purpose:	The main function. Contains the read
//		eval print loop for the shell.
//
// Parameters:	(none)
//
// Returns:	integer (exit status of shell)
//-

int main() {

    char commandBuffer[CMD_BUFFSIZE];
    // note the plus one, allows for an extra null
    char *args[MAXARGS+1];

    // print prompt.. fflush is needed because
    // stdout is line buffered, and won't
    // write to terminal until newline
    printf("%%> ");
    fflush(stdout);

    while(fgets(commandBuffer,CMD_BUFFSIZE,stdin) != NULL){
        //printf("%s",commandBuffer);

	// remove newline at end of buffer
	int cmdLen = strlen(commandBuffer);
	if (commandBuffer[cmdLen-1] == '\n'){
	    commandBuffer[cmdLen-1] = '\0';
	    cmdLen--;
            //printf("<%s>\n",commandBuffer);
	}

	// split command line into words.(Step 2)
    int nargs = splitCommandLine(commandBuffer, args, MAXARGS);

	// TODO

	// add a null to end of array (Step 2)
    args[nargs]='\0';

	// TODO

	// debugging
	//printf("%d\n", nargs);
	//int i;
	//for (i = 0; i < nargs; i++){
	//   printf("%d: %s\n",i,args[i]);
	//}
	//element just past nargs
	printf("%d: %x\n",i, args[i]);

        // TODO: check if 1 or more args (Step 3)
        // TODO: if one or more args, call doInternalCommand  (Step 3)

        if (nargs != 0){
            doInternalCommand(args, nargs);
        }

        // TODO: if doInternalCommand returns 0, call doProgram  (Step 4)
        // TODO: if doProgram returns 0, print error message (Step 3 & 4)
       
        if (doInternalCommand(args, nargs) == 0){
            if (doProgram(args, nargs) == 0){
                printf("Error, doInternalCommand and doProgram returned a 0 value");
            }
        }
        
        // that the command was not found.

	// print prompt
	printf("%%> ");
	fflush(stdout);
    }
    return 0;
}

////////////////////////////// String Handling (Step 1) ///////////////////////////////////

//+
// Function:	skipChar
//
// Purpose:	TODO: finish description of function
//
// Parameters:
//    charPtr	Pointer to string
//    skip	character to skip
//
// Returns:	Pointer to first character after skipped chars
//		ID function if the string doesn't start with skip,
//		or skip is the null character, it returns the pointer it was given 
//      unchanged
//-

char * skipChar(char * charPtr, char skip){
    // TODO: contents of function
    if (skip == '\0'){
        return charPtr;
    }

    while(*charPtr == skip){
        charPtr++;
    }
    return charPtr;
}

//+
// Funtion:	splitCommandLine
//
// Purpose:	TODO: This function takes as arguments, the commandBuffer,
//                the args array and the length of args array. After finding
//                the words and storing pointers to the beginning of each word
//                it returns the number of words(i.e. the number of pointers 
//                in the args array).
//
// Parameters:
//	TODO: parametrs and purpose
//
// Returns:	Number of arguments (< maxargs).
//
//-

int splitCommandLine(char * commandBuffer, char* args[], int maxargs){
   // TODO: Contents of function
   int argsCount = 0;
   char *firstChar = commandBuffer;
   
   while (*firstChar != '\0'){
    firstChar = skipChar(firstChar,' ');
        if (argsCount !=  maxargs) {
            args[argsCount] = firstChar;
            argsCount++;
         }

        else {
            fprintf(stderr, "The amount of args (%d) has been reached \n", maxargs);
        }

        while (*firstChar != ' ') {
            if (*firstChar == '\0') return argsCount;
			firstChar++;
        }

        *firstChar = '\0';
        firstChar++; 
   }

   return argsCount;
   // TODO: reutrn proper value
   return 0;
}
////////////////////////////// External Program  (Note this is step 4, complete doeInternalCommand first!!) //////////////////////////////////
// list of directorys to check for command.
// terminated by null value
char * path[] = {
    ".",
    "/usr/bin",
    NULL
};

//+
// Funtion:	doProgram
//
// Purpose:	TODO: The first part is a loop that loops through each
//                element of the path array, it then uses strlen to find the length
//                if the current element of the path and the length of the command.
//                Next malloc is used to allocate enough space for the path,
//                a '/' character and the command being used.
//
// Parameters:
//	TODO: add paramters and description
//
// Returns	int
//		1 = found and executed the file
//		0 = could not find and execute the file
//-

int doProgram(char * args[], int nargs){
  // find the executable
  // TODO: add body.

  int CurrentPath, Length = 0;
  char *CommandPath = NULL;
  
  for(int i = 1; i<nargs; i++){
    CurrentPath = strlen(*args[i]);
    Length = strlen(*args[0]);
    CommandPath = malloc(CurrentPath + 2 + Length);   

    sprintf(CommandPath, "%s/%s", args[i], args[0]);
    struct stat stats;

    if(stat(CommandPath, &stats) == 0){
        if(!(S_ISREG(stats.st_mode))){
            printf("Error from S_ISREG, irregular");
            return 0;
        }
        if (!(S_IXUSR & stats.st_mode)){
            printf("Error from S_IXUSR, unexecutable");
            return 0;
        }
        break;
    }
    free(CommandPath);
    CommandPath = NULL;
  }

  if (CommandPath == NULL) return 0;

  pid_t outFork = fork();

  // child could not be crewated
  if(outFork == -1) perror("Child could not be created");

  //is a child
  if(outFork == 0) execv(&CommandPath, args);

  free(CommandPath);
  wait(NULL);

  return 1;
}
  // Note this is step 4, complete doInternalCommand first!!!

////////////////////////////// Internal Command Handling (Step 3) ///////////////////////////////////

// define command handling function pointer type
typedef void(*commandFunc)(char * args[], int nargs);

// associate a command name with a command handling function
struct cmdStruct{
   char 	* cmdName;
   commandFunc 	cmdFunc;
};

// prototypes for command handling functions
// TODO: add prototype for each comamand function

void exit_function(char * args[], int nargs);
void pwd_function(char * args[], int nargs);
void ls_function(char * args[], int nargs);
int cd_function(char * args[], int nargs);

// list commands and functions
// must be terminated by {NULL, NULL} 
// in a real shell, this would be a hashtable.
struct cmdStruct commands[] = {
   // TODO: add entry for each command

   {"exit", exit_function},
   {"pwd", pwd_function},
   {"ls", ls_function},
   {"cd", cd_function},
   { NULL, NULL}	// terminator

};

//+
// Function:	doInternalCommand
//
// Purpose:	TODO: If the number of arguments returned in the previous step is not zero,
//                the dointernalcommmand is called with the argumnet array.
//
// Parameters:
//	TODO: add parameter names and descriptions
//
// Returns	int
//		1 = args[0] is an internal command
//		0 = args[0] is not an internal command
//-

int doInternalCommand(char * args[], int nargs){
    // TODO: function contents (step 3)
    int i = 0;
    while(commands[i].cmdName != NULL){
        if (strcmp(commands[i].cmdName, args[0]) == 0){
            (*commands[i].cmdFunc)(args,nargs);
            return 1;
        }
        i++;
    }
    return 0;
}

///////////////////////////////
// comand Handling Functions //
///////////////////////////////

int CheckFilter(const struct dirent *d){
    if (d->d_name[0] == '.') return 0;
    else return 1;
}

// TODO: a function for each command handling function
// goes here. Also make sure a comment block prefaces
// each of the command handling functions.


//Command Handling function for the internal command pdw
void pwd_function(char * args[], int nargs) {
    char *cwd = getcwd(NULL, 0); 
    fprintf("Current Working Directory is %s\n", cwd);
    free(cwd);
}

//Command Handling function for the internal command exit
void exit_function(char * args[], int nargs) {
    exit(0);
}

//Command Handling function for the internal command ls
void ls_function(char * args[], int nargs) {
    struct dirent **namelist;
    int numEnts = scandir(".", &namelist, NULL, NULL);
    int (*filterFunc)(const struct dirent *d) = NULL;

    if (nargs == 1){
        int numEnts = scandir(".", &namelist, filterFunc, NULL);
        for (int i = 0; i<numEnts; i++){
            if (CheckFilter(namelist[i]) == 1){
                printf("%s\n", namelist[i]->d_name); 
            }
        }
    }  
    else if (nargs == 2){
        if (strcmp(args[1], '-a') == 0){
            int numEnts = scandir(".", &namelist, NULL, NULL);
            for (int i = 0; i<numEnts; i++){
                printf("%s\n", namelist[i]->d_name);
            }
        }

        else{
            fprintf("Error, no -a", NULL);
        }
    }
    return;
}

//Command Handling function for the internal command cd
int cd_function(char * args[], int nargs) {
    struct passwd *pw = getpwuid(getuid());
    char *directory;
    if (pw == NULL){
        fprintf("The password is empty", NULL);
        return 0;
    }

    if (nargs == 1){
        //This is the home directory
        chdir(pw->pw_dir);
    }

    else{
        if(chdir(directory) != 0){
            fprintf("The directory could not be changed", NULL);
            return -1;
        }
        return 0;
    }
}
