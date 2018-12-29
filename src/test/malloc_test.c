
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct student stu;

struct student {
    int          age;
    u_char       *name;
    stu          *next;
    unsigned int is_local:1;
};

#define NAME "Mr.Q"

int main() {
    size_t stu_size  = sizeof(stu);
    size_t pool_size = 1024;

    stu *p;
    p = malloc(stu_size);

    p->name = (u_char *) malloc(sizeof(NAME));
    strcpy(p->name, NAME);
    p->next     = NULL;
    p->age      = 27;
    p->is_local = 1;

    printf("stu size is %zu\n", stu_size);

    stu *p2 = malloc(pool_size);
    p->name = (u_char *) malloc(sizeof(NAME));
    strcpy(p->name, NAME);
    p->next     = NULL;
    p->age      = 21;
    p->is_local = 0;

    return 0;
}

