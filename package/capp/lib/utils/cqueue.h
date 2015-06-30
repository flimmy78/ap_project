#ifndef __CQUEUE_H_8766cc9141bd463a83d13843b65a20c3__
#define __CQUEUE_H_8766cc9141bd463a83d13843b65a20c3__
/******************************************************************************/
enum {
    CQUEUE_F_BLOCK      = 0x01,
};

typedef struct {
    unsigned int flag;
    
    channel_t *ch;
    
    void (*free)(void *q);
} cqueue_t;

static inline bool
cq_is_block(cqueue_t *cq)
{
    return os_hasflag(cq->flag, CQUEUE_F_BLOCK);
}

static inline int
cq_init(cqueue_t *cq, int count, unsigned int flag)
{
    cq->ch = os_pch_new(count);
    if (NULL==cq->ch) {
        return -ENOMEM;
    }
    
    cq->flag = flag;
    
    return 0;
}

static inline void
cq_fini(cqueue_t *cq)
{
    if (cq->free) {
        void *pointer = NULL;
        
        while(0==os_pch_read(cq->ch, &pointer)) {
            (*cq->free)(pointer);
        }
    }

    os_ch_free(cq->ch);
}

static inline int
cq_read(cqueue_t *cq, void **pointer)
{
    return os_pch_read(cq->ch, pointer);
}

static inline int
cq_write(cqueue_t *cq, void *pointer)
{
    int err;
    bool droped = false;

retry:
    err = os_pch_write(cq->ch, pointer);
    if (err && false==cq_is_block(cq) && false==droped) {
        void *drop = NULL;
        int ret = cq_read(cq, &drop);
        if (ret) {
            return err;
        }
        
        if (cq->free) {
            (*cq->free)(drop);
        }

        droped = true;

        goto retry;
    }

    return err;
}
/******************************************************************************/
#endif /* __CQUEUE_H_8766cc9141bd463a83d13843b65a20c3__ */
