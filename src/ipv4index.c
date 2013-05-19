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
