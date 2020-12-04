#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

struct Obj{
        char *key;
        char *plainText;
        char *encripted;
        FILE *keyFile;
        FILE *plainTextFile;
        int size;
};

void initialize(struct Obj *obj){
        obj->key = NULL;
        obj->plainText = NULL;
        obj->keyFile = NULL;
        obj->plainTextFile = NULL;
        obj->size = 0;
}

void getSize(struct Obj *obj, char *argv[]){
        char c;
        obj->plainTextFile = fopen(argv[1], "r");
        while(1){
                c = fgetc(obj->plainTextFile);
                if(c == EOF){
                        break;
                }
                else{
                        obj->size++;
                }
        }
        fclose(obj->plainTextFile);
        obj->plainTextFile = NULL;
}

void getInFile(struct Obj *obj, char *argv[]){
        obj->plainTextFile = fopen(argv[1], "r");

        obj->plainText = (char *)malloc(obj->size + 1);

        fgets(obj->plainText, obj->size, obj->plainTextFile);
        fclose(obj->plainTextFile);
        obj->plainTextFile = NULL;
}

void display(struct Obj *obj){
        printf("\n--Display--\n");
        printf("Key: %s\n", obj->key);
        printf("plainText: %s\n", obj->plainText);
        printf("keyFile: %s\n", obj->keyFile);
        printf("PlainTextFile: %s\n", obj->plainTextFile);
        printf("Size: %d\n\n", obj->size);
}

void createKey(struct Obj *obj, char *argv[]){
        int pid;
        printf("1\n");
        pid = fork();
        if(fork == 0){
                printf("here\n");

		char *args[3];
		args[0] = malloc(sizeof("keygen") + 1);
		strcpy(args[0], "keygen");
		args[1] = malloc(64);
		sprintf(args[1], "%d", obj->size);
		args[2] = NULL;
                printf("%s %s %s\n", args[0], args[1], args[2]);

                int a;
		execvp(args[0], args);
                if(a < 0){
                        printf("Failed ot exec\n");
                        exit(1);
                }
        }
        else{
                wait(NULL);
        }
}

void encript(struct Obj *obj, char *argv[]){
        obj->keyFile = fopen("mykey.txt", "r");
        obj->key = (char*)(malloc(obj->size + 1));
        fgets(obj->key, obj->size, obj->keyFile);
        fclose(obj->keyFile);
        obj->keyFile = NULL;

        obj->encripted = (char *)malloc(sizeof(obj->size));
        int i, x, y, z;
        printf("Encryption\n");
        for(i = 0; i < obj->size - 1; i++){
                x = (int)(obj->plainText[i]);
                y = (int)(obj->key[i]);
                z = x + y;
                z = z % 27;
                obj->encripted[i] = z + '0';
                printf("%c    %c     %d       %c\n", obj->plainText[i], obj->key[i], z, obj->encripted[i]);

        }

        printf("Decrypt\n");
        for(i = 0; i < obj->size - 1; i++){
                x = (int)obj->encripted[i];
                y = (int)obj->key[i];
                z = x - y;
                z = x % 27;
                obj->encripted[i] = z + '0';
                printf("%c    %c     %d       %c\n", obj->plainText[i], obj->key[i], z, obj->encripted[i]);
        }
}

int main(int argc, char *argv[]){
        struct Obj *obj;
        obj = malloc(sizeof(struct Obj));
        initialize(obj);
        getSize(obj, argv);
        getInFile(obj, argv);
//        createKey(obj, argv);
        encript(obj, argv);

        display(obj);


        return 0;
}
