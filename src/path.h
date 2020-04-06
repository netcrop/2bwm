#ifndef PATH_H
#define PATH_H
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cube.h"
#ifndef debugging
#define debugging 1
#endif
// pathindex.
enum{
    maxmoni = CUBE_A,
    maxwsi = CUBE_B,
    maxloci = CUBE_C,
    dimsize = (CUBE_A * CUBE_B * CUBE_C),
    // save some memory due to only one wsfocpi per workspace.
    wsfocsize = (CUBE_A * CUBE_B),
    dimpi = dimsize + wsfocsize,
    datpi = dimsize + wsfocsize + 1,
    recpi = dimsize + wsfocsize + 2,
    winpi = dimsize + wsfocsize + 3,
    monpi = dimsize + wsfocsize + 4,
    selpi = dimsize + wsfocsize + 5,
    unkpi = dimsize + wsfocsize + 6,
    fixpi = dimsize + wsfocsize + 7,
    icopi = dimsize + wsfocsize + 8,
    pospi = dimsize + wsfocsize + 9,
    propsize = 10,
    pathsize = dimsize + wsfocsize + propsize,
    nodepath = 0,
    datpath = 1,
    recpath = 2,
    winpath = 3,
    monpath = 4,
    selpath = 5,
    unkpath = 6,
    fixpath = 7,
    icopath = 8,
    pospath = 9,
    pathlen = 10,
    dim = 1,
    dat = 2,
    datdim = 3,
    recdat = 6,
    recdatdim = 7,
    windat = 10,
    winrecdat = 14,
    mondat = 18,
    windatdim = 11,
    mondatdim = 19,
    seldat = 34,
    selrecdat = 38,
    selwindat = 42,
    selwindatdim = 43,
    unkwindatdim = 75,
    fixdat = 130,
    fixwindat = 138,
    fixwindatdim = 139,
    fixselwindat = 170,
    fixselwindatdim = 171,
    icowindat = 266,
    icowindatdim = 267,
    icoselwindat = 298,
    icoselwindatdim = 299,
};
typedef struct node_t node_t;
static node_t *path[pathsize];
static int length[pathlen];
static int othercount = 0;
static int datacount = 0;
static int nodecount = 0;
static int nodeid = 0;
static int curmoni = 0;
static int curwsi = 0;
static int curloci = 0;
#define incmoni (++curmoni >= maxmoni ? curmoni = 0 : curmoni)
#define incwsi (++curwsi >= maxwsi ? curwsi = 0 : curwsi)
#define incloci (++curloci >= maxloci ? curloci = 0 : curloci)
#define stepup(x,y) (1+(x) >= (y) ? 0 : 1+(x))

#define moni(x) cube_ai(x)
#define wsi(x) cube_bi(x)
#define loci(x) cube_ci(x)
#define next(node,pi) node->next[propi(pi)]
#define prev(node,pi) node->prev[propi(pi)]

#define propi(x) ((x) > (dimsize + wsfocsize) ? (x)%(dimsize + wsfocsize) : 0)
#define curpi (cube[(curmoni)][(curwsi)][(curloci)])
#define wspi(B) (cube[(curmoni)][(B)][(0)])
#define locpi(C) (cube[(curmoni)][(curwsi)][(C)])
#define pathindex(A,B,C) (cube[(A)][(B)][(C)])
// save some memory due to only one wsfocpi per workspace.
#define wsfocpi (dimsize + (cube[(curmoni)][(curwsi)][0])/maxloci)
#define focpi(A,B) (dimsize + (cube[(A)][(B)][0]))
#define curwspi (cube[(curmoni)][(curwsi)][(0)])

struct node_t
{
    void *data;
    int id,type,dimpi,headpi,prop;
    node_t *prev[propsize];
    node_t *next[propsize];
};
static inline void debug(int,node_t*,const char*,char*);
static inline void debug2(const char*,char*);
static inline void linknode(int,node_t*);
static inline void unlinknode(int,node_t*);
static inline node_t* addnode(size_t);
static inline void denode(node_t*);
static inline void delall(void);
static inline void alocate(void**,size_t);
static inline void delocate(void*);
static inline void* alocatenode(size_t);
static inline void delocatenode(node_t*);
static inline void countmem(const char *,char*);

void
countmem(const char *caller,char *msg)
{
    if(!debugging)
        return;
    printf("%-12s %s:datacount:%d nodecount:%d othercount:%d\n",
        caller,msg,datacount,nodecount,othercount);
}
static inline void
alocate(void **p,size_t size)
{
    if(!(*p = malloc(size)))
        exit(1);
    othercount++;
}
inline void static
delocate(void *p)
{
    if(!p)
        return;
    free(p);
    othercount--;
}
inline void static
delocatenode(node_t *node)
{
    if(!node)
        return;
    if(node->data){
        free(node->data);
        datacount--;
    }
    free(node);
    nodecount--;
    return;
}
inline void*
alocatenode(size_t size)
{
    void *data = NULL;
    node_t *node = NULL;
    if(!(data = malloc(size)) || !(node = malloc(sizeof(node_t))))
        return NULL;
    length[nodepath]++;
    datacount++;
    nodecount++;
    node->data = data;
    return node;
}
static inline void
initallpath()
{
    for(int i = 0; i < pathsize; i++)
        path[i] = NULL;
}
void
updateprop(int headpi,node_t *node)
{
    int pi = propi(headpi);
    node->prop &= ~(1 << pi);
    node->prop |= ((bool)node->next[pi] << pi);
//    debug(headpi,node,__func__,"");
}
void
linknode(int headpi,node_t *node)
{
    int pi = propi(headpi);
    // ignore node already on these paths.
    if(!node || node->next[pi])
        return;
    node->headpi = headpi;
    if(!pi)
        node->dimpi = headpi;
    else
        length[pi]++;
    // path is empty.
    if(!path[headpi]){
        node->prev[pi] = node->next[pi] = node;
        path[headpi] = node;
        updateprop(headpi,node);
        debug(headpi,node,__func__,"1'st node.");
        return;
    }
/* This case shouldn't happen. If it does, check enum declarations and don't
 * focus any node from recpi.
 */
//    printf("%s headpi: %d pi: %d\n",__func__,headpi,pi);
//   if(!path[headpi]->prev[pi] || !path[headpi]->next[pi]){
//       node->next[pi] = path[headpi];
//       node->next[pi]->next[pi] = node;
//       node->next[pi]->prev[pi] = node;
//       path[headpi] = node;
//       updateprop(headpi,node);
//       debug(headpi,node,__func__,"2'd nodes");
//       return;
//   }
    node->next[pi] = path[headpi];
    node->next[pi]->prev[pi]->next[pi] = node;
    node->prev[pi] = node->next[pi]->prev[pi];
    node->next[pi]->prev[pi] = node;
    path[headpi] = node;
    updateprop(headpi,node);
    debug(headpi,node,__func__,"more nodes.");
}
// alocate new node to path[datpi]
inline node_t*
addnode(size_t size)
{
    node_t *newhead = path[recpi];
    printf("%s rechead:%p\n",__func__,(void*)newhead);
    if(newhead && next(newhead,recpi)){
        newhead->dimpi = -1;
        newhead->headpi = datpi;
        unlinknode(recpi,newhead);
        debug(datpi,newhead,__func__,"recycled");
        return newhead;
    }
    if (!(newhead = (node_t *)alocatenode(size)))
        return NULL;
    newhead->type = size;
    newhead->id = nodeid++;
    newhead->dimpi = -1;
    newhead->headpi = datpi;
    newhead->prop = 0;
    for(int i = 0; i < propsize; i++){
        newhead->next[i] = newhead->prev[i] = NULL;
    }
    linknode(datpi,newhead);
    return newhead;
}
// remove reference node from path,
// reset header path except datpi.
// preserve node->dimpi,headpi due to linknode needs them.
void
unlinknode(int headpi,node_t *node)
{
    int pi = propi(headpi);
    // exclude datpi, empty head and node doesn't exist on this path.
    if(headpi == datpi || !path[headpi] || !node || !node->next[pi]){
        return;
    }
    if(pi){
        --length[pi] < 0 ? length[pi] = 0 : length[pi];
    }
    // the only one in path.
    if(node == node->next[pi] && node == path[headpi]){
        node->next[pi] = node->prev[pi] = path[headpi] = NULL;
        updateprop(headpi,node);
        debug(headpi,node,__func__,"empty");
        return;
    }
    node->next[pi]->prev[pi] = node->prev[pi];
    node->prev[pi]->next[pi] = node->next[pi];
    // reset path[headpi]
    if(path[headpi] == node)
        path[headpi] = node->next[pi];
    // remove reference pi
    node->next[pi] = node->prev[pi] = NULL;
    updateprop(headpi,node);
    debug(headpi,node,__func__,"");
}
// dereference all path form this node.
inline void
denode(node_t *node)
{
    // ignore node already in recpi path.
    if (!path[datpi] || !node || next(node,recpi))
        return;
    debug2(__func__,"start");
    // dereference path form this node, except datpi.
    unlinknode(node->dimpi,node);
    node->dimpi = -1;
    node->headpi = datpi;
    for(int headpi = winpi; headpi <= pospi; headpi++){
        unlinknode(headpi,node);
    }
    // add into recycle path.
    linknode(recpi,node);
    debug2(__func__,"ended");
}
// remove and free everyting.
void
delall(void)
{
    countmem(__func__,"start");
    node_t *tail = NULL;
    node_t *head = path[datpi];
    int pi = propi(datpi);
    if(!head)
        return;
    for(int i = 0; i < length[nodepath]; i++){
         if(!(tail = head))
            continue;
         head = head->next[pi];
         delocatenode(tail);
    }
    *path = NULL;
    countmem(__func__,"ended");
}
void
showpath(int headpi)
{
    int pi = propi(headpi);
    node_t *node = path[headpi];
    if(!node)
        return;
    do{
        debug(headpi,node,__func__,"");
    }while((node = node->prev[pi]) && node != path[headpi]);
}
void
debug2(const char *funname,char *mesg)
{
    if(!debugging)
        return;
    printf("%-14s %s\n",funname,mesg);
}
void
debug(int headpi,node_t *node,const char *funname, char *mesg)
{
    if(!debugging)
        return;
    int len = 20;
    char piname[len];
    char propname[len];
    for(int i = 0; i <  len; i++){
        propname[i] = piname[i] = '\0';
    }
    switch(headpi){
        case dimpi: snprintf(piname,6,"%.*s",5,"dimpi");break;
        case datpi: snprintf(piname,6,"%.*s",5,"datpi");break;
        case recpi: snprintf(piname,6,"%.*s",5,"recpi");break;
        case winpi: snprintf(piname,6,"%.*s",5,"winpi");break;
        case monpi: snprintf(piname,6,"%.*s",5,"monpi");break;
        case selpi: snprintf(piname,6,"%.*s",5,"selpi");break;
        case unkpi: snprintf(piname,6,"%.*s",5,"unkpi");break;
        case fixpi: snprintf(piname,6,"%.*s",5,"fixpi");break;
        case icopi: snprintf(piname,6,"%.*s",5,"icopi");break;
        case pospi: snprintf(piname,6,"%.*s",5,"pospi");break;
        default: snprintf(piname,6,"%.*s",5,"dimpi");break;
    }
    switch(node->prop){
      case dim: snprintf(propname,8,"%.*s",7,"dim");break;
      case dat: snprintf(propname,8,"%.*s",7,"dat");break;
      case datdim: snprintf(propname,8,"%.*s",7,"datdim");break;
      case recdat: snprintf(propname,11,"%.*s",10,"recdat");break;
      case windat: snprintf(propname,11,"%.*s",10,"windat");break;
      case mondat: snprintf(propname,11,"%.*s",10,"mondat");break;
      case seldat: snprintf(propname,11,"%.*s",10,"seldat");break;
      case fixdat: snprintf(propname,11,"%.*s",10,"fixdat");break;
      case recdatdim: snprintf(propname,14,"%.*s",13,"recdatdim");break;
      case winrecdat: snprintf(propname,14,"%.*s",13,"winrecdat");break;
      case windatdim: snprintf(propname,14,"%.*s",13,"windatdim");break;
      case mondatdim: snprintf(propname,14,"%.*s",13,"mondatdim");break;
      case selwindat: snprintf(propname,14,"%.*s",13,"selwindat");break;
      case fixwindat: snprintf(propname,14,"%.*s",13,"fixwindat");break;
      case icowindat: snprintf(propname,14,"%.*s",13,"icowindat");break;
      case selrecdat: snprintf(propname,14,"%.*s",13,"selrecdat");break;
      case selwindatdim: snprintf(propname,17,"%.*s",16,"selwindatdim");break;
      case unkwindatdim: snprintf(propname,17,"%.*s",16,"unkwindatdim");break;
      case fixwindatdim: snprintf(propname,17,"%.*s",16,"fixwindatdim");break;
      case icowindatdim: snprintf(propname,17,"%.*s",16,"icowindatdim");break;
      case icoselwindat: snprintf(propname,17,"%.*s",16,"icoselwindat");break;
      case fixselwindat: snprintf(propname,17,"%.*s",16,"fixselwindat");break;
      case fixselwindatdim: snprintf(propname,20,"%.*s",19,"fixselwindatdim");break;
      case icoselwindatdim: snprintf(propname,20,"%.*s",19,"icoselwindatdim");break;
      default: sprintf(propname,"%d",node->prop);break;
    }
    printf("%-14s %-10s:%2d:%2d:%2d nodeid:%2d %20s %s\n",funname,
        piname,moni(node->dimpi),wsi(node->dimpi),
        loci(node->dimpi),node->id,propname,mesg);
}
