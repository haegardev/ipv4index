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
#include "ipv4index.h"
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

uint8_t test_bit(uint8_t* bs, uint32_t addr)
{
    return bs[addr>>3] & (1 << (addr-((addr>>3)<<3)));    
} 

void dump(uint8_t* bitindex)
{   
    int i;
    printf("--- BEGIN ---\n");
    for (i=0;i<SPACE / 8;i++) {
        printf("%d %d\n", i, bitindex[i]);
    }
    printf("--- END ---\n");
}


