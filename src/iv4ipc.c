#include "iv4ipc.h"
/* Returns the maximal shared memory size on sucess and 0 on errors */
size_t get_max_shm(void)
{
    FILE*fp;
    size_t v;
    v = 0;
    fp = fopen("/proc/sys/kernel/shmmax","r");
    if (fp){
        //FIXME use a safer function
        fscanf(fp,"%ld",(long int*)&v);    
        fclose(fp);
    }else{
        //TODO propagate errors 
        //fprintf(stderr,"Cannot open /proc/sys/kernel/shmmax cause=%s\n",
        //        strerror(errno));
    }
    return v;
}

int init_shm(char* idfile)
{
    int hnd;
    FILE* fp;
    size_t s;
    s = SPACE_SIZE;
    if (s > get_max_shm()) {
        //FIXME use proper error handling
        //fprintf(stderr,"Your system does not support shared memory segments of %ld bytes\n",(long int)s);
        //return EXIT_FAILURE;
    }   
    printf("s=%ld\n",(long int)s); 
    hnd = shmget(IPC_PRIVATE, s, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (hnd > 0) {
        fp = fopen(idfile,"w");
        if (fp) {
            fprintf(fp,"%d",hnd);
            printf("[INFO] Shared memory segment created and the identifier is stored in %s\n",
                    idfile);
            return EXIT_SUCCESS;
        }else{
            //FIXME use proper error handling
            //fprintf(stderr,"Failed to store shared memory segment id in %s, cause=%s\n",
            //               idfile,strerror(errno));
            /* Destroy the shared memory segment */
            //if (shmctl(hnd, IPC_RMID, NULL)){
            //    fprintf(stderr,"Could not destroy shared memory segment %x\n",hnd);
            //}
        }
    }else{
        //fprintf(stderr,"Could not create shared memory segment %s\n",strerror(errno));
    }
    return EXIT_FAILURE;
}
