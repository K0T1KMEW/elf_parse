#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>

struct Elf64_Ehdr {
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

struct Elf64_Phdr {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
};

struct Elf64_Dyn {
    int64_t d_tag;
    union {
        uint64_t d_val;
        uint64_t d_ptr;
    } d_un;
};

struct Elf64_Sym {
    uint32_t st_name;
    uint8_t  st_info;
    uint8_t  st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
};

#define ELF64_ST_TYPE(info) ((info) & 0xf)
#define ELF64_ST_BIND(info) ((info) >> 4)

void printELFHeader(std::ifstream& file, const Elf64_Ehdr& ehdr);
void printImportedFunctions(std::ifstream& file, const Elf64_Ehdr& ehdr);