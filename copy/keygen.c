#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
	int arg1 = atoi(argv[1]);
	int i;
	srand(time(0));

	char *key = (char *)malloc(arg1);
	for(i = 0; i < arg1; i++){
		key[i] = (rand() % 27) + 65;
	}
	FILE *fp;
	fp = fopen("mykey.txt", "w+");
	fprintf(fp, "%s", key);
	fclose(fp);


//	printf("%s\n", key);

	return 0;
}
