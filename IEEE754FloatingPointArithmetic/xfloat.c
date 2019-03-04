#include <stdio.h>
#include <stdlib.h>

#define SMASK ((unsigned)0x80000000) //10000...
#define EMASK ((unsigned)0x7f800000) //011111111000...
#define MMASK ((unsigned)0x007fffff) //00000000011111111111111111111111
#define BMASK ((unsigned)0x00800000) //00000000100...

/* The following struct defines the XFLOAT type */
typedef struct {
  unsigned char sign;
  unsigned char exp;
  unsigned int man;
} XFLOAT;

/* The following union is used in conversions between float and XFLOAT */
typedef union {
  float f;
  unsigned int l;
} XFCONV;

float xfltofl(XFLOAT*);
void fltoxfl(float, XFLOAT*);
void xmult(XFLOAT*, XFLOAT*, XFLOAT*);
void xadd(XFLOAT*, XFLOAT*, XFLOAT*);
void printline(char*, XFLOAT*x);

int main(int argc, char *argv[])
{
  XFLOAT xf, yf, zf, wf;
  float x, y;
  XFCONV cv;

  if (argc < 3) exit(1);

  /* read x and y as floats from the command line */
  sscanf(argv[1], "%f", &x);
  sscanf(argv[2], "%f", &y);

  /* convert x and y to the xfloat struct format */
  fltoxfl(x, &xf);
  fltoxfl(y, &yf);

  //printf("%u\n", xf.exp);

  /* compute z=x*y and w=x+y */
  xmult(&xf, &yf, &zf);
  xadd(&xf, &yf, &wf);

  /* print x,, y, z, and w */
  printline("x:", &xf);
  printline("y:", &yf);
  printline("x*y:", &zf);
  printline("x+y:", &wf);
}

/* multiplies floats is IEEE 754 format */
void xmult(XFLOAT* x, XFLOAT* y, XFLOAT* z)
{
  //add leading 1
  x->man = x->man | BMASK;
  y->man = y->man | BMASK;

  //add exponents, correcting for offset
  z->exp = x->exp + y->exp - 127;

  //account for overflow and shift to 23 bit format
  unsigned long long int newMan = (unsigned long long)y->man * (unsigned long long)x->man;
  newMan = newMan >> 23;
  z->man = newMan;

  /*
  char bin[] = "0000000000000000000000000000";
  
  toBinary(z->man, bin, 28);

  printf("%s\n", bin);
  */

  //remove leading 1
  z->man = z->man & MMASK; 
}

/* adds floats in IEEE 754 format */
void xadd(XFLOAT* x, XFLOAT* y, XFLOAT* z)
{
  //Add leading 1
  x->man = x->man | BMASK;
  y->man = y->man | BMASK;

  //align decimal points by changing both exponents to the greater one
  if (x->exp > y->exp) {
    y->man = y->man >> (x->exp - y->exp);
    z->exp = x->exp;
  }
  else {
    x->man = x->man >> (y->exp - x->exp);
    z->exp = y->exp;
  }

  //add or subtract, depending on the sign of the inputs
  if (x->sign == 0 && y->sign == 0) {
    z->man = x->man + y->man;
  }
  else if (x->sign == 0) {
    if (x->man > y->man) {
      printf("hi\n");
      z->man = x->man - y->man;
      z->sign = 0;
    }
    else {
      z->man = y->man - x->man;
      z->sign = 1;
    }
  }
  else if (y->sign == 0) {
    if (x->man > y->man) {
      z->man = x->man - y->man;
      z->sign = 1;
    }
    else {
      z->man = y->man - x->man;
      z->sign = 0;
    }
  }
  else {
    z->man = x->man +  y->man;
    z->sign = 1;
  }

  char bin[] = "0000000000000000000000000";
  
  toBinary(z->man, bin, 25);

  int count = 1;

  //correct for offset for both addition and subtraction
  if (bin[0]=='1') {
    z->exp = z->exp + 1;
    z->man = z->man >> 1;
  }
  else {
    while (bin[count] != '1') {
      z->man = z->man << 1;
      z->exp = z->exp - 1;
      count++;
    }
  }

  //remove leading 1
  z->man = z->man & MMASK; 
}

/* convert xfloat to float */
float xfltofl(XFLOAT* xf)
{
  XFCONV xu;

  xu.l = (((unsigned long)xf->sign) << 31) | 
         (((unsigned long)xf->exp) << 23) |
         (unsigned long)xf->man;
  return xu.f;
}

/* convert float to XFLOAT */
void fltoxfl(float x, XFLOAT *xf)
{
  XFCONV xu;

  xu.f = x;
  xf->sign = (unsigned char) ((xu.l & SMASK) >> 31);
  xf->exp = (unsigned char) ((xu.l & EMASK) >> 23);
  xf->man = (unsigned long) (xu.l & MMASK);
}

/* Converts x to binary and copies its low order bits to d */
void toBinary(unsigned long x, char*d, int len){
  int bit;
  if(len>0){
    d = d+len-1;
    while(len>0){
      bit = x & 1;
      *d-- = '0'+bit;
      x>>=1;
      --len;
    }
  }
}

/* Displays x (with given prefix) in hex, binary, and float formats */
void printline(char*prefix, XFLOAT*x){
  XFCONV cv;
  char bin[] = "(0 00000000 00000000000000000000000)";
  cv.f = xfltofl(x);
  toBinary(x->sign, bin+1, 1);
  toBinary(x->exp, bin+3, 8);
  toBinary(x->man, bin+12, 23);
  printf("%-4s %08x %s s: %d e: %02x m: %06x  %f\n", 
       prefix, cv.l, bin, x->sign, x->exp, x->man, cv.f);
}

