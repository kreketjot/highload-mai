#include "http-user-service.h"

int main(int argc, char *argv[])
{
    HTTPUserService userService;
    return userService.run(argc, argv);
}