#include <stdio.h>
int main(void)
{
    int *px, x=0;
    px = &x;
    printf(x);
    printf(px);
    return 0;
}