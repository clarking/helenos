// TODO: replace with file generated by ccom_mkext

#ifndef _EXTERNAL_H_
#define _EXTERNAL_H_
#define MAXOPLEN 0
#define NUMBITS 32
#define BIT2BYTE(bits) ((((bits)+NUMBITS-1)/NUMBITS)*(NUMBITS/8))
#define BITSET(arr, bit) (arr[bit/NUMBITS] |= ((int)1 << (bit & (NUMBITS-1))))
#define BITCLEAR(arr, bit) (arr[bit/NUMBITS] &= ~((int)1 << (bit & (NUMBITS-1))))
#define TESTBIT(arr, bit) (arr[bit/NUMBITS] & ((int)1 << (bit & (NUMBITS-1))))
typedef int bittype;
extern int tempregs[], permregs[];
#define NTEMPREG 0
#define FREGS 0
#define NPERMREG 0
extern bittype validregs[];
#define AREGCNT 0
#define BREGCNT 0
#define CREGCNT 0
#define DREGCNT 0
#define EREGCNT 0
#define FREGCNT 0
#define GREGCNT 0
int aliasmap(int class, int regnum);
int color2reg(int color, int class);
int interferes(int reg1, int reg2);
#endif /* _EXTERNAL_H_ */
