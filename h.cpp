#include "h.h"

void printELFHeader(std::ifstream& file, const Elf64_Ehdr& ehdr) {
    std::cout << "ELF Header:\n";
    std::cout << "  Magic:   ";
    for (int i = 0; i < 16; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(ehdr.e_ident[i]) << " ";
    }
    std::cout << "\n";
    std::cout << "  Class:                             " << (ehdr.e_ident[4] == 2 ? "ELF64" : "ELF32") << "\n";
    std::cout << "  Data:                              " << (ehdr.e_ident[5] == 1 ? "little endian" : "big endian") << "\n";
    std::cout << "  Version:                           " << static_cast<int>(ehdr.e_ident[6]) << "\n";
    std::cout << "  OS/ABI:                            " << static_cast<int>(ehdr.e_ident[7]) << "\n";
    std::cout << "  ABI Version:                       " << static_cast<int>(ehdr.e_ident[8]) << "\n";
    std::cout << "  Type:                              " << ehdr.e_type << "\n";
    std::cout << "  Machine:                           " << ehdr.e_machine << "\n";
    std::cout << "  Version:                           " << ehdr.e_version << "\n";
    std::cout << "  Entry point address:               0x" << std::hex << ehdr.e_entry << "\n";
    std::cout << "  Start of program headers:          " << ehdr.e_phoff << "\n";
    std::cout << "  Start of section headers:          " << ehdr.e_shoff << "\n";
    std::cout << "  Flags:                             " << ehdr.e_flags << "\n";
    std::cout << "  Size of this header:               " << ehdr.e_ehsize << "\n";
    std::cout << "  Size of program header entry:      " << ehdr.e_phentsize << "\n";
    std::cout << "  Number of program headers:         " << ehdr.e_phnum << "\n";
    std::cout << "  Size of section headers:           " << ehdr.e_shentsize << "\n";
    std::cout << "  Number of section headers:         " << ehdr.e_shnum << "\n";
    std::cout << "  Section header string table index: " << ehdr.e_shstrndx << "\n";
}

void printImportedFunctions(std::ifstream& file, const Elf64_Ehdr& ehdr) {
    file.seekg(ehdr.e_phoff, std::ios::beg);

    uint64_t strtabOffset = 0;
    uint64_t symtabOffset = 0;
    uint64_t symtabSize = 0;
    std::vector<uint64_t> neededLibraries;  // Смещения для библиотек

    for (int i = 0; i < ehdr.e_phnum; ++i) {
        Elf64_Phdr phdr;
        file.read(reinterpret_cast<char*>(&phdr), sizeof(phdr));

        if (phdr.p_type == 2) {  // PT_DYNAMIC
            file.seekg(phdr.p_offset, std::ios::beg);
            std::vector<Elf64_Dyn> dynamicEntries;

            while (true) {
                Elf64_Dyn dyn;
                file.read(reinterpret_cast<char*>(&dyn), sizeof(dyn));
                if (dyn.d_tag == 0) {  // DT_NULL
                    break;
                }
                dynamicEntries.push_back(dyn);
            }

            // Ищем смещения для таблиц символов и строк, а также нужные библиотеки
            for (const auto& dyn : dynamicEntries) {
                if (dyn.d_tag == 5) {  // DT_STRTAB
                    strtabOffset = dyn.d_un.d_ptr;
                }
                else if (dyn.d_tag == 6) {  // DT_SYMTAB
                    symtabOffset = dyn.d_un.d_ptr;
                }
                else if (dyn.d_tag == 10) {  // DT_SYMENT
                    symtabSize = dyn.d_un.d_val;
                }
                else if (dyn.d_tag == 1) {  // DT_NEEDED
                    neededLibraries.push_back(dyn.d_un.d_val);
                }
            }
            break;
        }
    }

    // Выводим библиотеки
    std::cout << "\nImported libraries:\n";
    for (auto offset : neededLibraries) {
        std::string libName;
        file.seekg(strtabOffset + offset, std::ios::beg);
        char ch;
        while (file.get(ch) && ch != '\0') {
            libName += ch;
        }
        if (!libName.empty()) {
            std::cout << "Library: " << libName << "\n";
            // Выводим импортируемые функции
            if (symtabOffset != 0 && strtabOffset != 0) {
                file.seekg(symtabOffset, std::ios::beg);
                std::vector<Elf64_Sym> symbols;
                while (file.tellg() < symtabOffset + symtabSize) {
                    Elf64_Sym sym;
                    file.read(reinterpret_cast<char*>(&sym), sizeof(sym));
                    symbols.push_back(sym);
                }

                std::cout << "\nImported functions:\n";
                for (const auto& sym : symbols) {
                    if (ELF64_ST_TYPE(sym.st_info) == 2) {  // STT_FUNC
                        std::string funcName;
                        file.seekg(strtabOffset + sym.st_name, std::ios::beg);
                        char ch;
                        while (file.get(ch) && ch != '\0') {
                            funcName += ch;
                        }
                        if (!funcName.empty()) {
                            std::cout << "Function: " << funcName << "\n";
                        }
                    }
                }
            }
            else {
                std::cout << "Symbol table or string table not found.\n";
            }
        }
    }

    
}
