#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


uint64_t decode_varint(const unsigned char *p, int *len_out){
    uint64_t v = 0;
    int i = 0;

    for (; i < 9; ++i)
    {
        unsigned char b = p[i];

        if(i == 8)
        {
            v = (v << 8) | b;
            ++i;
            break;
        }

        v = (v << 7) | (b & 0x7F);
        if (!(b & 0x80)) 
        { 
            ++i; 
            break;
        }
    }

    if(len_out) 
        *len_out = i;

    return v;
}

int64_t read_be_signed(const unsigned char *p, int n)
{
    int64_t v = 0;
    int shift = (8 - n) * 8;

    for(int i = 0; i<n; ++i) 
        v = (v<<8) | p[i];

    return (v << shift) >> shift;
}

void print_record(const unsigned char *record, int rlen)
{
    int hlensz;
    uint64_t header_sz = decode_varint(record, &hlensz);
    int hdrpos = hlensz;

    while (hdrpos < (int) header_sz)
    {
        int stlen;
        decode_varint(record + hdrpos, &stlen);
        hdrpos += stlen;
    }

    int ncols = 0;
    hdrpos = hlensz;
    uint64_t serials[64];
    while(hdrpos < (int) header_sz && ncols < 64)
    {
        int stlen; serials[ncols] = decode_varint(record + hdrpos, &stlen);
        hdrpos += stlen; 
        ++ncols;
    }

    int bodypos = header_sz;
    for(int i = 0; i < ncols; ++i) 
    {
        uint64_t st = serials[i];

        if (!st)
            continue;
        else if (st >= 1 && st <= 6)
        {
            int sz;
            if (st == 1)
                sz = 1;
            else if (st == 2)
                sz = 2;
            else if (st == 3)
                sz = 3;
            else if (st == 4)
                sz = 4;
            else if (st == 5)
                sz = 6;
            else 
                sz = 8;

            int64_t val = read_be_signed(record + bodypos, sz);
            printf("%lld", (long long) val);
            bodypos += sz;
        } 
        else if(st == 7)
        {
            double d;
            memcpy(&d, record + bodypos, 8);
            printf("%g", d);
            bodypos += 8;
        }
        else if (st >= 13 && (st % 2))
        {
            int tlen = (st - 13) / 2;
            printf("\'%.*s\'", tlen, (const char*)(record + bodypos));
            bodypos += tlen;
        }
        else if(st >= 12 && !(st % 2))
        {
            int blen = (st - 12) / 2;
            printf("BLOB(%d)", blen); 
            bodypos += blen;
        }
        else if (st == 8)
            printf("0");
        else if (st == 9)
            printf("1");

        if (i + 1 < ncols)
            printf(" | ");
    }

    printf("\n");
}

int main(int argc, char **argv)
{
    if (argc < 3){
        fprintf(stderr, "Usage: %s <dbfile> <table_name>\n", argv[0]);
        return 2;
    }

    const char *fname = argv[1];
    const char *target = argv[2];
    FILE *f = fopen(fname, "rb");
    if (!f)
    {
        perror("open");
        return 1;
    }

    fseek(f, 0, SEEK_END); 
    long flen = ftell(f);
    fseek(f,0,SEEK_SET);

    unsigned char *buf = malloc(flen);
    if (!buf)
    {
        fclose(f);
        fprintf(stderr,"oom\n"); return 1;
    }

    if (fread(buf, 1, flen, f) != (size_t) flen){
        perror("read");
        fclose(f);
        free(buf);
        return 1;
    }
    fclose(f);

    if (flen < 100)
    {
        fprintf(stderr, "file too small\n");
        free(buf);
        return 1;
    }

    if (memcmp(buf, "SQLite format 3\0", 16) != 0)
    {
        fprintf(stderr, "not sqlite db\n");
        free(buf);
        return 1;
    }

    int page_size = (buf[16] << 8) | buf[17];

    if (page_size == 1)
        page_size = 65536;

    int encoding = (buf[56] << 24) | (buf[57] << 16) | (buf[58] << 8) | buf[59];

    (void)encoding;

    if (flen < page_size)
    {
        fprintf(stderr, "file too small for one page\n");
        free(buf);
        return 1;
    }

    unsigned char *page1 = buf;
    unsigned char *bthead = page1 + 100;
    unsigned char ptype = bthead[0];

    if (ptype != 0x0D)
        fprintf(stderr, "page1 not table leaf (0x0D)\n");

    int cell_count = (bthead[3] << 8) | bthead[4];
    int cellptrs = 100 + 8;

    for (int i = 0; i < cell_count; ++i)
    {
        int ptr = (page1[cellptrs + i * 2] << 8) | page1[cellptrs + i * 2 + 1];

        if (ptr <= 0 || ptr >= page_size)
            continue;

        unsigned char *cell = page1 + ptr;
        int l1;
        uint64_t payload = decode_varint(cell, &l1);
        unsigned char *p = cell + l1;
        int l2;
        decode_varint(p, &l2);
        unsigned char *payloaddata = p + l2;

        int rec_total = (int)payload;
        unsigned char *record = malloc(rec_total);
        if (!record)
            continue;

        memcpy(record, payloaddata, rec_total);

        int tmp_hlen;
        uint64_t hsz2 = decode_varint(record, &tmp_hlen);
        int hdrpos = tmp_hlen;
        int ncols = 0;
        uint64_t serials[16];

        while (hdrpos < (int)hsz2 && ncols < 16)
        {
            int stlen;
            serials[ncols] = decode_varint(record + hdrpos, &stlen);
            hdrpos += stlen;
            ++ncols;
        }

        int bodypos = hsz2;

        char typebuf[128] = {0};
        char namebuf[128] = {0};
        char tblbuf[128] = {0};
        uint64_t rootpage = 0;
        char sqlbuf[512] = {0};

        for (int c = 0; c < ncols; ++c)
        {
            uint64_t st = serials[c];

            if (st == 0) { /* NULL */ }
            else if (st >= 13 && (st % 2) == 1)
            {
                int tlen = (st - 13) / 2;

                if (bodypos + tlen <= rec_total)
                {
                    if (c == 0)
                        memcpy(typebuf, record + bodypos, tlen);
                    else if (c == 1)
                        memcpy(namebuf, record + bodypos, tlen);
                    else if (c == 2)
                        memcpy(tblbuf, record + bodypos, tlen);
                    else if (c == 4)
                        memcpy(sqlbuf, record + bodypos, tlen);
                }

                bodypos += tlen;
            }
            else if (st >= 1 && st <= 6)
            {
                int sz = (st == 1 ? 1 : st == 2 ? 2 : st == 3 ? 3 : st == 4 ? 4 : st == 5 ? 6 : 8);
                int64_t v = read_be_signed(record + bodypos, sz);

                if (c == 3)
                    rootpage = (uint64_t)v;

                bodypos += sz;
            }
        }

        if (strcmp(namebuf, target) == 0)
        {
            printf("Found table %s at root page %llu\n", target, (unsigned long long)rootpage);

            if (!rootpage)
            {
                fprintf(stderr, "rootpage==0\n");
                free(record);
                continue;
            }

            if ((uint64_t)page_size * (rootpage - 1) + page_size > (uint64_t)flen)
            {
                fprintf(stderr, "file too small for root page\n");
                free(record);
                continue;
            }

            unsigned char *page = buf + (rootpage - 1) * page_size;
            unsigned char *hdr = page + 0;
            unsigned char ptype2 = hdr[0];

            if (ptype2 != 0x0D)
                fprintf(stderr, "table root not leaf (type=%02x)\n", ptype2);

            int cellcnt = (hdr[3] << 8) | hdr[4];
            int ptrs_off = 8;

            for (int ci = 0; ci < cellcnt; ++ci)
            {
                int cellptr = (page[ptrs_off + ci * 2] << 8) | page[ptrs_off + ci * 2 + 1];
                unsigned char *cell2 = page + cellptr;
                int l1;
                uint64_t payload2 = decode_varint(cell2, &l1);
                unsigned char *p2 = cell2 + l1;
                int l2;
                uint64_t rowid2 = decode_varint(p2, &l2);
                unsigned char *payloaddata2 = p2 + l2;

                unsigned char *rec2 = malloc(payload2);

                if (!rec2)
                    continue;

                memcpy(rec2, payloaddata2, payload2);
                printf("rowid=%llu: ", (unsigned long long)rowid2);
                print_record(rec2, payload2);
                free(rec2);
            }

            free(record);
            break;
        }

        free(record);
    }

    free(buf);
    return 0;
}
