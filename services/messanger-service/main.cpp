#include "http-messanger-service.h"

int main(int argc, char *argv[])
{
    HTTPMessangerService messangerService;
    return messangerService.run(argc, argv);
}