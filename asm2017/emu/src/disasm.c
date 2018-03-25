#include <disasm.h>
#include <util.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define    r4(n)    n, n, n, n
#define    r8(n)    r4(n), r4(n)

/* Associate a unique instruction id to all sequences of 7 bits. This table
   avoids having to read 4 bits, test if it's a valid instruction (Huffman
   encoding), if not, read another bit, test again... until 7 bits.
   This is the default encoding -- ids will store the encoding default or not */
static const uint8_t default_ids[128] = {
        r8(0), r8(1), r8(2), r8(3), r8(4), r8(5), r8(6), r8(7),
        r8(8), r4(9), r4(10), r8(11), r8(12),
        13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
};
/* Indicate the length of each unique instruction id. */
static uint8_t length[37] = {
        r8(4), 4, 5, 5, 4, 4, r8(6), r8(7), r8(7)
};

static uint8_t opcode_size = 7;
static uint8_t *ids;


/* Instruction set. The entry at each unique id represents the associated
   instruction, with a predefined format. The first three letters are elements
   of the arg_t enumeration and represent the instruction's arguments. The
   fifth letter is an element of the ctgy_t enumeration and classifies the
   instruction. The mnemonic is read starting at index 6. */
static const char instructions[37][16] = {
        "rr- A add2", "rl- A add2i", "rr- A sub2", "rl- A sub2i",
        "rr- T cmp", "rc- T cmpi", "rr- L let", "rc- L leti",
        "drh A shift", "psr M readze", "psr M readse", "a-- J jump",
        "oa- J jumpif", "rr- A or2", "rl- A or2i", "rr- A and2",
        "rl- A and2i", "psr M write", "a-- J call", "pr- C setctr",
        "pr- C getctr", "sr- M push", "--- J return", "rrr A add3",
        "rrl A add3i", "rrr A sub3", "rrl A sub3i", "rrr A and3",
        "rrl A and3i", "rrr A or3", "rrl A or3i", "rrr A xor3",
        "rrl A xor3i", "rrh A asr3", "l-- C sleep", "r-- A rand",
        "--- C (res)",
};

/*
   The number of bits reserved to give information about the arguments and to
   classify the instructions. The real name of the instruction starts at the
   bit with the number INSTR_INFORMATION_BITS.
   If anybody wants to change the information about instructions, the value
   of this macro should be modified consequently.
*/
#define INSTR_INFORMATION_BITS (6)

/* load_encoding() -- load a huffman encoding as instruction set
   Loads the default encoding if "filename" is NULL. Returns non-zero
   on failure */
uint load_encoding(const char *filename)
{
    /* If filename is not null we load the associated file and build the encoding */
    if (filename)
    {
        FILE *huffman_f = fopen(filename, "r");

        if(huffman_f)
        {
            /* Read the size of the larger opcode */
            fscanf(huffman_f, "%u", &opcode_size);

            /* Allocate an array able to store any opcode */
            ids = (uint8_t *) malloc(sizeof(uint8_t) * (1 << opcode_size));

            char mnemonic[64];
            uint8_t iid;
            uint8_t size;
            uint64_t opcode;

            for (int i = 0; i < 36; i++)
            {
                fscanf(huffman_f, "%s %u %u %u", &mnemonic, &iid, &size, &opcode);

                /* We build the integers linked to the opcode */
                opcode = opcode << (opcode_size - size);

                for (int j = 0; j < (1 << (opcode_size - size)); j++)
                    ids[opcode + j] = iid;

                /* Set the size of that opcode */
                length[iid] = size;

            }

            fclose(huffman_f);
        }
        else
            return 1;
    }

        /* Loading default encoding */
    else
        ids = (uint8_t *) default_ids;

    return 0;

}

/* disasm_opcode() -- read an instruction code */
uint disasm_opcode(memory_t *mem, uint64_t *ptr, const char **format)
{
    uint opcode = memory_read(mem, *ptr, opcode_size) & 0x7f;
    uint id = ids[opcode & 0x7f];

    *ptr += length[id];
    if (format) *format = instructions[id];

    return id;
}

/* disasm_format() -- get the format for a given instruction */
const char *disasm_format(uint opcode)
{
    if (opcode >= DISASM_INS_COUNT) return NULL;
    return instructions[opcode];
}

/* disasm_reg() -- read a register number */
uint disasm_reg(memory_t *mem, uint64_t *ptr)
{
    *ptr += 3;
    return memory_read(mem, *ptr - 3, 3);
}

/* disasm_dir() -- read a shift direction bit */
uint disasm_dir(memory_t *mem, uint64_t *ptr)
{
    *ptr += 1;
    return memory_read(mem, *ptr - 1, 1);
}

/* disasm_cond() -- read a jump condition type */
uint disasm_cond(memory_t *mem, uint64_t *ptr)
{
    *ptr += 3;
    return memory_read(mem, *ptr - 3, 3);
}

/* disasm_addr() -- read a relative address */
int64_t disasm_addr(memory_t *mem, uint64_t *ptr, uint *size_arg)
{
    /* Length of header, size of address, 3 header bits of address */
    uint offset = 1, size = 8;
    uint head = memory_read(mem, *ptr, 3);

    /* Headers of size 2 start with 10, which allows 100 and 101 */
    if (head == 4 || head == 5) offset = 2, size = 16;
        /* Headers of size 3 only include 110 and 111 */
    else if (head >= 6) offset = 3, size = 1 << (head - 1);

    *ptr += offset + size;
    if (size_arg) *size_arg = size;
    uint64_t addr = memory_read(mem, *ptr - size, size);

    return sign_extend(addr, size);
}

/* disasm_lconst() -- read a zero-extended constant */
uint64_t disasm_lconst(memory_t *mem, uint64_t *ptr, uint *size_arg)
{
    /* Length of header, size of constant, 3 header bits of constant */
    uint offset = 1, size = 1;
    uint head = memory_read(mem, *ptr, 3);

    /* Headers of size 2 start with 10, which allows 100 and 101 */
    if (head == 4 || head == 5) offset = 2, size = 8;
        /* Headers of size 3 only include 110 and 111 */
    else if (head >= 6) offset = 3, size = 1 << (head - 1);

    *ptr += offset + size;
    if (size_arg) *size_arg = size;

    uint64_t t = memory_read(mem, *ptr - size, size);
    return t;
}

/* disasm_aconst() -- read a sign-extended constant */
int64_t disasm_aconst(memory_t *mem, uint64_t *ptr, uint *size_arg)
{
    uint size;

    /* Let disasm_lconst() do the bit retrieval job for us */
    uint64_t cst = disasm_lconst(mem, ptr, &size);
    if (size_arg) *size_arg = size;

    return sign_extend(cst, size);
}

/* disasm_shift() -- read a shift constant */
uint disasm_shift(memory_t *mem, uint64_t *ptr)
{
    uint shift = memory_read(mem, *ptr, 7);

    /* If the first bit is set, then the value was just 1 */
    if (shift & 0x40) shift = 1, (*ptr)++;
        /* Otherwise, we have 6 bits which represent the shift value */
    else *ptr += 7;

    return shift;
}

/* disasm_size() -- read a memory operation size */
uint disasm_size(memory_t *mem, uint64_t *ptr)
{
    uint size = memory_read(mem, *ptr, 3);
    /* Values lower than 4 mean 00x or 01x, thus sizes of length 2 */
    *ptr += 3 - (size < 4);

    /* Cheap formulas to get the size "efficiently" */
    if (size < 4) return 1 + 3 * (size >> 1);
    return 1 << (size - 1);
}

/* disasm_pointer() -- read a pointer id */
uint disasm_pointer(memory_t *mem, uint64_t *ptr)
{
    *ptr += 2;
    return memory_read(mem, *ptr - 2, 2);
}

uint disasm_instr_length(uint id)
{
    return length[id];
}

char *disasm_instruction_name(uint id)
{
    return &instructions[id][INSTR_INFORMATION_BITS];
}
