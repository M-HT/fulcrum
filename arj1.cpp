#include <stdio.h>
#include <limits.h>


typedef unsigned long UCRC;     /* CRC-32 */
#define CODE_BIT          16
#define CRC_MASK        0xFFFFFFFFL
#define CRCPOLY         0xEDB88320L
#define MAXSFX              25000L
#define DEFAULT_METHOD     1
#define DEFAULT_TYPE       0    /* if type_sw is selected */
#define HEADER_ID     0xEA60
#define HEADER_ID_HI    0xEA
#define HEADER_ID_LO    0x60
#define FIRST_HDR_SIZE    30
#define FIRST_HDR_SIZE_V  34
#define COMMENT_MAX     2048
#define FNAME_MAX           512
#define HEADERSIZE_MAX   (FIRST_HDR_SIZE + 10 + FNAME_MAX + COMMENT_MAX)
#define ASCII_MASK      0x7F

#define BINARY_TYPE        0    /* This must line up with binary/text strings */
#define TEXT_TYPE          1
#define COMMENT_TYPE       2
#define DIR_TYPE           3
#define LABEL_TYPE         4

#define GARBLE_FLAG     0x01
#define VOLUME_FLAG     0x04
#define EXTFILE_FLAG    0x08
#define PATHSYM_FLAG    0x10
#define BACKUP_FLAG     0x20

#define OS                  0

#define PATH_CHAR   '\\'
#define ARJ_PATH_CHAR   '/'

static UCRC             crctable[UCHAR_MAX + 1];
unsigned char           header[HEADERSIZE_MAX];
long                    compsize;
long                    origsize;
static UCRC             header_crc;
static unsigned char    *get_ptr;
static unsigned short   headersize;
static unsigned char    first_hdr_size;
UCRC                    crc;
static unsigned char    arj_nbr;
static unsigned char    arj_x_nbr;
static unsigned char    host_os;
static unsigned char    arj_flags;
static short            method;
static unsigned int     file_mode;
static unsigned long    time_stamp;
static short            entry_pos;
static unsigned short   host_data;
int                     file_type;
static UCRC             file_crc;

//static char             filename[FNAME_MAX];
//static char             comment[COMMENT_MAX];
static char             *hdr_filename;
//static char             *hdr_comment;

#define UPDATE_CRC(r,c) r=crctable[((unsigned char)(r)^(unsigned char)(c))&0xff]^(r>>CHAR_BIT)
#define get_crc()       get_longword()
#define fget_crc(f)     fget_longword(f)
#define setup_get(PTR)  (get_ptr = (PTR))
#define get_byte()      ((unsigned char)(*get_ptr++ & 0xff))
#define FIX_PARITY(c)   c &= ASCII_MASK




#define THRESHOLD    3
#define DDICSIZ      26624
#define MAXDICBIT   16
#define MATCHBIT     8
#define MAXMATCH   256
#define NC          (UCHAR_MAX + MAXMATCH + 2 - THRESHOLD)
#define NP          (MAXDICBIT + 1)
#define CBIT         9
#define NT          (CODE_BIT + 3)
#define PBIT         5
#define TBIT         5

#if NT > NP
#define NPT NT
#else
#define NPT NP
#endif

#define CTABLESIZE  4096
#define PTABLESIZE   256

#define STRTP          9
#define STOPP         13

#define STRTL          0
#define STOPL          7

/* Local variables */

//static unsigned char  *text = NULL;

static short  getlen;
static short  getbuf;

static unsigned short left[2 * NC - 1];
static unsigned short right[2 * NC - 1];
static unsigned char  c_len[NC];
static unsigned char  pt_len[NPT];

static unsigned short c_table[CTABLESIZE];
static unsigned short pt_table[PTABLESIZE];
static unsigned short blocksize;

unsigned short                  bitbuf;
unsigned char                   subbitbuf;
int    bitcount;

#define BUFFERSIZE      4096



static jump;

void exit(char *);
//void exit(char *s) {
//  printf("%s!\n",s);
//  exit(1);
//}


/****************************************************************************/

static void crc_buf(char *str, int len)
{
    while (len--)
        UPDATE_CRC(crc, *str++);
}

/****************************************************************************/


void fillbuf(int n, FILE *FileHandle)                /* Shift bitbuf n bits left, read n bits */
{
    bitbuf = (bitbuf << n) & 0xFFFF;  /* lose the first n bits */
    while (n > bitcount)
    {
        bitbuf |= subbitbuf << (n -= bitcount);
        if (compsize != 0)
        {
            compsize--;
            subbitbuf = (unsigned char) fgetc(FileHandle);
        }
        else
            subbitbuf = 0;
        bitcount = CHAR_BIT;
    }
    bitbuf |= subbitbuf >> (bitcount -= n);
}

/****************************************************************************/

void init_getbits(FILE *FileHandle)
{
    bitbuf = 0;
    subbitbuf = 0;
    bitcount = 0;
    fillbuf(2 * CHAR_BIT, FileHandle);
}

/****************************************************************************/

unsigned short getbits(int n, FILE *FileHandle)
{
    unsigned short x;

    x = bitbuf >> (2 * CHAR_BIT - n);
    fillbuf(n, FileHandle);
    return x;
}

/****************************************************************************/

static void make_table(int nchar, unsigned char *bitlen, int tablebits, unsigned short *table, int tablesize)
{
    unsigned short count[17], weight[17], start[18], *p;
    unsigned int i, k, len, ch, jutbits, avail, nextcode, mask;

    for (i = 1; i <= 16; i++)
        count[i] = 0;
    for (i = 0; (int)i < nchar; i++)
        count[bitlen[i]]++;

    start[1] = 0;
    for (i = 1; i <= 16; i++)
        start[i + 1] = start[i] + (count[i] << (16 - i));
//    if (start[17] != (unsigned short) (1 << 16)) printf("bad table\n");

    jutbits = 16 - tablebits;
    for (i = 1; (int)i <= tablebits; i++)
    {
        start[i] >>= jutbits;
        weight[i] = 1 << (tablebits - i);
    }
    while (i <= 16)
    {
        weight[i] = 1 << (16 - i);
        i++;
    }

    i = start[tablebits + 1] >> jutbits;
    if (i != (unsigned short) (1 << 16))
    {
        k = 1 << tablebits;
        while (i != k)
            table[i++] = 0;
    }

    avail = nchar;
    mask = 1 << (15 - tablebits);
    for (ch = 0; (int)ch < nchar; ch++)
    {
        if ((len = bitlen[ch]) == 0)
            continue;
        k = start[len];
        nextcode = k + weight[len];
        if ((int)len <= tablebits)
        {
//            if (nextcode > (unsigned int )tablesize) printf("bad table\n");
            for (i = start[len]; i < nextcode; i++)
                table[i] = ch;
        }
        else
        {
            p = (unsigned short *)&table[k >> jutbits];
            i = len - tablebits;
            while (i != 0)
            {
                if (*p == 0)
                {
                    right[avail] = left[avail] = 0;
                    *p = avail++;
                }
                if (k & mask)
                    p = &right[*p];
                else
                    p = &left[*p];
                k <<= 1;
                i--;
            }
            *p = ch;
        }
        start[len] = nextcode;
    }
}

/****************************************************************************/

static void read_pt_len(int nn, int nbit, int i_special, FILE *FileHandle)
{
    int i, n;
    short c;
    unsigned short mask;

    n = getbits(nbit, FileHandle);
    if (n == 0)
    {
        c = getbits(nbit, FileHandle);
        for (i = 0; i < nn; i++)
            pt_len[i] = 0;
        for (i = 0; i < 256; i++)
            pt_table[i] = c;
    }
    else
    {
        i = 0;
        while (i < n)
        {
            c = bitbuf >> (13);
            if (c == 7)
            {
                mask = 1 << (12);
                while (mask & bitbuf)
                {
                    mask >>= 1;
                    c++;
                }
            }
            fillbuf((c < 7) ? 3 : (int)(c - 3), FileHandle);
            pt_len[i++] = (unsigned char)c;
            if (i == i_special)
            {
                c = getbits(2, FileHandle);
                while (--c >= 0)
                    pt_len[i++] = 0;
            }
        }
        while (i < nn)
            pt_len[i++] = 0;
        make_table(nn, pt_len, 8, pt_table, sizeof(pt_table));
    }
}

/****************************************************************************/

static void read_c_len(FILE *FileHandle)
{
    short i, c, n;
    unsigned short mask;

    n = getbits(CBIT, FileHandle);
    if (n == 0)
    {
        c = getbits(CBIT, FileHandle);
        for (i = 0; i < NC; i++)
            c_len[i] = 0;
        for (i = 0; i < CTABLESIZE; i++)
            c_table[i] = c;
    }
    else
    {
        i = 0;
        while (i < n)
        {
            c = pt_table[bitbuf >> (8)];
            if (c >= NT)
            {
                mask = 1 << (7);
                do
                {
                    if (bitbuf & mask)
                        c = right[c];
                    else
                        c = left[c];
                    mask >>= 1;
                } while (c >= NT);
            }
            fillbuf((int)(pt_len[c]), FileHandle);
            if (c <= 2)
            {
                if (c == 0)
                    c = 1;
                else if (c == 1)
                    c = getbits(4,FileHandle) + 3;
                else
                    c = getbits(CBIT,FileHandle) + 20;
                while (--c >= 0)
                    c_len[i++] = 0;
            }
            else
                c_len[i++] = (unsigned char)(c - 2);
        }
        while (i < NC)
            c_len[i++] = 0;
        make_table(NC, c_len, 12, c_table, sizeof(c_table));
    }
}

/****************************************************************************/

static unsigned short decode_c(FILE *FileHandle)
{
    unsigned short j, mask;

    if (blocksize == 0)
    {
        blocksize = getbits(16, FileHandle);
        read_pt_len(NT, TBIT, 3, FileHandle);
        read_c_len(FileHandle);
        read_pt_len(NP, PBIT, -1, FileHandle);
    }
    blocksize--;
    j = c_table[bitbuf >> 4];
    if (j >= NC)
    {
        mask = 1 << (3);
        do
        {
            if (bitbuf & mask)
                j = right[j];
            else
                j = left[j];
            mask >>= 1;
        } while (j >= NC);
    }
    fillbuf((int)(c_len[j]), FileHandle);
    return j;
}

/****************************************************************************/

static unsigned short decode_p(FILE *FileHandle)
{
    unsigned short j, mask;

    j = pt_table[bitbuf >> (8)];
    if (j >= NP)
    {
        mask = 1 << (7);
        do
        {
            if (bitbuf & mask)
                j = right[j];
            else
                j = left[j];
            mask >>= 1;
        } while (j >= NP);
    }
    fillbuf((int)(pt_len[j]), FileHandle);
    if (j != 0)
    {
        j--;
        j = (1 << j) + getbits((int)j,FileHandle);
    }
    return j;
}

/****************************************************************************/

static void decode_start(FILE *FileHandle)
{
    blocksize = 0;
    init_getbits(FileHandle);
}

/****************************************************************************/


int decode(FILE *FileHandle, unsigned char *text) {
  static short i;
  static short j;
  short c;
  short r;
  static long count;

  if (jump == 1) goto j1;
  if (jump == 2) goto j2;
  if (jump >= 3) return 0;

  crc = CRC_MASK;
  if (method != 1 && method != 2 && method != 3) exit("Wrong packing method");

  decode_start(FileHandle);
  count = 0;
  r = 0;

  while (count < origsize) {
    if ((c = decode_c(FileHandle)) <= UCHAR_MAX) {
      text[r] = (unsigned char) c;
      count++;
      if (++r >= DDICSIZ) {
        crc_buf((char *) text,DDICSIZ);
        jump = 1;
        return DDICSIZ;
j1:
        r = 0;
      }
    } else {
      j = c - (UCHAR_MAX + 1 - THRESHOLD);
      count += j;
      i = decode_p(FileHandle);
      if ((i = r - i - 1) < 0) i += DDICSIZ;
      if (r > i && r < DDICSIZ - MAXMATCH - 1) {
        while (--j >= 0) text[r++] = text[i++];
      } else {
        while (--j >= 0) {
          text[r] = text[i];
          if (++r >= DDICSIZ) {
            crc_buf((char *) text,DDICSIZ);
            jump = 2;
            return DDICSIZ;
j2:
            r = 0;
          }
          if (++i >= DDICSIZ) i = 0;
        }
      }
    }
  }
  crc_buf((char *) text,r);
  if ((crc ^ CRC_MASK) != file_crc) exit("Datafile crc error");
  jump = 3;
  return r;
}

/* Macros */

#define BFIL {getbuf|=bitbuf>>getlen;fillbuf(CODE_BIT-getlen, FileHandle);getlen=CODE_BIT;}
#define GETBIT(c) {if(getlen<=0)BFIL c=(getbuf&0x8000)!=0;getbuf<<=1;getlen--;}
#define BPUL(l) {getbuf<<=l;getlen-=l;}
#define GETBITS(c,l) {if(getlen<l)BFIL c=(unsigned short)getbuf>>(CODE_BIT-l);BPUL(l)}

/****************************************************************************/

static short decode_ptr(FILE *FileHandle)
{
    short c;
    short width;
    short plus;
    short pwr;

    plus = 0;
    pwr = 1 << (STRTP);
    for (width = (STRTP); width < (STOPP) ; width++)
    {
        GETBIT(c);
        if (c == 0)
            break;
        plus += pwr;
        pwr <<= 1;
    }
    if (width != 0)
        GETBITS(c, width);
    c += plus;
    return c;
}

/****************************************************************************/

static short decode_len(FILE *FileHandle)
{
    short c;
    short width;
    short plus;
    short pwr;

    plus = 0;
    pwr = 1 << (STRTL);
    for (width = (STRTL); width < (STOPL) ; width++)
    {
        GETBIT(c);
        if (c == 0)
            break;
        plus += pwr;
        pwr <<= 1;
    }
    if (width != 0)
        GETBITS(c, width);
    c += plus;
    return c;
}


/****************************************************************************/

static int fget_byte(FILE *f)
{
    int c;

    if ((c = fgetc(f)) == EOF)
        exit("Unexpected end of datafile");
    return c & 0xFF;
}

/****************************************************************************/
static unsigned int fget_word(FILE *f)
{
    unsigned int b0, b1;

    b0 = fget_byte(f);
    b1 = fget_byte(f);
    return (b1 << 8) + b0;
}
/****************************************************************************/
static unsigned long fget_longword(FILE *f)
{
    unsigned long b0, b1, b2, b3;

    b0 = fget_byte(f);
    b1 = fget_byte(f);
    b2 = fget_byte(f);
    b3 = fget_byte(f);
    return (b3 << 24) + (b2 << 16) + (b1 << 8) + b0;
}

/****************************************************************************/

static void fread_crc(unsigned char *p, int n, FILE *f)
{
    n = fread((char *)p, 1, n, f);
    origsize += n;
    crc_buf((char *)p, n);
}

/****************************************************************************/

static unsigned int get_word()
{
    unsigned int b0, b1;

    b0 = get_byte();
    b1 = get_byte();
    return (b1 << 8) + b0;
}

/****************************************************************************/

static unsigned long get_longword()
{
    unsigned long b0, b1, b2, b3;

    b0 = get_byte();
    b1 = get_byte();
    b2 = get_byte();
    b3 = get_byte();
    return (b3 << 24) + (b2 << 16) + (b1 << 8) + b0;
}

/****************************************************************************/
static void strncopy(char *to, char *from, int len)
{
    int i;

    for (i = 1; i < len && *from; i++)
        *to++ = *from++;
    *to = 0;
}

/****************************************************************************/

/*
static void strparity(unsigned char *p)
{
    while (*p)
    {
        FIX_PARITY(*p);
        p++;
    }
}
*/

/****************************************************************************/

int read_header(FILE *fd) {
    unsigned short extheadersize, header_id;

    header_id = fget_word(fd);
    if (header_id != HEADER_ID && header_id != 0x239A)
      exit("Bad datafile header");

    headersize = fget_word(fd);
 if (header_id == 0x239A) headersize -= 32768;
    if (headersize == 0) return 0;
//      exit("read_header : end of arj file"); // end of archive
    if (headersize > HEADERSIZE_MAX)
      exit("Datafile header corrupted");

    crc = CRC_MASK;
    fread_crc(header, (int) headersize, fd);
    header_crc = fget_crc(fd);
    if ((crc ^ CRC_MASK) != header_crc)
      exit("Datafile header crc error");

    setup_get(header);
    first_hdr_size = get_byte();
    arj_nbr = get_byte();
    arj_x_nbr = get_byte();
    host_os = get_byte();
    arj_flags = get_byte();
    method = get_byte();
    file_type = get_byte();
    (void)get_byte();
    time_stamp = get_longword();
    compsize = get_longword();
    origsize = get_longword();
    file_crc = get_crc();
    entry_pos = get_word();
    file_mode = get_word();
    host_data = get_word();

    hdr_filename = (char *)&header[first_hdr_size];
//    strncopy(filename, hdr_filename, sizeof(filename));
//        strparity((unsigned char *)filename);

//    hdr_comment = (char *)&header[first_hdr_size + strlen(hdr_filename) + 1];
//    strncopy(comment, hdr_comment, sizeof(comment));
//        strparity((unsigned char *)comment);

    /* if extheadersize == 0 then no CRC */
    /* otherwise read extheader data and read 4 bytes for CRC */

    while ((extheadersize = fget_word(fd)) != 0)
        fseek(fd, (long) (extheadersize + 4), SEEK_CUR);

    jump = 0;
  return compsize;
}

void get_filename(char *filename, int len) {
  strncopy(filename, hdr_filename, len);
}

/****************************************************************************/

void make_crctable()
{
    unsigned int i, j;
    UCRC r;


    for (i = 0; i <= UCHAR_MAX; i++)
    {
        r = i;
        for (j = CHAR_BIT; j > 0; j--)
        {
            if (r & 1)
                r = (r >> 1) ^ CRCPOLY;
            else
                r >>= 1;
        }
        crctable[i] = r;
    }
}


/****************************************************************************/

