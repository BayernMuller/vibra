#include "cli/cli.h"

int main(int argc, char** argv)
{
    CLIMain cli(argc, argv);
    cli.Run();
    return 0;
}