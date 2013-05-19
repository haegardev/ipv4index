/*
 *   IPV4index
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
#ifndef IPV4INDEX
#define IPV4INDEX
#include <stdint.h>
#include <sys/time.h>
#include <stdlib.h>
#define BITINDEX_SET(bs, addr) bs[addr>>3] |= 1 << (addr-((addr>>3)<<3))
#define SPACE 0xFFFFFFFF
#define LASTBLOCK 536854528
#define SPACE_SIZE SPACE / 8 + 1
#define CHUNK 16384 
#define NO_MEMORY 3
#define TARGET_FILE_FAILURE 4 
#define TARGET_FILE_TRUNC 5
#define INTERNAL_ERROR 6
#define NAME "NFIPv4CACHE"
#define IPV4CACHE_MAGIC "IPV4CACHE" 
#define IPV4CACHE_VERSION 1 
#define HASH_ONE_TO_ONE 1
#define HDRSTRSZ 128  
#define MAXSOURCES 16
#define EXPERIMENTAL "EXPERIMENTAL"

/* Netflow types */
#define TYPE_NETFLOW 1
#define TYPE_PCAP 2
#define TYPE_OTHER 3

/* Merge types */
#define NOT_MERGED 1

#define TZSZ 32
/* IPv4Cache Header */                                                          

#define ERRDATAMAX 64    
#define BAREIPV4INDEX 0 
#define FULLIPV4INDEX 1

/* ERROR CODES */
#define INVALID_PARAMETERS 1

#define ERR_NOSUCHFILE 10
#define ERR_TRUNCFILE 11
#define ERR_TRUNCHDR 12
#define ERR_MAGIC 13
#define ERR_VERSION 14
#define ERR_HASHFUNCTION 15

#define ERR_STOREOPEN 20
#define ERR_GZIPFAIL 21
#define ERR_STOREHDR 22
#define ERR_STOREBITIDX 23
typedef struct tz_data_s {
    uint64_t timezone;
    char     tzname [TZSZ][2];
    uint32_t daylight;
} tz_data_t;          
                                                                  
typedef struct ipv4cache_hdr_s {                                                
    char            magic[9];                                                          
    uint8_t         version;                                                        
    uint8_t         hash_function;                                                  
    uint8_t         mergeop;
    char            source [HDRSTRSZ][MAXSOURCES];
    uint8_t         type;
    char            creator[HDRSTRSZ];
    struct timeval  creator_time; 
    tz_data_t       creator_tz;
    tz_data_t       observation_tz;
    struct timeval  firstseen;
    struct timeval  lastseen;
    char            description[HDRSTRSZ];                                          
}ipv4cache_hdr_t;                     

typedef struct ipv4index_s {
    uint8_t* bitindex;
    uint16_t error_code;
    char error_data[ERRDATAMAX];
    int shadow_errno;
    ipv4cache_hdr_t* header;
} ipv4index_t;

/*
 * Initialize a new bitindex.
 * The number of bits is specified with the parameter nelem.
 * Returns a pointer to the an ipv4index structure on success.
 * The bitindex itself is in ipv4index->bitindex which also initialized
 * Returns NULL when no memory is available.
 * The memory should be freed when it is not used.
 */

ipv4index_t* bitindex_new(uint32_t nelem, int flags);

/* Sets a bit related to an IPV4 address defined in the parameter addr. The 
 * bitset bs is updated. If a lot of such INSERT operations are done, this
 * function should not be used because for each operation a stackframe is 
 * build. Therefore the macro BITINDEX_SET should be used
 */ 
uint8_t bit_index_set(uint8_t* bs, uint32_t addr);
/* Test if an IPv4 address addr is known in the bitset bs.
 * Returns 0 if the IP address is not known
 * Returns a positive number if the IP address is known
 */ 
uint8_t test_bit(uint8_t* bs, uint32_t addr);

#endif
