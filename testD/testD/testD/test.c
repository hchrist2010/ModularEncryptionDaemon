#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
        int arg1 = atoi(argv[1]);
        int i;
        srand(time(0));
        char *key = (char*)malloc(sizeof(arg1 + 1));
        char buffer[16];
        char *ch;
        int count = 0;
        for(i = 0; i < arg1; i++){
                key[i] = ((rand() % 26) + 65);
        }
        printf("Key\n%s\n", key);
        printf("Strlen(key) = %d\n", strlen(key));
        while(count < strlen(key)){
                for(i = 0; i < 16; i++){
                        buffer[i] = key[i + count];
                }
                printf("%s\n", buffer);
                count += 16;
                memset(buffer, '\0', strlen(buffer));
        }

        return 0;
}
