
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct student stu;

//struct student { // stu size is 32
//    int          age;
//    u_char       *name;
//    stu          *next;
//    unsigned int is_local:1;
//};

struct student { // stu size is still 32
    int          age;
    u_char       *name;
    stu          *next;
    unsigned int is_local:1;
    unsigned int gender:1;
};

//struct student { // stu size is still 32
//    unsigned int gender:1;
//    int          age;
//    u_char       *name;
//    stu          *next;
//    unsigned int is_local:1;
//};


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
    printf("size of p->age is %zu\n", sizeof(p->age));
    printf("size of p->name is %zu\n", sizeof(p->name));
    printf("size of p->next is %zu\n", sizeof(p->next));
    printf("size of unsigned int is %zu\n", sizeof(unsigned int));

    stu *p2 = malloc(pool_size);
    p->name = (u_char *) malloc(sizeof(NAME));
    strcpy(p->name, NAME);
    p->next     = NULL;
    p->age      = 21;
    p->is_local = 0;

    return 0;
}

