#include <stdio.h>
#include <stdlib.h>

int main(){
        FILE *fp;
        char buffer[256];

        fp = fopen("plaintext4.msi", "r");

        while(fgets(buffer, 256, fp) != NULL){
                printf("%s", buffer);
        }

        fclose(fp);

}
