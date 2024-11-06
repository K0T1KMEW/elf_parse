#include <iostream>
#include <fstream>

#include "h.h"

int main() {
    std::string filename = "libstdc++.so.6.0.26";
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }
    
    Elf64_Ehdr ehdr;
    file.read(reinterpret_cast<char*>(&ehdr), sizeof(ehdr));

    printELFHeader(file, ehdr);

    printImportedFunctions(file, ehdr);

    return 0;
}