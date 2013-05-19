#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
void usage(void)
{
    printf("   iv4idx-update [-h] [-r filename] [-s] [-m]\n");
    printf("                  [-w filename | shared_memory_id] [-l index_list] \"BPF filter\"\n");
    printf("\n");
    printf("    -h shows this screen\n");
    printf("    -r read a single file defined in filename\n");
    printf("    -s Specify if the index is a shared memory segment. If not set a\n"); 
    printf("       filename is assumed.\n"); 
    printf("    -m read a list of nfcapd file from stdin\n");
    printf("    -w write ipv4index in the file specified by the index or to the shared\n"); 
    printf("       memory segment id stored in the file specified by the filename. If the\n"); 
    printf("       file does not exist a new file is created otherwise the old file is\n"); 
    printf("       updated.\n");
    printf("    -l update all the shared memory segments or files specified in the\n"); 
    printf("       index_list\n");
    printf("    -v Progress messages are displayed on standard output\n");
    printf("    -d Specify the input data FORMAT\n");
    printf("\n");
    printf("FORMAT\n");
    printf("\n");
    printf("     nfcapd      netflow data included in nfcapd files\n");
    printf("    pcap        pcap file generated with a program using libpcap\n");
    printf("    zpcap       compressed pcap file\n");
    printf("    binary      binary stream of IPv4 addresses expressed in network byte order\n");
    printf("    dotted      A list of IP addresses expressed in dotted decimal notation\n");
    printf("                broken up with a white spaces\n");
    printf("\n");    
    printf("\n");         
    printf("EXAMPLES\n");
    printf("\n");
    printf("    Update the ipv4index of today. The file nfcapd.201305191503 is read and\n"); 
    printf("    the file 20130519.idx is either created or updated.\n");
    printf("\n");
    printf("    $iv4idx-update -d nfcapd -r nfcapd.201305191503 -w  20130519.idx\n"); 
    printf("\n");
    printf("    Update the ipv4index of today located in a shared memory segment which\n");
    printf("    identifier is stored in the file daily_shm.id\n");
    printf("\n");
    printf("    $iv4idx-update -d nfcapd -s -r nfcapd.201305191503 -w daily_shm.id\n");
    printf("\n");
    printf("    Update the ipv4index of today. The file 201305191503.pcap is read and\n");printf("    the file 201305191503.idx is either created or updated.\n");
    printf("\n");         
    printf("    $iv4idx-update -d pcap -r nfcapd.201305191503 -w 201305191503.idx\n"); 
    printf("\n");
    printf("    Go through a binary stream of IPv4 addresses and update the hourly index\n");
    printf("    shared memory segment and the daily shared memory segment.\n"); 
    printf("\n");
    printf("    $iv4idx-update -s -d binary -r feed_A.201305191503 -l monitoring.idx\n");
    printf("\n");
    printf("    The monitoring.idx file contains in this example the two lines:\n");
    printf("    hourly_shm.id\n");
    printf("    daily_shm.id\n");
    printf("\n");
    printf("    where hourly_shm.id is a filename containing the identifier of the hourly\n"); 
    printf("    shared memory segment and daily_shm.id is the file pointing to the daily\n");
    printf("    shared memory segment.\n");
    printf("\n");
    printf("    Read a list of pcap files from stdin  and put them in the ipv4index\n");
    printf("    stored in the file test.idx\n");
    printf("    $echo \"20130518.cap 20130519.cap\" | ipv4index -d pcap -m -w test.idx\n");
    printf("\n");
    printf("    AUTHOR\n");
    printf("        Gerard Wagener\n");
}

int main(int argc, char* argv[])
{
    usage();
    return EXIT_SUCCESS;
}
