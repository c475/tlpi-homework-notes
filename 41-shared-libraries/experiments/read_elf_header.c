/*
    This program only reads the initial ELF header of an object file.
    It does not go into other stuff like Section and Program headers,
    dynamic linking, or symbol location etc
*/

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// #include "../../lib/tlpi_hdr.h"


// e_type (type of file)
#define ET_NONE         0 // no file type
#define ET_REL          1 // Relocatable file
#define ET_EXEC         2 // executable file
#define ET_DYN          3 // shared object file
#define ET_CORE         4 // corefile
#define ET_LOPROC       0xFF00 // Processor-specific
#define ET_HIPROC       0xFFFF // Processor-specific

// e_machine (type of machine)
#define EM_NONE         0 // No machine
#define EM_M32          1 // AT&T WE 32100
#define EM_SPARC        2 // SPARC
#define EM_386          3 // Intel 80386
#define EM_68K          4 // Motorola 68000
#define EM_88K          5 // Motorola 88000
#define EM_860          7 // Intel 80860
#define EM_MIPS         8 // MIPS RS3000

// e_ident index values
#define EI_MAG0         0 // file identification (0x7F e_ident[EI_MAG0])
#define EI_MAG1         1 // file identification ('E'  e_ident[EI_MAG1])
#define EI_MAG2         2 // file identification ('L'  e_ident[EI_MAG2])
#define EI_MAG3         3 // file identification ('F'  e_ident[EI_MAG3])

#define EI_CLASS            4 // file class
    #define ELFCLASSNONE    0 // Invalid class
    #define ELFCLASS32      1 // 32-bit objects
    #define ELFCLASS64      2 // 64-bit objects

#define EI_DATA         5 // data encoding
    #define ELFDATANONE     0 // Invalid data encoding

    // 0x01         [0x01]
    // 0x0102       [0x02, 0x01]
    // 0x01020304   [0x04, 0x03, 0x02, 0x01]
    #define ELFDATA2LSB     1 // object data is least significant bit (2's compliment)

    // same as above but in reverse
    #define ELFDATA2MSB     2 // object data is most significant bit (2's compliment)

#define EI_VERSION      6 // file version
    #define EV_NONE         0 // Invalid version
    #define EV_CURRENT      1 // Current version

#define EI_PAD          7 // start of padding bytes (to EI_NIDENT-1)
#define EI_NIDENT       16 // size of e_ident


typedef uint32_t        ELF_32_ADDR;
typedef uint16_t        ELF_32_HALF;
typedef uint32_t        ELF_32_OFF;
typedef int32_t         ELF_32_SWORD;
typedef uint32_t        ELF_32_WORD;
typedef unsigned char   ELF_CHAR;


typedef struct {

    ELF_CHAR e_ident[EI_NIDENT];

    ELF_32_HALF e_type;
    ELF_32_HALF e_machine;

    // EV_CURRENT
    ELF_32_WORD e_version;

    // Virtual address to which the system first transfers control, thus starting the process
    // If the file has no program header table, this member holds zero
    ELF_32_ADDR e_entry;

    // The PROGRAM header table's file offset in bytes
    // If there is no PROGRAM header table, this field is zero
    ELF_32_OFF e_phoff;

    // The SECTION header table's file offset in bytes
    // If there is no SECTION header table, this field is zero
    ELF_32_OFF e_shoff;

    // Process specific flags
    // 0 on Intel architecture (intel architecture defines no flags)
    ELF_32_WORD e_flags;

    // ELF header's size in bytes
    ELF_32_HALF e_ehsize;

    // Size of an entry in bytes of the PROGRAM header table
    ELF_32_HALF e_phentsize;

    // Number of entries in the PROGRAM header table
    // e_phnum * e_phentsize = size of the entire PROGRAM header table
    ELF_32_HALF e_phnum;

    // Size of an entry in bytes of the SECTION header table
    ELF_32_HALF e_shentsize;

    // Number of entries in the SECTION header table
    // e_phnum * e_phentsize = size of the entire SECTION header table
    ELF_32_HALF e_shnum;


    ELF_32_HALF e_shstrndx;

} ELF_HEADER;


int elf_check_preamble(ELF_HEADER *header)
{
    if (!header) {
        return -1;
    }

    return header->e_ident[EI_MAG0] == 0x7F &&
        header->e_ident[EI_MAG1] == 'E' &&
        header->e_ident[EI_MAG2] == 'L' &&
        header->e_ident[EI_MAG3] == 'F';
}


int elf_check_type(ELF_HEADER *header)
{
    if (!header) {
        return -1;
    }

    switch (header->e_type) {

        case ET_NONE:
            printf("Type: ET_NONE\n");
            break;

        case ET_REL:
            printf("Type: ET_REL\n");
            break;

        case ET_EXEC:
            printf("Type: ET_EXEC\n");
            break;

        case ET_DYN:
            printf("Type: ET_DYN\n");
            break;

        case ET_CORE:
            printf("Type: ET_CORE\n");
            break;

        case ET_LOPROC:
            printf("Type: ET_LOPROC\n");
            break;

        case ET_HIPROC:
            printf("Type: ET_HIPROC\n");
            break;

        default:
            printf("Type: UNKNOWN\n");
            break;

    }

    return 0;
}


int elf_check_machine(ELF_HEADER *header)
{
    if (!header) {
        return -1;
    }

    switch (header->e_machine) {

        case EM_NONE:
            printf("Machine: ELF_NONE\n");
            break;

        case EM_M32:
            printf("Machine: EM_M32\n");
            break;

        case EM_SPARC:
            printf("Machine: EM_SPARC\n");
            break;

        case EM_386:
            printf("Machine: EM_386\n");
            break;

        case EM_68K:
            printf("Machine: EM_68K\n");
            break;

        case EM_88K:
            printf("Machine: EM_88K\n");
            break;

        case EM_860:
            printf("Machine: EM_860\n");
            break;

        case EM_MIPS:
            printf("Machine: EM_MIPS\n");
            break;

        default:
            printf("Machine: UNKNOWN\n");
            break;
    }

    return 0;
}


int elf_check_version(ELF_HEADER *header)
{
    if (!header) {
        return -1;
    }

    if (header->e_version == EV_CURRENT) {
        printf("Version: EV_CURRENT\n");

    } else {
        printf("Version: EV_NONE\n");
    }

    return 0;
}


int elf_check_entrypoint(ELF_HEADER *header)
{
    if (!header) {
        return -1;
    }

    printf("Entrypoint: %x\n", header->e_entry);

    return 0;
}


void __exit_error(const char *message)
{
    printf("%s\n", message);
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    int fd;
    ELF_HEADER header;

    if (argc < 2) {
        __exit_error("./program object-file\n");
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        __exit_error("open");
    }

    memset(&header, 0, sizeof(ELF_HEADER));

    if (read(fd, &header, sizeof(ELF_HEADER)) != sizeof(ELF_HEADER)) {
        __exit_error("read");
    }

    close(fd);

    if (elf_check_preamble(&header) != 1) {
        printf("Not an object file\n");
        exit(EXIT_FAILURE);
    }

    // if (elf_describe_ident(&header) == -1) {
    //     errExit("elf_describe_ident");
    // }

    if (elf_check_type(&header) == -1) {
        __exit_error("elf_check_type");
    }

    if (elf_check_machine(&header) == -1) {
        __exit_error("elf_check_machine");
    }

    if (elf_check_version(&header) == -1) {
        __exit_error("elf_check_version");
    }

    if (elf_check_entrypoint(&header) == -1) {
        __exit_error("elf_check_entrypoint");
    }

    // won't bother wrapping the rest...
    printf("Program Header Offset: %ld\n", (long)header.e_phoff);
    printf("Section Header Offset: %ld\n", (long)header.e_shoff);
    printf("Processor Flags: %ld\n", (long)header.e_flags);
    printf("ELF Header Size: %ld\n", (long)header.e_ehsize);
    printf("Program Header Entry Size: %ld\n", (long)header.e_phentsize);
    printf("Program Header Entry Number: %ld\n", (long)header.e_phnum);
    printf("Section Header Entry Size: %ld\n", (long)header.e_shentsize);
    printf("Program Header Entry Number: %ld\n", (long)header.e_shnum);
    printf("e_shstrndx: %ld\n", (long)header.e_shstrndx);

    exit(EXIT_SUCCESS);
}
