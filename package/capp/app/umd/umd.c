#ifndef __THIS_APP
#define __THIS_APP      umd
#endif

#ifndef __THIS_FILE
#define __THIS_FILE     "umd"
#endif

#include "umd.h"

OS_INITER;

extern simpile_server_t um_cli_server;
extern simpile_server_t um_flow_server;
extern simpile_server_t um_timer_server;

struct um_control umd = {
    .head   = {
        .mac    = {HLIST_HEAD_INIT},
        .ip     = {HLIST_HEAD_INIT},
        .list   = LIST_HEAD_INIT(umd.head.list),
    },

    .server = {
        [UM_SERVER_TIMER]   = &um_timer_server,
        [UM_SERVER_CLI]     = &um_cli_server,
        [UM_SERVER_FLOW]    = &um_flow_server,
    },

    .intf = {
        [UM_INTF_FLOW] = {
            .name = UM_INTF_FLOW_DEFT,
        },
        [UM_INTF_TC] = {
            .name = UM_INTF_TC_DEFT,
        },
    },
};

static int
um_handle_server(fd_set *r)
{
    int i, err = 0;

    for (i=0; i<UM_SERVER_END; i++) {
        simpile_server_t *server = umd.server[i];
        
        if (FD_ISSET(server->fd, r)) {
            err = (*server->handle)(server);
            if (err) {
                /* log, but no return */
            }
        }
    }
    
    return 0;
}

static int
um_server_handle_init(fd_set *r)
{
    simpile_server_t *server;
    int i, fdmax = 0;
    
    FD_ZERO(r);
    for (i=0; i<UM_SERVER_END; i++) {
        server = umd.server[i];
        
        FD_SET(server->fd, r);

        fdmax = os_max(fdmax, server->fd);
    }

    return fdmax;
}

static int
um_server_handle(void)
{
    fd_set rset;
    int fdmax = um_server_handle_init(&rset);
    int err;
    
    while(1) {
        err = select(fdmax+1, &rset, NULL, NULL, NULL);
        switch(err) {
            case -1:/* error */
                if (EINTR==errno) {
                    // is breaked
                    debug_event("select breaked");
                    continue;
                } else {
                    debug_trace("select error:%d", -errno);
                    return -errno;
                }
            case 0: /* timeout, retry */
                debug_timeout("select timeout");
                
                return os_assertV(-ETIMEOUT);
            default: /* to accept */
                return um_handle_server(&rset);
        }
    }

    return 0;
}

static int
um_server_init(void)
{
    simpile_server_t *server;
    int i, err;

    for (i=0; i<UM_SERVER_END; i++) {
        server = umd.server[i];
        
        err = (*server->init)(server);
        if (err) {
            return err;
        }
    }
    
    return 0;
}

static int
um_intf_init(void)
{
    int err;
    struct um_intf *intf;

    intf = &umd.intf[UM_INTF_FLOW];
    {
        intf->index = if_nametoindex(intf->name);

        err = os_intf_get_mac(intf->name, intf->mac);
        if (err) {
            return err;
        }
        os_maccpy(umd.basemac, intf->mac);
    }

    intf = &umd.intf[UM_INTF_TC];
    {
        intf->index = if_nametoindex(intf->name);
        
        err = os_intf_get_mac(intf->name, intf->mac);
        if (err) {
            return err;
        }
        
        err = os_intf_get_ip(intf->name, &intf->ip);
        if (err) {
            return err;
        }
        
        err = os_intf_get_netmask(intf->name, &intf->mask);
        if (err) {
            return err;
        }

#if UM_USE_PROMISC
        err = os_intf_set_promisc(intf->name);
        if (err) {
            return err;
        }
#endif
    }
    
    return 0;
}

static void
__umevent_fini(void)
{
    if (false==umd.deinit) {
        umd.deinit = true;
        
        os_system(UM_SCRIPT_EVENT " fini");
    }
}

static void 
__exit(int sig)
{
    __umevent_fini();
    
    exit(sig);
}

static int
__fini(void)
{
    __umevent_fini();
    
    os_fini();

    return 0;
}

static int
__init(void)
{
    int err;

    os_setup_signal_exit(__exit);
    os_setup_signal_callstack(NULL);

    err = os_init();
    if (err) {
        return err;
    }

    return 0;
}

static int
__main(int argc, char **argv)
{
    int err = 0;

    err = um_cfg_init();
    if (err) {
        return err;
    }
    
    err = um_intf_init();
    if (err) {
        return err;
    }

    err = um_server_init();
    if (err) {
        return err;
    }

    limit_update_test();

    os_system(UM_SCRIPT_EVENT " init");

    while(1) {
        um_server_handle();
    }
    
finish:
    return err;
}

#ifndef __BUSYBOX__
#define umd_main  main
#endif

int umd_main(int argc, char **argv)
{
    return os_call(__init, __fini, __main, argc, argv);
}
/******************************************************************************/

