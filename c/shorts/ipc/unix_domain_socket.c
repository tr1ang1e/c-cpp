/*
    writing socket:
        - must be non-blocking to avoid dependency of this service
        - it's SO_SNDBUF should be set via setsockopt() to limit data
*/ 


#define _XOPEN_SOURCE 700


#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


#define SOCK_PATH           ".s.monitor"
#define SOCK_BUF_SIZE       256
#define SOCK_CLOSED         -1


/* --------------------------------------------------------- */
/*                        T Y P E S                          */
/* --------------------------------------------------------- */


typedef struct 
{
    int        read_errno;
    ssize_t    read_len;
} read_result_t;


struct S_s
{
    uint64_t       var1;
    uint64_t       var2;
} __attribute__ ((packed));


typedef struct S_s S;       // '__attribute__ ((packed))' cant be combined with 'typedef' in one expression


/* --------------------------------------------------------- */
/*                 S T A T I C   D A T A                     */
/* --------------------------------------------------------- */


static S s;
static sig_atomic_t terminate; 
static const size_t read_len = sizeof(S);


/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */


static read_result_t readn(int client_sock, char* dest, size_t req_count);
static void sighandler(int signum);
static void clean_terminal();
static void print_monitoring(S *data);
static void print_terminating();


/* --------------------------------------------------------- */
/*                         M A I N                           */
/* --------------------------------------------------------- */


int main(void)
{
    int                   retcode = EXIT_FAILURE;
    int                   rc;
    int                   slen;
    int                   server_sock = 0, client_sock = 0; 
    read_result_t         read_result;
    struct sockaddr_un    client_sockaddr = {};
    struct sockaddr_un    server_sockaddr = {};
  
    /* set termination signal handler */

    struct sigaction act = {};
    sigemptyset(&act.sa_mask);
	act.sa_handler = sighandler;
	act.sa_flags = 0;

    rc = sigaction(SIGINT, &act, NULL);
	if (rc == -1)
    {
        perror("sigaction");
        goto exit;
    }

    /* create, bind ans start listening socket */
    
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        perror("socket");
        goto exit;
    }

    server_sockaddr.sun_family = AF_UNIX;   
    memcpy(server_sockaddr.sun_path, SOCK_PATH, strlen(SOCK_PATH) + 1); 
    slen = sizeof(server_sockaddr);

    rc = bind(server_sock, (struct sockaddr *) &server_sockaddr, slen);
    if (rc == -1)
    {
        perror("bind");
        goto cleanup;
    }

    rc = listen(server_sock, 1);
    if (rc == -1)
    { 
        perror("listen");
        goto cleanup;
    }

    /* accepting socket connection */

    while (1)
    {
        if (terminate == 1) 
        {
            print_terminating();
            goto cleanup;
        }

        client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &slen);
        if (client_sock == -1)
        {
            perror("accept");
            goto cleanup;
        }

        char buf[SOCK_BUF_SIZE] = { 0 };

        /* handle incoming data */

        while (1)
        {
            read_result = readn(client_sock, buf, read_len);

            if (read_result.read_errno == SOCK_CLOSED)
            {
                break;
            }
            else if (read_result.read_errno > 0)
            {
                perror("readn");
                goto cleanup;
            }

            /* display received data */

            clean_terminal();
            print_monitoring((S *)buf);
        }
    }
    
cleanup:

    if (server_sock > 0) 
    {
        close(server_sock);    
    }

    if (client_sock > 0) 
    {
        close(server_sock);    
    }

    rc = unlink(SOCK_PATH);
    if (rc == -1)
    {
        perror("unlink");
    }

exit:
    
    exit(retcode);
}

/* --------------------------------------------------------- */
/*             S T A T I C   F U N C T I O N S               */
/* --------------------------------------------------------- */


static read_result_t readn(int client_sock, char* dest, size_t req_count)
{
    ssize_t          act_read = 0;
    ssize_t          curr_read;
    read_result_t    read_result = {};
    
    while (act_read < req_count)
    {
        curr_read = read(client_sock, dest, req_count);

        if (curr_read < 0)
        {          
            if (EINTR == errno)
            {
                // interrupt before any data have been read
                continue;                           
            }
            else
            {
                // stop, return errno code
                read_result.read_errno = errno;
                break;
            }
        }
        else if (0 == curr_read)
        {
            // EOF, no more data can be read
            read_result.read_errno = SOCK_CLOSED;
            break;
        }
        else
        {
            // another chunk of data have been read
            act_read += curr_read;
            dest += curr_read;
        }
    }

    read_result.read_len = act_read;

    return read_result;
}


static void sighandler(int signum)
{
    terminate = 1;
    return;
}


static void clean_terminal()
{
    printf("\x1B[1J");     // erase from current cursor position to the home position
    printf("\x1B[H");      // return cursor back to the home position

    return;
}


static void print_monitoring(S *data)
{
    printf("%zd\n", data->var1);
    printf("%zd\n", data->var2);

    return;
}


static void print_terminating()
{
    clean_terminal();
    sleep(1);
    
    printf("Terminating monitoring service ");
    sleep(1);

    printf(".");
    sleep(1);
    printf(".");
    sleep(1);
    printf(".");
    sleep(1);

    return;
}
