#include <stdio.h>
#include <stdlib.h>

#include <platform.h>
#include <capstone.h>

struct platform {
    cs_arch arch;
    cs_mode mode;
    unsigned char *code;
    size_t size;
    char *comment;
    cs_opt_type opt_type;
    cs_opt_value opt_value;
};

static void print_string_hex(unsigned char *str, size_t len)
{
    unsigned char *c;

    printf("Code: ");
    for (c = str; c < str + len; c++) {
        printf("0x%02x ", *c & 0xff);
    }
    printf("\n");
}

int main()
{
    #define X86_CODE16 "\x8d\x4c\x32\x08\x01\xd8\x81\xc6\x34\x12\x00\x00"
    #define X86_CODE32 "\x8d\x4c\x32\x08\x01\xd8\x81\xc6\x34\x12\x00\x00\x0f\xa7\xc0"
    #define X86_CODE64 "\x55\x48\x8b\x05\xb8\x13\x00\x00"
    struct platform {
        cs_arch arch;
        cs_mode mode;
        unsigned char *code;
        size_t size;
        char *comment;
        cs_opt_type opt_type;
        cs_opt_value opt_value;
    };
    struct platform platforms[] = {
        {
            CS_ARCH_X86,
            CS_MODE_16,
            (unsigned char*)X86_CODE16,
            sizeof(X86_CODE16) - 1,
            "X86 16bit (Intel syntax)"
        },
        {
            CS_ARCH_X86,
            CS_MODE_32,
            (unsigned char*)X86_CODE32,
            sizeof(X86_CODE32) - 1,
            "X86 32 (Intel syntax)"
        },
        {
            CS_ARCH_X86,
            CS_MODE_64,
            (unsigned char*)X86_CODE64,
            sizeof(X86_CODE64) - 1,
            "X86 64 (Intel syntax)"
        },
    };

    csh handle;
    uint64_t address = 0x1000;
    cs_insn *insn;
    int i;
    size_t count;
    cs_err err;

    for (i = 0; i < sizeof(platforms) / sizeof(platforms[0]); i++) {
        printf("****************\n");
        printf("Platform: %s\n", platforms[i].comment);
        err = cs_open(platforms[i].arch, platforms[i].mode, &handle);
        if (err) {
            printf("Failed on cs_open() with error returned: %u\n", err);
            continue;
        }

        if (platforms[i].opt_type)
            cs_option(handle, platforms[i].opt_type, platforms[i].opt_value);

        count = cs_disasm(handle, platforms[i].code, platforms[i].size, address, 0, &insn);
        if (count) {
            size_t j;

            print_string_hex(platforms[i].code, platforms[i].size);
            printf("Disasm:\n");

            for (j = 0; j < count; j++) {
                printf("0x%" PRIx64 ":\t%s\t\t%s\n",
                    insn[j].address, insn[j].mnemonic, insn[j].op_str);
            }

            // print out the next offset, after the last insn
            printf("0x%" PRIx64 ":\n", insn[j - 1].address + insn[j - 1].size);

            // free memory allocated by cs_disasm()
            cs_free(insn, count);
        }
        else {
            printf("****************\n");
            printf("Platform: %s\n", platforms[i].comment);
            print_string_hex(platforms[i].code, platforms[i].size);
            printf("ERROR: Failed to disasm given code!\n");
        }

        printf("\n");

        cs_close(&handle);
    }
    return 0;
}
