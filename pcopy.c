#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>



#define LBA_START 1


int main(int argc, char *argv[]) 
{

	FILE *fp, *fp1, *fp2;

	char *stage0 = (char *)malloc(512); //  512
	char *bpb = (char *)malloc(512); //  512
	char *data = (char *)malloc(0x1000); // 4 KiB

	struct stat st;
	int part_size = 0;


	// Open fp
	if((fp = fopen(argv[1],"r+b")) == NULL) {
		printf("Erro ao abrir o arquivo \"argv[1]\"\n");
		exit(1); 
	}


	if( ( fread(stage0,1,512,fp) ) != 512) {
		printf("Read  stage0 Error\n");
		exit(1);	
	}


	// Open fp1 vfat
	if((fp1 = fopen(argv[2],"r+b")) == NULL) {
		printf("Erro ao abrir o arquivo \"argv[2]\"\n");
		exit(1); 
	}

	if( ( fread(bpb,1,512,fp1) ) != 512) {
		printf("Read  BPB error\n");
		exit(1);	
	}

	*(int*)(bpb +28) =  LBA_START;

	rewind(fp1);

	printf("Escrevendo a FAT BPB 512 Bytes\n");
	if(( fwrite(bpb,1,512,fp1) ) != 512) {
		printf("Erro ao escrever a FAT BPB\n");
		exit(1);	
	}


	// Unique Disk ID:
	*(int*)(stage0 + 0x1B8) = 0x12345678;
	// First partition table entry:

	// Drive attributes (bit 7 set = active or bootable)
	*(char*)(stage0 + 0x1BE + 0x0) = 0x80;
	
	// CHS Address of partition start
	*(int*)(stage0 + 0x1BE + 0x1) = 0;
	
	// Partition type
	*(char*)(stage0 + 0x1BE + 0x4) = 0;
	
	// CHS address of last partition sector
	*(int*)(stage0 + 0x1BE + 0x5) = 0;
	
	// LBA of partition start
	*(int*)(stage0 + 0x1BE + 0x8) = LBA_START;
	 

	// Number of sectors in partition
	if( *(short*)(bpb +19) ) *(int*)(stage0 + 0x1BE + 0xc) = *(short*)(bpb +19); // BPB_TotSec16
	else *(int*)(stage0 + 0x1BE + 0xc) = *(int*)(bpb +32); // BPB_TotSec32


	// Open fp2  vhd
	if((fp2 = fopen(argv[3],"r+b")) == NULL) {
		printf("Erro ao abrir o arquivo \"argv[3]\"\n");
		exit(1); 
	}

	printf("Escrevendo o sector de boot 512 Bytes\n");
	if(( fwrite(stage0,1,0x200,fp2) ) != 0x200) {
		printf("Erro ao escrever o sector de boot\n");
		exit(1);	
	}



	
	stat (argv[2],&st);
    	part_size = st.st_size;

	stat (argv[3],&st);

	if(st.st_size <= part_size ) {

		printf("Erro, o VHD <= Part.img\n");
		exit(1);


	}

	printf("Escreve a Partiçao\n");

	rewind(fp1);
	rewind(fp2);

	fseek(fp2, 512*LBA_START,SEEK_SET);

	int i;
	for(i = 0; i < (part_size/0x1000);i++) {

	if( ( fread(data,1,0x1000,fp1) ) != 0x1000) {
		printf("Erro ao copiar Particao\n");
		exit(1);	
	}

	if(( fwrite(data,1,0x1000,fp2) ) != 0x1000) {
		printf("Erro ao escrever a Particao\n");
		exit(1);	
	}

	fseek(fp1, 0x1000*i,SEEK_SET);
	fseek(fp2, 0x1000*i + 512,SEEK_SET);

	}

	






	free(data);
	free(bpb);
	free(stage0);
	fclose(fp2);
	fclose(fp1);
	fclose(fp);

	printf("done!\n");

	exit(0);
	return 0;

}
