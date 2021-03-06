#ifndef STIMY_H
#include <stimy.h>
#endif
#ifndef CUBE_HEADER
#define CUBE_HEADER
#endif
#include <stdio.h>
#include <unistd.h>
enum {
    CUBE_A=2,
    CUBE_B=10,
    CUBE_C=2,
    CUBE_DIM=3,
    CUBE_LEN = (CUBE_A * CUBE_B * CUBE_C)
};
#define cube_ai(x) ((x) < CUBE_LEN ? ((x)/(CUBE_A*CUBE_B)) : -1)
#define cube_bi(x) ((x) < CUBE_LEN ? (x)%(CUBE_A*CUBE_B)/(CUBE_C) : -1)
#define cube_ci(x) ((x) < CUBE_LEN ? ((x)%(CUBE_A*CUBE_B)%(CUBE_C)) : -1)
static int cube[CUBE_A][CUBE_B][CUBE_C];
inline static void cube_init(void);
inline static void cube_print(void);
inline static void
cube_init(void)
{
    stimy_pre();
    int counter = 0;
    for (int a = 0; a < CUBE_A; a++){
        for (int b = 0; b < CUBE_B; b++){
            for (int c = 0; c < CUBE_C; c++){
                cube[a][b][c] = counter++;
            }
        }
    }
    stimy_post();
}
inline static void
cube_print(void)
{
    stimy_pre();
    for (int a = 0; a < CUBE_A; a++){
        for (int b = 0; b < CUBE_B; b++){
            for (int c = 0; c < CUBE_C; c++){
                 stimy_echo(printf,printf("a:%d b:%d c:%d | %d\n",a,b,c,cube[a][b][c]));
            }
        }
    }
     stimy_echo(printf,printf("%s\n",""));
     stimy_echo(printf,printf("cube len: %d\n",CUBE_LEN));
    stimy_post();
}
