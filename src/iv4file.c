/*
 *   ipv4index  
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
 */
#include "iv4file.h"
#include "ipv4index.h"
#include "stdlib.h"
#include <assert.h>
/*
 * Helper function for the function build_netflow_hdr. This function sets the
 * current timezone settings in the ipv4cache_hdr_t headers specified as 
 * as parameter. The header passed as parameter is modified.
 */
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

int load_ipv4cache_hdr(ipv4index_t* self, gzFile* fp)
{
    size_t r;
    /* Clean the memory */
    bzero(self->header, sizeof(self->header));
    r = gzread(fp, self->header,  sizeof(ipv4cache_hdr_t));
    if (r != sizeof(ipv4cache_hdr_t)){
        self->error_code = ERR_TRUNCHDR;
        return 0;
    }
    /* Check file magic */
    if (strncmp((char*)&self->header->magic, IPV4CACHE_MAGIC, 
        strlen(IPV4CACHE_MAGIC))) {
        self->header->magic[8]=0;
        self->error_code = ERR_MAGIC;
        strncpy((char*)&(self->error_data), self->header->magic, ERRDATAMAX);
        return 0;
    }
    /* Check file version */
    if (self->header->version != IPV4CACHE_VERSION) {
        self->error_code = ERR_VERSION;
        snprintf((char*)&(self->error_data), ERRDATAMAX,"%d", 
                 self->header->version);
        return 0;
    }
    /* Check the hashing function */
    if (self->header->hash_function != HASH_ONE_TO_ONE){
        self->error_code = ERR_HASHFUNCTION;
        snprintf((char*)&(self->error_data), ERRDATAMAX, "%d" , 
                 self->header->hash_function);
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

int store_bitindex(ipv4index_t* ipv4index, char* filename, ipv4cache_hdr_t* hdr)
{
    gzFile *fp;
    int r,out;
    out = 0; 
    assert(ipv4index->bitindex); 
    fp = gzopen(filename,"wb");
    if (fp) {
        r = gzwrite(fp, hdr, sizeof(ipv4cache_hdr_t));
        if (r == sizeof(ipv4cache_hdr_t)) {
            r = gzwrite(fp, ipv4index->bitindex, SPACE_SIZE);
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


//FIXME an int is sufficent as return value
ipv4cache_hdr_t* load_bitindex(ipv4index_t* self, char* filename)
{
    gzFile *fp;
    int r;
    
    if (!(filename && self->bitindex))
        self->error_code = INVALID_PARAMETERS;

    self->header = calloc(sizeof(ipv4cache_hdr_t),1);
    if (!self->header)
        return NULL;
    fp = gzopen(filename,"rb");
    if (fp) {
        if (load_ipv4cache_hdr(self,fp)){
            /* Header was loaded and checks passed load bitindex*/
            r = gzread(fp, self->bitindex, SPACE_SIZE);
            if (r == SPACE_SIZE) {
                gzclose(fp);
                return self->header;    
            } else{
                self->error_code = ERR_TRUNCFILE;
            }
        }
        gzclose(fp);
    }else{
        self->error_code = ERR_NOSUCHFILE;    
        self->shadow_errno = errno;
    }
    /* There was an error somewhere */
    return NULL;    
}

