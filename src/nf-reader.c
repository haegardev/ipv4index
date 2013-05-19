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
#include "nf-reader.h"
int index_nfcapd_file(char* filename, ipv4index_t* ipv4index)
{
    libnfstates_t* states;
    master_record_t* rec;
    
    states = initlib(NULL, filename, NULL);
    if (states) {
        do {
            rec = get_next_record(states);
            if (rec) {
                if ( (rec->flags & FLAG_IPV6_ADDR ) != 0 ) {
                    continue;
                    /* Bitset is not suited for IPv6 */
                }
                /* Use network byte order to be independent of the architecture */ 
                rec->v4.srcaddr = htonl(rec->v4.srcaddr);
                rec->v4.dstaddr = htonl(rec->v4.dstaddr);
                /* Update the bitindex */
                BITINDEX_SET(ipv4index->bitindex,rec->v4.srcaddr);
                BITINDEX_SET(ipv4index->bitindex,rec->v4.dstaddr);
                /* Sometimes the order of the nfcapd files is not assured
                 * The first flow record in an nfcapd file does not necessary have the oldest timestamp
                 * The last flow record in an nfcapd file is not necessary the youngest.
                 * FIXME If the timestamps are equal, the usec are not properly updated
                 */ 
                 
                if (rec->first < ipv4index->header->firstseen.tv_sec) {
                    /* This flow is older than all the other flows seen before */
                    ipv4index->header->firstseen.tv_sec = rec->first;
                    ipv4index->header->firstseen.tv_usec = rec->msec_first;
                }
                if (rec->last > ipv4index->header->lastseen.tv_sec) {
                    /* This flow more recent than all the other observed flows */
                    ipv4index->header->lastseen.tv_sec = rec->last;
                    ipv4index->header->lastseen.tv_usec = rec->msec_last;
                }
            }
        } while (rec);
        

        /* Close the nfcapd file and free up internal states */
        libcleanup(states);
        /* Everything went fine */
        return 1;
    }
    /* There was something broken */
    return 0;
}
