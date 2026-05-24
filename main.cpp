#include "TimeStamp.hpp"
#include <iostream>

int main()
{
    Timestamp ts = Timestamp::Now();

    std::cout << "toString: " << ts.toString() << std::endl;
    std::cout << "formatted: " << ts.toFormattedString(true) << std::endl;
    std::cout << "filetime: " << ts.toFormattedFile() << std::endl;

    return 0;
}