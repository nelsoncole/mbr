#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char *argv[]) 
{

	FILE *fp, *fp1;

	char *stage0 = (char *)malloc(512); //  512


	// Open fp
	if((fp = fopen(argv[1],"r+b")) == NULL) {
		printf("Erro ao abrir o arquivo \"argv[1]\"\n");
		exit(1); 
	}


	if( ( fread(stage0,1,512,fp) ) != 512) {
		printf("Read  stage0 Error\n");
		exit(1);	
	}


	// Open fp1
	if((fp1 = fopen(argv[2],"r+b")) == NULL) {
		printf("Erro ao abrir o arquivo \"argv[2]\"\n");
		exit(1); 
	}

	//Escrevendo o sector de boot
	if(( fwrite(stage0,1,0x1B8,fp1) ) != 0x1B8) {
		printf("Erro ao escrever o sector de boot");
		exit(1);	
	}

	free(stage0);
	fclose(fp1);
	fclose(fp);

	printf("done!\n");

	exit(0);
	return 0;

}
