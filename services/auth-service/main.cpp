#include "http-auth-service.h"

int main(int argc, char *argv[])
{
    HTTPAuthService authService;
    return authService.run(argc, argv);
}