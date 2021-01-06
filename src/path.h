#ifndef STIMY_H
#include <stimy.h>
#endif
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
enum{
    maxmoni = CUBE_A,
    maxwsi = CUBE_B,
    maxloci = CUBE_C,
    dimsize = (CUBE_A * CUBE_B * CUBE_C),
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
    stimy_pre();
    if(!debugging)
        stimy_post(return;);
     stimy_echo(printf,printf("%-12s %s:datacount:%d nodecount:%d othercount:%d\n",
        caller,msg,datacount,nodecount,othercount));
    stimy_post();
}
static inline void
alocate(void **p,size_t size)
{
    stimy_pre();
    if(!(*p =  stimy_echo(malloc,malloc(size))))
         stimy_echo(exit,exit(1));
    othercount++;
    stimy_post();
}
inline void static
delocate(void *p)
{
    stimy_pre();
    if(!p)
        stimy_post(return;);
     stimy_echo(free,free(p));
    othercount--;
    stimy_post();
}
inline void static
delocatenode(node_t *node)
{
    stimy_pre();
    if(!node)
        stimy_post(return;);
    if(node->data){
         stimy_echo(free,free(node->data));
        datacount--;
    }
     stimy_echo(free,free(node));
    nodecount--;
    stimy_post(return;);
    stimy_post();
}
inline void*
alocatenode(size_t size)
{
    stimy_pre();
    void *data = NULL;
    node_t *node = NULL;
    if(!(data =  stimy_echo(malloc,malloc(size))) || !(node =  stimy_echo(malloc,malloc(sizeof(node_t)))))
        stimy_post(return NULL;);
    length[nodepath]++;
    datacount++;
    nodecount++;
    node->data = data;
    stimy_post(return node;);
    stimy_post();
}
static inline void
initallpath()
{
    stimy_pre();
    for(int i = 0; i < pathsize; i++)
        path[i] = NULL;
    stimy_post();
}
void
updateprop(int headpi,node_t *node)
{
    stimy_pre();
    int pi =  stimy_echo(propi,propi(headpi));
    node->prop &= ~(1 << pi);
    node->prop |= ((bool)node->next[pi] << pi);
    stimy_post();
}
void
linknode(int headpi,node_t *node)
{
    stimy_pre();
    int pi =  stimy_echo(propi,propi(headpi));
    if(!node || node->next[pi])
        stimy_post(return;);
    node->headpi = headpi;
    if(!pi)
        node->dimpi = headpi;
    else
        length[pi]++;
    if(!path[headpi]){
        node->prev[pi] = node->next[pi] = node;
        path[headpi] = node;
         stimy_echo(updateprop,updateprop(headpi,node));
         stimy_echo(debug,debug(headpi,node,__func__,"1'st node."));
        stimy_post(return;);
    }
    node->next[pi] = path[headpi];
    node->next[pi]->prev[pi]->next[pi] = node;
    node->prev[pi] = node->next[pi]->prev[pi];
    node->next[pi]->prev[pi] = node;
    path[headpi] = node;
     stimy_echo(updateprop,updateprop(headpi,node));
     stimy_echo(debug,debug(headpi,node,__func__,"more nodes."));
    stimy_post();
}
inline node_t*
addnode(size_t size)
{
    stimy_pre();
    node_t *newhead = path[recpi];
     stimy_echo(printf,printf("%s rechead:%p\n",__func__,(void*)newhead));
    if(newhead &&  stimy_echo(next,next(newhead,recpi))){
        newhead->dimpi = -1;
        newhead->headpi = datpi;
         stimy_echo(unlinknode,unlinknode(recpi,newhead));
         stimy_echo(debug,debug(datpi,newhead,__func__,"recycled"));
        stimy_post(return newhead;);
    }
    if (!(newhead = (node_t *)alocatenode(size)))
        stimy_post(return NULL;);
    newhead->type = size;
    newhead->id = nodeid++;
    newhead->dimpi = -1;
    newhead->headpi = datpi;
    newhead->prop = 0;
    for(int i = 0; i < propsize; i++){
        newhead->next[i] = newhead->prev[i] = NULL;
    }
     stimy_echo(linknode,linknode(datpi,newhead));
    stimy_post(return newhead;);
    stimy_post();
}
void
unlinknode(int headpi,node_t *node)
{
    stimy_pre();
    int pi =  stimy_echo(propi,propi(headpi));
    if(headpi == datpi || !path[headpi] || !node || !node->next[pi]){
        stimy_post(return;);
    }
    if(pi){
        --length[pi] < 0 ? length[pi] = 0 : length[pi];
    }
    if(node == node->next[pi] && node == path[headpi]){
        node->next[pi] = node->prev[pi] = path[headpi] = NULL;
         stimy_echo(updateprop,updateprop(headpi,node));
         stimy_echo(debug,debug(headpi,node,__func__,"empty"));
        stimy_post(return;);
    }
    node->next[pi]->prev[pi] = node->prev[pi];
    node->prev[pi]->next[pi] = node->next[pi];
    if(path[headpi] == node)
        path[headpi] = node->next[pi];
    node->next[pi] = node->prev[pi] = NULL;
     stimy_echo(updateprop,updateprop(headpi,node));
     stimy_echo(debug,debug(headpi,node,__func__,""));
    stimy_post();
}
inline void
denode(node_t *node)
{
    stimy_pre();
    if (!path[datpi] || !node ||  stimy_echo(next,next(node,recpi)))
        stimy_post(return;);
     stimy_echo(debug2,debug2(__func__,"start"));
     stimy_echo(unlinknode,unlinknode(node->dimpi,node));
    node->dimpi = -1;
    node->headpi = datpi;
    for(int headpi = winpi; headpi <= pospi; headpi++){
         stimy_echo(unlinknode,unlinknode(headpi,node));
    }
     stimy_echo(linknode,linknode(recpi,node));
     stimy_echo(debug2,debug2(__func__,"ended"));
    stimy_post();
}
void
delall(void)
{
    stimy_pre();
     stimy_echo(countmem,countmem(__func__,"start"));
    node_t *tail = NULL;
    node_t *head = path[datpi];
    int pi =  stimy_echo(propi,propi(datpi));
    if(!head)
        stimy_post(return;);
    for(int i = 0; i < length[nodepath]; i++){
         if(!(tail = head))
            continue;
         head = head->next[pi];
          stimy_echo(delocatenode,delocatenode(tail));
    }
    *path = NULL;
     stimy_echo(countmem,countmem(__func__,"ended"));
    stimy_post();
}
void
showpath(int headpi)
{
    stimy_pre();
    int pi =  stimy_echo(propi,propi(headpi));
    node_t *node = path[headpi];
    if(!node)
        stimy_post(return;);
    do{
         stimy_echo(debug,debug(headpi,node,__func__,""));
    }while((node = node->prev[pi]) && node != path[headpi]);
    stimy_post();
}
void
debug2(const char *funname,char *mesg)
{
    stimy_pre();
    if(!debugging)
        stimy_post(return;);
     stimy_echo(printf,printf("%-14s %s\n",funname,mesg));
    stimy_post();
}
void
debug(int headpi,node_t *node,const char *funname, char *mesg)
{
    stimy_pre();
    if(!debugging)
        stimy_post(return;);
    int len = 20;
    char piname[len];
    char propname[len];
    for(int i = 0; i <  len; i++){
        propname[i] = piname[i] = '\0';
    }
    switch(headpi){
        case dimpi:  stimy_echo(snprintf,snprintf(piname,6,"%.*s",5,"dimpi"));break;
        case datpi:  stimy_echo(snprintf,snprintf(piname,6,"%.*s",5,"datpi"));break;
        case recpi:  stimy_echo(snprintf,snprintf(piname,6,"%.*s",5,"recpi"));break;
        case winpi:  stimy_echo(snprintf,snprintf(piname,6,"%.*s",5,"winpi"));break;
        case monpi:  stimy_echo(snprintf,snprintf(piname,6,"%.*s",5,"monpi"));break;
        case selpi:  stimy_echo(snprintf,snprintf(piname,6,"%.*s",5,"selpi"));break;
        case unkpi:  stimy_echo(snprintf,snprintf(piname,6,"%.*s",5,"unkpi"));break;
        case fixpi:  stimy_echo(snprintf,snprintf(piname,6,"%.*s",5,"fixpi"));break;
        case icopi:  stimy_echo(snprintf,snprintf(piname,6,"%.*s",5,"icopi"));break;
        case pospi:  stimy_echo(snprintf,snprintf(piname,6,"%.*s",5,"pospi"));break;
        default:  stimy_echo(snprintf,snprintf(piname,6,"%.*s",5,"dimpi"));break;
    }
    switch(node->prop){
      case dim:  stimy_echo(snprintf,snprintf(propname,8,"%.*s",7,"dim"));break;
      case dat:  stimy_echo(snprintf,snprintf(propname,8,"%.*s",7,"dat"));break;
      case datdim:  stimy_echo(snprintf,snprintf(propname,8,"%.*s",7,"datdim"));break;
      case recdat:  stimy_echo(snprintf,snprintf(propname,11,"%.*s",10,"recdat"));break;
      case windat:  stimy_echo(snprintf,snprintf(propname,11,"%.*s",10,"windat"));break;
      case mondat:  stimy_echo(snprintf,snprintf(propname,11,"%.*s",10,"mondat"));break;
      case seldat:  stimy_echo(snprintf,snprintf(propname,11,"%.*s",10,"seldat"));break;
      case fixdat:  stimy_echo(snprintf,snprintf(propname,11,"%.*s",10,"fixdat"));break;
      case recdatdim:  stimy_echo(snprintf,snprintf(propname,14,"%.*s",13,"recdatdim"));break;
      case winrecdat:  stimy_echo(snprintf,snprintf(propname,14,"%.*s",13,"winrecdat"));break;
      case windatdim:  stimy_echo(snprintf,snprintf(propname,14,"%.*s",13,"windatdim"));break;
      case mondatdim:  stimy_echo(snprintf,snprintf(propname,14,"%.*s",13,"mondatdim"));break;
      case selwindat:  stimy_echo(snprintf,snprintf(propname,14,"%.*s",13,"selwindat"));break;
      case fixwindat:  stimy_echo(snprintf,snprintf(propname,14,"%.*s",13,"fixwindat"));break;
      case icowindat:  stimy_echo(snprintf,snprintf(propname,14,"%.*s",13,"icowindat"));break;
      case selrecdat:  stimy_echo(snprintf,snprintf(propname,14,"%.*s",13,"selrecdat"));break;
      case selwindatdim:  stimy_echo(snprintf,snprintf(propname,17,"%.*s",16,"selwindatdim"));break;
      case unkwindatdim:  stimy_echo(snprintf,snprintf(propname,17,"%.*s",16,"unkwindatdim"));break;
      case fixwindatdim:  stimy_echo(snprintf,snprintf(propname,17,"%.*s",16,"fixwindatdim"));break;
      case icowindatdim:  stimy_echo(snprintf,snprintf(propname,17,"%.*s",16,"icowindatdim"));break;
      case icoselwindat:  stimy_echo(snprintf,snprintf(propname,17,"%.*s",16,"icoselwindat"));break;
      case fixselwindat:  stimy_echo(snprintf,snprintf(propname,17,"%.*s",16,"fixselwindat"));break;
      case fixselwindatdim:  stimy_echo(snprintf,snprintf(propname,20,"%.*s",19,"fixselwindatdim"));break;
      case icoselwindatdim:  stimy_echo(snprintf,snprintf(propname,20,"%.*s",19,"icoselwindatdim"));break;
      default:  stimy_echo(sprintf,sprintf(propname,"%d",node->prop));break;
    }
     stimy_echo(printf,printf("%-14s %-10s:%2d:%2d:%2d nodeid:%2d %20s %s\n",funname,
        piname,moni(node->dimpi),wsi(node->dimpi),
         stimy_echo(loci,loci(node->dimpi)),node->id,propname,mesg));
    stimy_post();
}
