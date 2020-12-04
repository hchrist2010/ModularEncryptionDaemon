//Name: Hunter Christiansen
//Email: christhu@oregonstate.edu
//UserID: 932211550
//Assignment4 Keygen

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
	int arg1 = atoi(argv[1]);			//store the size of the key as an int
	int i;
	srand(time(0));					//seed the rand function

	char *key = (char *)malloc(arg1 + 1);		//allocate space for the key
	for(i = 0; i < arg1; i++){
		key[i] = (rand() % 27) + 65;		//generate random letter for each index of key
		if(key[i] == '['){
			key[i] = ' ';			//if the key generates a '[', then replace it with a space
		}
	}
	key[arg1] = '\0';				//end key with a null terminater
	printf("%s\n", key);				//print key
	free(key);					//free the memory used for the key
	return 0;
}
