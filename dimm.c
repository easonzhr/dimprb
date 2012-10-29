#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define MEM_DEVICE "/dev/mem"
#define MAP_FAILED ((void *) -1)

void *mem_chunk(size_t base, size_t len, const char *devmem)
{
	int fd;
	void * buf;
	size_t mmoffset;
	void * mmp;
	

	if((fd = open(devmem, O_RDONLY)) == -1){
		printf("could not open /dev/mem\n");
		return NULL;
	}

	if ((buf = malloc(len)) == NULL){
                printf("malloc failure\n");
                return NULL;
        }

 	mmoffset = base % getpagesize();
        mmp = mmap(0, mmoffset + len, PROT_READ, MAP_SHARED, fd, base - mmoffset);       	
        if(mmp == MAP_FAILED){
		printf("mmap fialure\n");
		return NULL;
	}

	memcpy(buf, (char *)mmp + mmoffset, len);
        
        if (munmap(mmp, mmoffset + len) == -1){
                printf("munmap failure\n");
		return NULL;
        }

	if(close(fd) == -1){
		printf("could not close /dev/mem\n");
	}
	return buf;
}

int main(int argc, char * const argv[])
{
	int fp;
	int ret = 0;
	void * buf;
	size_t len  = 0x10000;
	size_t base = 0xF0000;

	buf = mem_chunk(base, len, MEM_DEVICE);

	for(fp = 0; fp <= 0xFFF0; fp += 16 ){
		if(memcmp(buf + fp, "_SM_" , 4) == 0 && fp <= 0xFFE0){
			printf("_SM_\n");
			return 0;
		}else if(memcmp(buf + fp, "_DMI_", 5) == 0){
			printf("_DMI_\n");
			return 0;
		}
	}
	

	printf("ok\n");
	
	
	
	return ret;
}
