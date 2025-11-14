#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


int encode_varint(uint64_t v, unsigned char *out)
{
    unsigned char tmp[9];
    int n = 0;

    do
    {
        tmp[n++] = v & 0x7F;
        v >>= 7;
    } while (v);

    int outn = 0;

    for (int i = n - 1; i >= 0; i--)
    {
        unsigned char b = tmp[i];

        if (i != 0)
            b |= 0x80;

        out[outn++] = b;
    }

    return outn;
}

int write_be32(unsigned char *p, uint32_t v)
{
    p[0] = (v >> 24) & 0xFF;
    p[1] = (v >> 16) & 0xFF;
    p[2] = (v >> 8) & 0xFF;
    p[3] = v & 0xFF;

    return 4;
}

int write_be16(unsigned char *p, uint16_t v)
{
    p[0] = (v >> 8) & 0xFF;
    p[1] = v & 0xFF;

    return 2;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <out.sqlite>\n", argv[0]);
        return 2;
    }

    const char *fname = argv[1];
    const int page_size = 1024;
    unsigned char page[1024];

    memset(page, 0, page_size);

    memcpy(page, "SQLite format 3\0", 16);
    page[16] = (page_size >> 8) & 0xFF;
    page[17] = page_size & 0xFF;
    page[18] = 1;
    page[19] = 1;
    page[20] = 0;
    page[21] = 64;
    page[22] = 32;
    page[23] = 32;

    page[56] = 0;
    page[57] = 0;
    page[58] = 0;
    page[59] = 1;

    const char *type_txt = "table";
    const char *name_txt = "mytable";
    const char *tblname = "mytable";
    const char *sql_txt = "CREATE TABLE mytable(id INTEGER PRIMARY KEY, name TEXT)";
    unsigned char rec_body[1024];
    int rbody = 0;

    int type_len = strlen(type_txt);
    memcpy(rec_body + rbody, type_txt, type_len);
    rbody += type_len;
    int name_len = strlen(name_txt);
    memcpy(rec_body + rbody, name_txt, name_len);
    rbody += name_len;
    int tbl_len = strlen(tblname);
    memcpy(rec_body + rbody, tblname, tbl_len);
    rbody += tbl_len;

    unsigned char rootpage_val = 2;
    memcpy(rec_body + rbody, &rootpage_val, 1);
    rbody += 1;
    int sql_len = strlen(sql_txt);
    memcpy(rec_body + rbody, sql_txt, sql_len);
    rbody += sql_len;

    unsigned char record[1024];
    int rpos = 0;

    rpos = 1;

    int s1 = 13 + 2 * type_len;
    record[rpos++] = s1;
    int s2 = 13 + 2 * name_len;
    record[rpos++] = s2;
    int s3 = 13 + 2 * tbl_len;
    record[rpos++] = s3;
    int s4 = 1;
    record[rpos++] = s4;
    int s5 = 13 + 2 * sql_len;
    record[rpos++] = s5;

    int header_size = rpos;
    record[0] = header_size;

    memcpy(record + rpos, type_txt, type_len);
    rpos += type_len;
    memcpy(record + rpos, name_txt, name_len);
    rpos += name_len;
    memcpy(record + rpos, tblname, tbl_len);
    rpos += tbl_len;

    record[rpos++] = 2;
    memcpy(record + rpos, sql_txt, sql_len);
    rpos += sql_len;
    int record_total = rpos;

    unsigned char cell[2048];
    int cpos = 0;
    unsigned char vtmp[16];
    int vsz = encode_varint(record_total, vtmp);
    memcpy(cell + cpos, vtmp, vsz);
    cpos += vsz;
    int v2sz = encode_varint(1, vtmp);
    memcpy(cell + cpos, vtmp, v2sz);
    cpos += v2sz;
    memcpy(cell + cpos, record, record_total);
    cpos += record_total;
    int cell_len = cpos;

    int bh = 100;
    page[bh + 0] = 0x0D;
    page[bh + 1] = 0;
    page[bh + 2] = 0;
    page[bh + 3] = 0;
    page[bh + 4] = 1;

    int cell_content_start = page_size - cell_len;
    page[bh + 5] = (cell_content_start >> 8) & 0xFF;
    page[bh + 6] = cell_content_start & 0xFF;
    page[bh + 7] = 0;

    int cellptr_off = 100 + 8;
    page[cellptr_off + 0] = (cell_content_start >> 8) & 0xFF;
    page[cellptr_off + 1] = cell_content_start & 0xFF;
    memcpy(page + cell_content_start, cell, cell_len);

    FILE *out = fopen(fname, "wb");
    if (!out)
    {
        perror("open");
        return 1;
    }
    if (fwrite(page, 1, page_size, out) != page_size)
    {
        perror("write page1");
        fclose(out);
        return 1;
    }

    memset(page, 0, page_size);

    page[0] = 0x0D;
    page[1] = 0;
    page[2] = 0;
    page[3] = 0;
    page[4] = 2;

    unsigned char cellA[512];

    unsigned char recA[256];
    int rp = 0;
    rp = 1;

    const char *nameA = "Alice";
    int na = strlen(nameA);
    int st0 = 0;
    int st1 = 13 + 2 * na;
    recA[rp++] = st0;
    recA[rp++] = st1;
    recA[0] = rp;
    memcpy(recA + rp, nameA, na);
    rp += na;
    int recA_total = rp;

    int cposA = 0;
    cposA += encode_varint(recA_total, cellA + cposA);
    cposA += encode_varint(1, cellA + cposA);
    memcpy(cellA + cposA, recA, recA_total);
    cposA += recA_total;

    unsigned char cellB[512];
    int bpos = 0;
    unsigned char recB[256];
    int rpb = 0;
    rpb = 1;
    const char *nameB = "Bob";
    int nb = strlen(nameB);
    int stb0 = 0;
    int stb1 = 13 + 2 * nb;
    recB[rpb++] = stb0;
    recB[rpb++] = stb1;
    recB[0] = rpb;
    memcpy(recB + rpb, nameB, nb);
    rpb += nb;
    int recB_total = rpb;
    bpos += encode_varint(recB_total, cellB + bpos);
    bpos += encode_varint(2, cellB + bpos);
    memcpy(cellB + bpos, recB, recB_total);
    bpos += recB_total;

    int total_cells = cposA + bpos;
    int start = page_size - total_cells;

    int offA = start;
    memcpy(page + offA, cellA, cposA);
    int offB = start + cposA;
    memcpy(page + offB, cellB, bpos);

    page[8] = (offA >> 8) & 0xFF;
    page[9] = offA & 0xFF;
    page[10] = (offB >> 8) & 0xFF;
    page[11] = offB & 0xFF;

    int content_start = start;
    page[5] = (content_start >> 8) & 0xFF;
    page[6] = content_start & 0xFF;

    if (fwrite(page, 1, page_size, out) != page_size)
    {
        perror("write page2");
        fclose(out);
        return 1;
    }

    fclose(out);
    printf("Wrote sqlite db to %s\n", fname);

    return 0;
}
