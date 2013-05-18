#ifndef IPV4FILE
#define IPV4FILE
#include "ipv4index.h"
#include <time.h>
#include <string.h>
#include <zlib.h>
/*
 * Helper function for the function build_netflow_hdr. This function sets the
 * current timezone settings in the ipv4cache_hdr_t headers specified as 
 * as parameter. The header passed as parameter is modified.
 */
void set_current_tz(ipv4cache_hdr_t* hdr);
/* Creates a simplified new header for a bitindex file. A source can be 
 * specified as parameter to identify the source of IP addresses presented
 * in the corresponding bitindex. A timezone structure related to the sources
 * of IP addresses can also be specified.
 * On success this function returns and ipv4cache_hdr_t header.
 * On error it returns NULL.
 * Other fields such as the firstseen, lastseen fields are not set and should
 * be set later. The memory used by the returned header should be freed.
 */
ipv4cache_hdr_t* build_netflow_hdr(char* source, tz_data_t *tz);
/* 
 * Loads an ipv4cache header. This function is a helper function for the 
 * function load_bitindex and should not be directly used.
 * The header is read from the file pointer  fp and an empty header structure
 * passed as parameter is updated by this function.
 * Returns 1 on success
 * Returns 0 on error
 */
int load_ipv4cache_hdr(gzFile* fp, ipv4cache_hdr_t* hdr );

/* Set the local observation time zone to the header 
 * A name of the source should be passed as parameter
 * Returns the results of the function build_netflow_hdr
 */
ipv4cache_hdr_t* create_local_header(char* source);
#endif

