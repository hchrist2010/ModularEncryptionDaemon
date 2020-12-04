#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	char *inputText;
	char *keyText;
	int size;
	FILE *plainTextFile;
	FILE *keyFile;
	char c;
	int i;

	size = 0;
	plainTextFile = fopen(argv[1], "r");
	while(1){
		c = fgetc(plainTextFile);
		if(c == EOF){
			break;
		}
		else{
			size++;
		}
	}
	close(plainTextFile);
	printf("%d\n", size);
	plainTextFile = NULL;
	plainTextFile = fopen(argv[1], "r");

	inputText = (char *)malloc(size + 1);

	fgets(inputText, size, plainTextFile);

	int pid = fork();
	if(fork == 0){
		char* args[3];
		args[0] = {"keygen"};
		if(size <= 256){
			args[1] = size;
		}
		else{
			args[1] = 256;
		}
		args[2] = NULL;

		execvp(args[0], args);
	}
	else{
		wait(NULL);
	}

	printf("%s\n", inputText);
	
}
