#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main(int argc, char **argv)
{
    //struct sockaddr_rc loc_addr = {00:1B:DC:04:AE:D7}, rem_addr = {44:d4:e0:c8:45:11};
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[1024] = { 0 };
    int s, client, bytes_read, status;
    int number, counter;
    socklen_t opt = sizeof(rem_addr);
    int ch = 1 < argc ? atoi(argv[1]) : 1;
    printf("channel [%d]\n",ch);

    // allocate socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    printf("socket [%d]\n",s);

    // bind socket to port 22 of the first available 
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) ch;
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // put socket into listening mode
    listen(s, 1);

    char LDIR[1024]={"/root/github/selfie201506"};
    char GDIR[1024]={"/var/www/html/loveselfie/pictures"};
while(1){

    // accept one connection
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);
    printf("ACCEPT\n"); fflush(stdout);

    ba2str( &rem_addr.rc_bdaddr, buf );
    fprintf(stderr, "accepted connection from %s\n", buf);
    memset(buf, 0, sizeof(buf));

    bytes_read = read(client, buf, sizeof(buf));
    if( bytes_read > 0 ) {
        printf("[%s]\n", buf);
    }


    for(number=0;number<2; number++){

        for(counter=5;counter>0;counter--){
       	    char buf1[5]={0};
            sprintf(buf1,"%d ",counter);
            printf("\a"); 
            printf("%s  ", buf1); fflush(stdout);
            status = write(client, buf1, sizeof(buf1));
            sleep(1);  // wait 1sec
        }

        char takephoto[1024]={0};
        sprintf(takephoto,"/usr/bin/raspistill -t 1 -o %s/%d.jpg -w 2339 -h 1654", LDIR, number);
        //printf("[%s]\n", takephoto);
        system(takephoto);

        sprintf(buf,"Photo No %d taken.",number);
        printf("%s\n", buf);
        status = write(client, buf, sizeof(buf));
        if( status < 0 ) perror("uh oh");

    }

    // close connection
    close(client);

        char command[1024]={0};
        sprintf(command,"/usr/bin/scp %s/*.jpg root@www.broadbandjapan.net:%s", LDIR, GDIR);
	printf("%s\n", command); fflush(stdout);
	system(command);
        sprintf(command,"/bin/rm -f %s/*.jpg", LDIR);
	printf("%s\n", command); fflush(stdout);
	system(command);
        sprintf(buf,"Upload photos.");   printf("%s\n", buf);
}

    close(s);
    return 0;
}
