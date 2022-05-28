#include "types.h"
#include "user.h"

int func1(int);
int func2(int);

char d1 = 'a';
int d2[1000];
int d3;
int d4[1000] = {0};

void show_va2pa(void *, char *, char *);

void
show_va2pa(void *va, char *var, char *seg)
{
    int pa = 0;

    pa = va2pa((int) va);
    printf(1, " %p  \t\t  %x\t%s -> %s segment\n", va, pa, var, seg);
}

int
func1(int a)
{
    return a + 1;
}

int
main(int argc, char *argv[])
{
#ifdef VA2PA
    char s1 = 'b';
    char *h1 = malloc(10);
    char *h2 = malloc(10);
    char *h3 = malloc(10000);

    printf(1, "virtual addr\t physical addr\n");

    show_va2pa(main, "main", "code");
    show_va2pa(func1, "func1", "code");
    show_va2pa(func2, "func2", "code");
    show_va2pa(&d1, "d1", "data");
    show_va2pa(&d4, "d4", "data");
    show_va2pa(&d2, "d2", "data");
    show_va2pa(&d3, "d3", "data");

    show_va2pa(&s1, "s1", "stack");
    
    show_va2pa(&h1, "h1", "heap");
    show_va2pa(&h2, "h2", "heap");
    show_va2pa(&h3, "h3", "heap");
#endif // VA2PA
    
    exit();
}

int
func2(int a)
{
    return a + 2;
}
