#include "inc/args.h"

int main(int argc, char *argv[])
{
    t_args test;
    checkArguments(argc, argv, &test);
    for (int i = 0; i < IMPLEMENTED_OPTIONS; i++)
    {
        printf("%d\n", test.activatedOptions[i]);
    }
}