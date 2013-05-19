/*
 *   Test to traverse an nfcapd file and put the IPv4 addresses in a bitindex
 *
 *   Copyright (C) 2013  Gerard Wagener
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Open points
 * TODO Generation timestamp 
 * TODO write merge functions,diff
 * TODO implement append mode in file mode
 * TODO extend shm support: metadata, flush, delete
 * TODO export bitindex as list of dotted decimal notation
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>
#include <zlib.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <libnfdump/libnfdump.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "ipv4index.h"             
#include "iv4file.h"
#include "iv4ipc.h"
#include "nf-reader.h"
/*
 * Initialize a new bitindex.
 * The number of bits is specified with the parameter nelem.
 * Returns a pointer to the an ipv4index structure on success.
 * The bitindex itself is in ipv4index->bitindex which also initialized
 * Returns NULL when no memory is available.
 * The memory should be freed when it is not used.
 */
ipv4index_t* bitindex_new(uint32_t nelem, int flags)
{
    ipv4index_t* self;
    self = calloc(sizeof(ipv4index_t),1);
    if (self) {
        if (flags) {
            self->bitindex = calloc((nelem / 8) + 1 , 1);
            if (!self->bitindex)
                return NULL;
        }
        return self; 
    }
    /* Somewhere no memory is available */
    return NULL;
}


/* Sets a bit related to an IPV4 address defined in the parameter addr. The 
 * bitset bs is updated. If a lot of such INSERT operations are done, this
 * function should not be used because for each operation a stackframe is 
 * build. Therefore the macro BITINDEX_SET should be used
 */ 
uint8_t bit_index_set(uint8_t* bs, uint32_t addr)
{
    uint32_t cell;
    uint32_t x;
    uint8_t p;
    cell = addr>>3;
    x = (addr>>3)<<3;
    p = addr-x; 
    //DEBUG printf("cell %d, x %d,p %d\n",cell, x, p);
    return bs[cell] |= 1 << p;
}

/* Test if an IPv4 address addr is known in the bitset bs.
 * Returns 0 if the IP address is not known
 * Returns a positive number if the IP address is known
 */ 
uint8_t test_bit(uint8_t* bs, uint32_t addr)
{
    return bs[addr>>3] & (1 << (addr-((addr>>3)<<3)));    
} 

/* Dumps a bit index (bitindex) on standard output for debugging */
void dump(uint8_t* bitindex)
{   
    int i;
    printf("--- BEGIN ---\n");
    for (i=0;i<SPACE / 8;i++) {
        printf("%d %d\n", i, bitindex[i]);
    }
    printf("--- END ---\n");
}


void usage(void)
{
    printf("nf-bitindex - Put IPv4 addresses extracted from nfcapd files in a bitindex\n");
    printf("\n");
    printf("nf-bitindex [-h] [-b -w <filename>] [ -q -r <filename> ] [ -c filename]\n");
    printf("[-b -a=id] [-q -a=id] [-z id]\n");
    printf("OPTIONS\n");
    printf("\n");
    printf("    -h --help   Shows this screen\n");
    printf("    -b --batch  Read nfcapd files from standard input that are indexed\n");
    printf("    -w --write  Specify the filename where the bitindex is stored\n");    
    printf("    -s --source Specify a source to identify the netflow records\n");
    printf("    -q --query  Query if the ip addresses read through standard input are \n");
    printf("                in the bitindex. The result is outputed on standard output. \n");
    printf("    -r --read   Read a gzip compressed bitindex\n");
    printf("    -c --create Creates a shared memory segment for storing the bitindex.\n");
    printf("                The shared memory identifier is stored in the specified filename\n");
    printf("    -a --attach=id Attaches to a shared memory segment\n");
    printf("    -z --zero=id In case a shared memory segment is used, the bitset can be\n");
    printf("              reset (all bits to 0) with this option. The shared memory segment\n");
    printf("              is identified with the id parameter in decimal\n");
    printf("\n");
    printf("EXAMPLE\n");
    printf("    Put all the nfcapd files from Septembre 2012 in a bitindex\n\n");
    printf("find . | grep \"nfcapd.201209\" | nf-bitindex -b -w september2012.ibi.gz -s router_1\n\n"); 
    printf("QUERY INPUT FORMAT\n\n");
    printf("IP addresses should be passed as list (delimited with a \\n) in dotted decimal notation\n\n");
    printf("QUERY OUTPUT FORMAT\n");
    printf("\n");
    printf("xxx.xxx.xxx.xxx source firstseen lastseen\n\n");
    printf("xxx.xxx.xxx.xxx is an IPv4 address\n");
    printf("source is the source that generated the netflow records\n");
    printf("firstseen indicates the oldest timestamp of *ANY* netflow record present in this index\n");
    printf("lastseen indicates the youngest timestamp of *ANY* netflow record presented in this index\n");
    printf("Hence, the right file must be still searched!\n\n");
    printf("AUTHOR\n");
    printf("    Gerard Wagener\n");
    printf("\n");
    printf("LICENSE\n");
    printf("    GNU Affero General Public License\n");
}

/* Read files from standard input and put them in the bitindex file  
 * identified with the targetfile parameter.
 * returns EXIT_SUCCESS on success and EXIT_FAILURE on errors.
 */
int batch_processing(char *source, char* targetfile, int segment_id)
{
    int i,r;
    char *filename;
    ipv4cache_hdr_t* hdr;
    ipv4index_t* ipv4index;
 
    r = EXIT_FAILURE; /* Return code */
    /* FIXME assume that the timezone of the netflow collector is the same
     * than the timezone configured on this machine
     */
    if (!(hdr = create_local_header(source)))
        goto out;
    
    /* Check if the data should be exported in shared memory segment */
    if (segment_id){
        if (!(ipv4index=bitindex_new(SPACE, BAREIPV4INDEX)))
            goto out;
        printf("[INFO] Trying to connect to segment_id %d\n", segment_id);
        if ((ipv4index->bitindex=(uint8_t*)shmat(segment_id, 0, SHM_RND)) < 0){
            printf("Failed to attach to shared memory segment id=%d cause=%s\n",
                    segment_id, strerror(errno));
            goto out;
        }else{
            printf("[INFO] Successfully connected to segment_id %d\n",segment_id);

        }
    }else{
        printf("[INFO] No shared memory used, use local memory\n");
        if (!(ipv4index = bitindex_new(SPACE, FULLIPV4INDEX)))
            goto out;
    }
    /* A bit index is needed here either shared or private */
    filename = calloc(1024,1);
    if (!filename)  
        goto out;
 
    while (fgets(filename, 1024, stdin)){
        filename[1023] = 0;
        /* remove new line */
        for (i=0; i<1024; i++){
            if (filename[i] == '\n'){
                filename[i] = 0;
                break;
            }
        }
        printf("[INFO] Processing %s\n",filename);
        if (!index_nfcapd_file(filename, ipv4index)){
            printf("[ERROR] Could not process %s\n",filename);
        }
    }
    if (targetfile) {
        printf("[INFO] Creating %s\n",targetfile);
        if (store_bitindex(ipv4index, targetfile, hdr)){
            printf("[INFO] Sucessfully created %s",targetfile);
            r = EXIT_SUCCESS;
        } else {
            printf("[ERROR] Could not store bitindex in file %s\n",targetfile);
            r = EXIT_FAILURE;
        }
    }else{
        /* When no file is used, is a shm used? */
        if (segment_id > 0)
            printf("[INFO] Sucessfully updated the shared memory segment_id=%d\n",
                   segment_id);
            r = EXIT_SUCCESS;
    }
out:
    if (hdr)
        free(hdr);
    //FIXME write a destructor
    if (!segment_id && ipv4index && ipv4index->bitindex)
        free(ipv4index->bitindex);
    if (!segment_id && ipv4index)
        free(ipv4index);
    if (filename)
        free(filename);
    /* Detach from shared memory segment if needed */
    if ((segment_id>0) && (shmdt(ipv4index->bitindex))<0){
        fprintf(stderr,"Failed to detach from segment_id %d, cause=%s\n",
                       segment_id, strerror(errno));
        r = EXIT_FAILURE;
    }
    return r;                          
}

int query_addr (char* sourcefile, int segment_id)
{
    char *istr;
    int i,r;
    uint32_t addr;
    ipv4index_t* ipv4index;

    r = EXIT_FAILURE;
    istr  = calloc(64,1);
    if (!istr)
        goto oret;
    
    if (!segment_id) {
        if (!(ipv4index = bitindex_new(SPACE, FULLIPV4INDEX)))
            goto oret;
        //fprintf(stderr,"[DEBUG] use local memory\n"); 
        if (!load_bitindex(ipv4index, sourcefile))
            goto oret;
    } else {
        if (!(ipv4index = bitindex_new(SPACE, BAREIPV4INDEX)))
            goto oret;
        //fprintf(stderr,"[DEBUG] use shared memory segment\n");
        if ((ipv4index->bitindex=(uint8_t*)shmat(segment_id, 0, SHM_RND)) < 0){
            printf("%p\n",ipv4index->bitindex);
            fprintf(stderr,"Failed to attach to segment_id %d cause=%s\n",
                    segment_id, strerror(errno));
            goto oret;
        }
    }
    assert(ipv4index->bitindex);
    while (fgets(istr, 64, stdin)){
        istr[63] = 0;
        /* Replace the new line */
        for (i=0; i<64;i++){
            if (istr[i] == '\n'){
                istr[i] = 0;
                break;
            }
        }
        addr = 0; 
        if (inet_pton(AF_INET, istr,&addr)){ 
            if (test_bit(ipv4index->bitindex, addr)){
                /* FIXME In shared memory segment no header is present, that
                 *should be done in the future. Otherwise metainformation are 
                 *not known
                 */
                if (!segment_id){
                    printf("%s %s %d %d\n",istr, ipv4index->header->source[0], 
                                      (uint32_t)ipv4index->header->firstseen.tv_sec, 
                                      (uint32_t)ipv4index->header->lastseen.tv_sec); 
                }else{
                    /* If the IP address is in the bitindex the ip address 
                     * is printed on stdout otherwise nothing is printed
                     */
                    printf("%s\n",istr);
                }
            }
        }else{
            fprintf(stderr,"The string %s is not a valid IP address\n",istr);
        }
    }
    /* Here every thing is assumed to be fine */
    r = EXIT_SUCCESS;
oret:
    if (istr)
        free(istr);
    //FIXME build destructor
    if (!segment_id && ipv4index &&ipv4index->bitindex)
        free(ipv4index->bitindex);
    /* Detach from shared memory segment if needed */
    if ((segment_id>0) && (shmdt(ipv4index->bitindex))<0){
        fprintf(stderr,"Failed to detach from segment_id %d, cause=%s\n",
                       segment_id, strerror(errno));
        r = EXIT_FAILURE;
    }
    //FIXME write a destructor for ipv4index instance
    if (ipv4index)
        free(ipv4index);    
    return r;
}

int main(int argc, char* argv[])
{
    int next_option = 0;
    const char* const short_options = "hw:bs:r:qc:a:z:";
    const struct option long_options[] = {
                { "help", 0, NULL, 'h' },
                { "batch", 0, NULL, 'b' },
                { "write", 1, NULL, 'w' },
                { "source",1, NULL, 'b' },
                { "query",0,NULL, 'b'},
                { "read",1,NULL,'q'},
                { "create",1,NULL,'q'},
                { "attach",1,NULL,'a'},
                { "zero",1,NULL,'z'},
                {NULL,0,NULL,0}};
    char* targetfile;
    char * source;
    char *sourcefile;
    char *shmidfile;
    int batch,query;
    int segment_id;
    int shouldzero;
    segment_id = 0;
    batch = 0;
    query = 0;
    targetfile = NULL;
    source = NULL;
    shmidfile = NULL;
    sourcefile = NULL;
    shouldzero =0;
    do {
        next_option = getopt_long (argc, argv, short_options, 
                                   long_options, NULL);
        if (next_option > 0) {
            switch(next_option)
            {
            case 'h':
                usage();
                return EXIT_SUCCESS;
            case 'b':
                batch = 1;
                break;
            case 'w':
                targetfile = optarg;
                break;
            case 's':
                source = optarg;
                break;
            case 'r':
                sourcefile = optarg;
                break;
            case 'q':
                query = 1;
                break;
            case 'c':
                shmidfile = optarg;
                break;
            case 'a':
                segment_id = atoi(optarg);
                break;
            case 'z':
                shouldzero = 1;
                segment_id = atoi(optarg);
                break;
            default:
                return EXIT_FAILURE;
            }
        }
    }while (next_option != -1);
    
    /* maintenance stuff */
    if (shouldzero)
        return reset_shm(segment_id);

    if (shmidfile) {
        return init_shm(shmidfile);    
    }
     
    /* test parameters */
    if (!batch) {
        if (!query) {
            printf("Nothing to do.\n");
            return EXIT_SUCCESS;
        }
    }
    if (query & batch) {
        fprintf(stderr,"Batch processing and query mode are mutal exclusive.\n");
        return EXIT_FAILURE;
    }
    if ((!segment_id) & batch & (!targetfile)){
        fprintf(stderr, "A target file has to be specified with the -w option\n");
        return EXIT_FAILURE;
    }

    if (batch & (!source)) {
        fprintf(stderr,"A source must be specified with the -s option\n");
        return EXIT_FAILURE;
    }

    if (query & (segment_id==0) & (!sourcefile) ){
        fprintf(stderr,"In query mode a shared memory segment or a source file");
        fprintf(stderr," must be specified.\n");
        return EXIT_FAILURE;
    }
    if (query & (!sourcefile) & (segment_id == 0)) {
        fprintf(stderr, "In query mode, a source file must be specified with the -r option\n");
        return EXIT_FAILURE;
    }
    /* Do the work */
    if (batch)
        return batch_processing(source, targetfile, segment_id);
   
    if (query)
        return query_addr(sourcefile,segment_id); 
    return EXIT_SUCCESS;
}
