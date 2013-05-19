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
#ifndef IPV4IPC
#define IPV4IPC
#include "ipv4index.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <strings.h>
/* Function to iniliatlize a shared memory segment. The shared memory segment
 * identifier is stored in the idfile using decimal notation (fitted for 
 * iprm)
 */
int init_shm(char* idfile);

/* Function to iniliatlize a shared memory segment. The shared memory segment
 * identifier is stored in the idfile using decimal notation (fitted for 
 * iprm)
 */
int reset_shm(int segment_id);

#endif
