#include <stdio.h>

#define GAMEAPI __declspec(dllexport)

GAMEAPI void Run()
{
    printf("Baa!\n");
}