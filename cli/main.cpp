#include "cli.h"

int main(int argc, char** argv)
{
    CLI cli(argc, argv);
    cli.Run();
    return 0;
}