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
#ifndef NFREADER
#define NFREADER
#include "ipv4index.h"
#include <libnfdump/libnfdump.h>
#include <stdlib.h>
#include <arpa/inet.h>

/* Take a nfcapd filename as argument and updated 
 * the bitindex also passed as argument. 
 * Hence, multiple filenames can be processed and the included IP addresses
 * canbe mapped on the same bitindex.
 *
 * Return values: 
 * 1 on success 
 * 0 on errors (due to the failure of initlib) 
 */ 
int index_nfcapd_file(char* filename, ipv4index_t *ipv4index);
#endif

