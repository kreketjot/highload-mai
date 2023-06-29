#include "http-blog-service.h"

int main(int argc, char *argv[])
{
    HTTPBlogService blogService;
    return blogService.run(argc, argv);
}