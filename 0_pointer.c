#include <stdio.h>
int main(void)
{
    int *px, x{ 10 };
    px = &x;
    printf(x);
    printf(px);
    return 0;
}