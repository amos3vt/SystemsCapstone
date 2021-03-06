#include "vtslist.h"
#include "macros.h"
#include <stdbool.h>

void vtslist_init(struct vtslist *vtslist)
{
    list_init(&vtslist->list);
    vtslist_init_locks_only(vtslist);
}

void vtslist_init_locks_only(struct vtslist *vtslist)
{
    pthread_mutex_init(&vtslist->lock, NULL);
    pthread_cond_init(&vtslist->haselems, NULL);
}

void vtslist_cleanup(struct vtslist *vtslist)
{
    pthread_mutex_destroy(&vtslist->lock);
    pthread_cond_destroy(&vtslist->haselems);
}

void vtslist_push_back(struct vtslist *vtslist, struct vtslist_elem *tselem)
{
    tselem->owner = vtslist;

    pthread_mutex_lock(&vtslist->lock);

    list_push_back(&vtslist->list, &tselem->elem);

    pthread_cond_signal(&vtslist->haselems);
    pthread_mutex_unlock(&vtslist->lock);
}

void vtslist_push_front(struct vtslist *vtslist, struct vtslist_elem *tselem)
{
    tselem->owner = vtslist;

    pthread_mutex_lock(&vtslist->lock);

    list_push_front(&vtslist->list, &tselem->elem);

    pthread_cond_signal(&vtslist->haselems);
    pthread_mutex_unlock(&vtslist->lock);
}

struct vtslist_elem *vtslist_pop_back(struct vtslist *vtslist)
{
    struct vtslist_elem *tselem;
    struct list_elem *elem;

    pthread_mutex_lock(&vtslist->lock);

    while (list_empty(&vtslist->list))
        pthread_cond_wait(&vtslist->haselems, &vtslist->lock);

    elem = list_pop_back(&vtslist->list);
    tselem = container_of(elem, struct vtslist_elem, elem);

    pthread_mutex_unlock(&vtslist->lock);

    return tselem;
}

struct vtslist_elem *vtslist_pop_front(struct vtslist *vtslist)
{
    struct vtslist_elem *tselem;
    struct list_elem *elem;

    pthread_mutex_lock(&vtslist->lock);

    while (list_empty(&vtslist->list))
        pthread_cond_wait(&vtslist->haselems, &vtslist->lock);

    elem = list_pop_front(&vtslist->list);
    tselem = container_of(elem, struct vtslist_elem, elem);

    pthread_mutex_unlock(&vtslist->lock);

    return tselem;
}

struct vtslist_elem *vtslist_try_pop_back(struct vtslist *vtslist)
{
    struct vtslist_elem *tselem = NULL;
    struct list_elem *elem;

    pthread_mutex_lock(&vtslist->lock);

    if (list_empty(&vtslist->list))
        goto unlock;

    elem = list_pop_back(&vtslist->list);
    tselem = container_of(elem, struct vtslist_elem, elem);

unlock:
    pthread_mutex_unlock(&vtslist->lock);

    return tselem;
}

struct vtslist_elem *vtslist_try_pop_front(struct vtslist *vtslist)
{
    struct vtslist_elem *tselem = NULL;
    struct list_elem *elem;

    pthread_mutex_lock(&vtslist->lock);

    if (list_empty(&vtslist->list))
        goto unlock;

    elem = list_pop_front(&vtslist->list);
    tselem = container_of(elem, struct vtslist_elem, elem);

unlock:
    pthread_mutex_unlock(&vtslist->lock);

    return tselem;
}

void vtslist_remove(struct vtslist_elem *tselem)
{
    pthread_mutex_lock(&tselem->owner->lock);
    list_remove(&tselem->elem);
    pthread_mutex_unlock(&tselem->owner->lock);

    tselem->owner = NULL;
}