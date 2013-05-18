#include "iv4file.h"
#include "ipv4index.h"
#include "stdlib.h"
void set_current_tz(ipv4cache_hdr_t* hdr)
{
    tzset();
    hdr->creator_tz.timezone = timezone;
    strncpy(hdr->creator_tz.tzname[0], tzname[0], TZSZ);
    strncpy(hdr->creator_tz.tzname[1], tzname[1], TZSZ);
    hdr->creator_tz.daylight = daylight;
}
ipv4cache_hdr_t* build_netflow_hdr(char* source, tz_data_t *tz)
{
    ipv4cache_hdr_t* hdr;
    hdr = calloc(sizeof(ipv4cache_hdr_t),1);
    /* Set the fields */
    if (hdr) {
        strncpy((char*)&hdr->magic, (char*)&IPV4CACHE_MAGIC, HDRSTRSZ);
        hdr->version = IPV4CACHE_VERSION;
        hdr->hash_function = HASH_ONE_TO_ONE;  
        hdr->mergeop = NOT_MERGED;
        strncpy((char*)&(hdr->source[0]),source, HDRSTRSZ); 
        hdr->type = TYPE_NETFLOW;
        strncpy((char*)&(hdr->creator), NAME, HDRSTRSZ); 
        set_current_tz(hdr);
        /* Copy the observation time zone info */
        hdr->observation_tz.timezone = tz->timezone;
        hdr->observation_tz.daylight = tz->daylight;
        strncpy((char*)&hdr->observation_tz.tzname[0], tz->tzname[0],TZSZ);
        strncpy((char*)&hdr->observation_tz.tzname[1], tz->tzname[1],TZSZ);
        /* Set description */
        strncpy((char*)&hdr->description,"EXPERIMENTAL", strlen(EXPERIMENTAL));
        /* Put a timestamp in the structure */
        gettimeofday(&(hdr->creator_time),NULL);
        /* The first seen field and the last seen field are later set */
        hdr->firstseen.tv_sec = 0xFFFFFFFF;
    }
    return hdr;
}

int load_ipv4cache_hdr(gzFile* fp, ipv4cache_hdr_t* hdr )
{
    size_t r;
    /* Clean the memory */
    bzero(hdr, sizeof(ipv4cache_hdr_t));
    r = gzread(fp, hdr,  sizeof(ipv4cache_hdr_t));
    if (r != sizeof(ipv4cache_hdr_t)){
        //FIXME when in a library don't print on stderr
        //fprintf(stderr,"The IPV4CACHE file is too small\n");
        return 0;
    }
    /* Check file magic */
    if (strncmp((char*)&hdr->magic, IPV4CACHE_MAGIC, strlen(IPV4CACHE_MAGIC))) {
        hdr->magic[8]=0;
        //FIXME when in a library don't print on stderr
        //fprintf(stderr,"Invalid magic string: %s\n",hdr->magic);
        return 0;
    }
    /* Check file version */
    if (hdr->version != IPV4CACHE_VERSION) {
        //FIXME when in a library don't print on stderr
        //fprintf(stderr,"Unsupported version of IPV4CACHE: %d\n",hdr->version);
        return 0;
    }
    /* Check the hashing function */
    if (hdr->hash_function != HASH_ONE_TO_ONE){
        //FIXME when in a library don't print on stderr
        //fprintf(stderr, "Unsupported hash function is used: %d\n",
        //        hdr->hash_function);
        return 0;
    } 
    /* Assume the header is fine */
    return 1;
}

ipv4cache_hdr_t* create_local_header(char* source)
{
    tz_data_t tz;
    tzset();
    tz.timezone = timezone;
    strncpy(tz.tzname[0], tzname[0], TZSZ);
    strncpy(tz.tzname[1], tzname[1], TZSZ);
    tz.daylight = daylight;
    return build_netflow_hdr(source, (tz_data_t*)&tz);
}

int store_bitindex(char* filename, ipv4cache_hdr_t* hdr, uint8_t* bitindex)
{
    gzFile *fp;
    int r,out;
    out = 0; 
    fp = gzopen(filename,"wb");
    if (fp) {
        r = gzwrite(fp, hdr, sizeof(ipv4cache_hdr_t));
        if (r == sizeof(ipv4cache_hdr_t)) {
            r = gzwrite(fp, bitindex, SPACE_SIZE);
            if (r == SPACE_SIZE) {
                out = 1;
            }else{
                //TODO add errno per instance
                //fprintf(stderr,"Could not store bitindex");
            }
        }else{
            //TODO add errno per instance
            //fprintf(stderr,"Could not store header\n");
        }
        gzclose(fp);
    } else{
        //TODO add errno per instance
        //fprintf(stderr,"Could not open file %s. cause: %s\n",filename,
        //                                              strerror(errno));
    }
    return out;
}

ipv4cache_hdr_t* load_bitindex(char* filename, uint8_t* bitindex)
{
    gzFile *fp;
    int r;
    ipv4cache_hdr_t* hdr;
    //FIXME use softer alternative
    //assert(filename && bitindex);

    hdr = calloc(sizeof(ipv4cache_hdr_t),1);
    if (!hdr)
        return NULL;
    fp = gzopen(filename,"rb");
    if (fp) {
        if (load_ipv4cache_hdr(fp, hdr)){
            /* Header was loaded and checks passed load bitindex*/
            r = gzread(fp, bitindex, SPACE_SIZE);
            if (r == SPACE_SIZE) {
                gzclose(fp);
                return hdr;    
            } else{
                //TODO use proper error handling
                //fprintf(stderr,"File %s seems to be truncated\n",filename);    
            }
        }
        gzclose(fp);
    }
    /* There was an error somewhere */
    return NULL;    
}

