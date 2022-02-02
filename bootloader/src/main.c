#include <uefi.h>

int main(int argc, char **argv)
{
    ST->ConOut->ClearScreen(ST->ConOut);
    printf("SmplosV3.1 Loader started\n");
    getchar();
    return 0;
}
