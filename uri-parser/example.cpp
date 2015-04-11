#include <iostream>
#include <UriParser.hpp>

int main(int argc, const char *argv[])
{
    std::string temp;
    std::string haystack = "http://user:password@www.google.com:80/path?search";
    for (int i = 0; i < 1; i++) {
        http::url parsed = http::ParseHttpUrl(haystack);
        temp = parsed.host;
        std::cout << parsed.host << " " <<  parsed.user;
    }
    std::cout << std::endl;

    return 0;
}
