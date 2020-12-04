#include <stdio.h>
#include <stdlib.h>

#define MOD(a,b) ((((a)%(b))+(b))%(b));

struct Object{
        FILE *fp;
        FILE *keyFile;
        int fileLength;
        int keyLength;
        char *enc;
        char *key;
};

int main(int argc, char *argv[]){
        struct Object *obj;
        obj = malloc(sizeof(struct Object));

        int ch;
        int count = 0;

        obj->fp = fopen(argv[1], "r");
        while((ch = fgetc(obj->fp)) != EOF){
                count++;
        }
        fclose(obj->fp);
        obj->fileLength = count;
        count = 0;

        obj->keyFile = fopen(argv[2], "r");
        while((ch = fgetc(obj->keyFile)) != EOF){
                count++;
        }
        fclose(obj->keyFile);
        obj->keyLength = count;
        printf("FileLength = %d\nkeyLength = %d\n", obj->fileLength, obj->keyLength);

        if(obj->fileLength > obj->keyLength){
                perror("Key shorter than file\n");
                exit(1);
        }

        obj->enc = (char*)malloc(obj->fileLength);
        obj->key = (char*)malloc(obj->keyLength);

        obj->fp = fopen(argv[1], "r");
        fgets(obj->enc, obj->fileLength, obj->fp);
        fclose(obj->fp);

        obj->keyFile = fopen(argv[2], "r");
        fgets(obj->key, obj->keyLength, obj->keyFile);
        fclose(obj->keyFile);

        int temp;
        for(count = 0; count < obj->fileLength - 1; count++){
                temp = obj->enc[count];
                if(obj->enc[count] == ' '){
                        obj->enc[count] = '[';
                }
                printf("%d ", temp);
                if(temp < 65 || temp > 91){
                        if(temp != 32){
                                perror("Bad input\n");
                                exit(1);
                        }
                }
        }
        printf("\n");

        printf("File:\n%s\nKey:\n%s\n-------------------------", obj->enc, obj->key);
        for(count = 0; count < obj->fileLength - 1; count++){
                obj->enc[count] -= 65;
                temp = (int)obj->enc[count] + (int)obj->key[count];
                temp = temp % 27;
                obj->enc[count] = (char)(temp + 65);
        }
        printf("File:\n%s\nKey:\n%s\n-------------------------", obj->enc, obj->key);

        for(count = 0; count < obj->fileLength - 1; count++){
                obj->enc[count] -= 65;
                temp = (int)obj->enc[count] - (int)obj->key[count];
                temp = MOD(temp, 27);
                obj->enc[count] = (char)(temp + 65);
        }

        for(count = 0; count < obj->fileLength; count++){
                if(obj->enc[count] == '['){
                        obj->enc[count] = ' ';
                }
        }
        printf("File:\n%s\nKey:\n%s\n-------------------------", obj->enc, obj->key);

        return 0;
}
