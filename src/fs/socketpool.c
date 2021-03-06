#include "socketpool.h"

#include "../dbg.h"


/** destroy a single 0mq socket */
void
SocketPool_socket_destroy(void * sock)
{
    if (sock != NULL) {
        zmq_close(sock);
        sock = NULL;
    }
}


bool
SocketPool_init(SocketPool * socketpool, void * context, const char * socket_name,
        int socket_type)
{
    int rc;

    memset(socketpool, 0, sizeof(SocketPool));

    socketpool->socket_name = strdup(socket_name);
    check_mem(socketpool->socket_name);

    socketpool->socket_type = socket_type;
    socketpool->context = context;

    rc = pthread_key_create(&(socketpool->key), SocketPool_socket_destroy);
    check((rc==0), "pthread_key_create failed.");

    return true;

error:

    if (socketpool->socket_name != NULL) {
        free(socketpool->socket_name);
    }
    if (socketpool->key) {
        pthread_key_delete(socketpool->key);
    }
    return false;
}


void
SocketPool_deinit(SocketPool * sp)
{
    if (sp) {
        if (sp->socket_name != NULL) {
            free(sp->socket_name);
        }
        if (sp->key) {
            pthread_key_delete(sp->key);
        }
    }
}


void
SocketPool_renew_socket(SocketPool * sp)
{
    check(sp != NULL, "passed socketpool is NULL");

    void * sock = pthread_getspecific(sp->key);
    if (sock != NULL) {
        zmq_close(sock);
        if (pthread_setspecific(sp->key, NULL) != 0) {
            debug("could not clear socket in thread");
        }
    }
    return;
error:
    return;
}

void *
SocketPool_get_socket(SocketPool * sp)
{
    void * sock = NULL;

    check(sp != NULL, "passed socketpool is NULL");

    sock = pthread_getspecific(sp->key);
    if (sock == NULL) {

        /* create a new socket */
        sock = zmq_socket(sp->context, sp->socket_type);
        check((sock != NULL), "Could not create 0mq socket");

        int hwm = 1; /* prevents memory leaks when fuse interrupts while waiting on server */
        zmq_setsockopt(sock, ZMQ_HWM, &hwm, sizeof(hwm));

#ifdef ZMQ_MAKE_VERSION
#if ZMQ_VERSION >= ZMQ_MAKE_VERSION(2,1,0)
        int linger = 0;
        zmq_setsockopt(sock, ZMQ_LINGER, &linger, sizeof(linger));
#endif
#endif

        check((zmq_connect(sock, sp->socket_name) == 0), "could not connect to socket");
        check((pthread_setspecific(sp->key, sock) == 0), "could not set socket in thread");
    }

    return sock;

error:
    SocketPool_socket_destroy(sock);
    return NULL;
}

