#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define SIZE 32768

static const long long mask[8] =
{0x0000000000000000LL,
 0x00000000000000ffLL,
 0x000000000000ffffLL,
 0x0000000000ffffffLL,
 0x000000ffffffffffLL,
 0x0000ffffffffffffLL,
 0x00ffffffffffffffLL,
 0xffffffffffffffffLL};

static void Compress(long predsizem1)
{
  register long i, out, intot, hash, dhash, code, bcode, ioc;
  register long long val, lastval, stride, pred1, pred2, xor1, xor2;
  register long long *fcm, *dfcm;
  unsigned long long inbuf[SIZE + 1];
  unsigned char outbuf[6 + (SIZE / 2) + (SIZE * 8) + 2];

  assert(0 == ((long)outbuf & 0x7));

  outbuf[0] = predsizem1;
  ioc = fwrite(outbuf, 1, 1, stdout);
  assert(1 == ioc);
  predsizem1 = (1L << predsizem1) - 1;

  hash = 0;
  dhash = 0;
  lastval = 0;
  pred1 = 0;
  pred2 = 0;
  fcm = (long long *)calloc(predsizem1 + 1, 8);
  assert(NULL != fcm);
  dfcm = (long long *)calloc(predsizem1 + 1, 8);
  assert(NULL != dfcm);

  intot = fread(inbuf, 8, SIZE, stdin);
  while (0 < intot) {
    val = inbuf[0];
    out = 6 + ((intot + 1) >> 1);
    *((long long *)&outbuf[(out >> 3) << 3]) = 0;
    for (i = 0; i < intot; i += 2) {
      xor1 = val ^ pred1;
      fcm[hash] = val;
      hash = ((hash << 6) ^ ((unsigned long long)val >> 48)) & predsizem1;
      pred1 = fcm[hash];

      stride = val - lastval;
      xor2 = val ^ (lastval + pred2);
      lastval = val;
      val = inbuf[i + 1];
      dfcm[dhash] = stride;
      dhash = ((dhash << 2) ^ ((unsigned long long)stride >> 40)) & predsizem1;
      pred2 = dfcm[dhash];

      code = 0;
      if ((unsigned long long)xor1 > (unsigned long long)xor2) {
        code = 0x80;
        xor1 = xor2;
      }
      bcode = 7;                // 8 bytes
      if (0 == (xor1 >> 56))
        bcode = 6;              // 7 bytes
      if (0 == (xor1 >> 48))
        bcode = 5;              // 6 bytes
      if (0 == (xor1 >> 40))
        bcode = 4;              // 5 bytes
      if (0 == (xor1 >> 24))
        bcode = 3;              // 3 bytes
      if (0 == (xor1 >> 16))
        bcode = 2;              // 2 bytes
      if (0 == (xor1 >> 8))
        bcode = 1;              // 1 byte
      if (0 == xor1)
        bcode = 0;              // 0 bytes

      *((long long *)&outbuf[(out >> 3) << 3]) |= xor1 << ((out & 0x7) << 3);
      if (0 == (out & 0x7))
        xor1 = 0;
      *((long long *)&outbuf[((out >> 3) << 3) + 8]) = (unsigned long long)xor1 >> (64 - ((out & 0x7) << 3));

      out += bcode + (bcode >> 2);
      code |= bcode << 4;

      xor1 = val ^ pred1;
      fcm[hash] = val;
      hash = ((hash << 6) ^ ((unsigned long long)val >> 48)) & predsizem1;
      pred1 = fcm[hash];

      stride = val - lastval;
      xor2 = val ^ (lastval + pred2);
      lastval = val;
      val = inbuf[i + 2];
      dfcm[dhash] = stride;
      dhash = ((dhash << 2) ^ ((unsigned long long)stride >> 40)) & predsizem1;
      pred2 = dfcm[dhash];

      bcode = code | 0x8;
      if ((unsigned long long)xor1 > (unsigned long long)xor2) {
        code = bcode;
        xor1 = xor2;
      }
      bcode = 7;                // 8 bytes
      if (0 == (xor1 >> 56))
        bcode = 6;              // 7 bytes
      if (0 == (xor1 >> 48))
        bcode = 5;              // 6 bytes
      if (0 == (xor1 >> 40))
        bcode = 4;              // 5 bytes
      if (0 == (xor1 >> 24))
        bcode = 3;              // 3 bytes
      if (0 == (xor1 >> 16))
        bcode = 2;              // 2 bytes
      if (0 == (xor1 >> 8))
        bcode = 1;              // 1 byte
      if (0 == xor1)
        bcode = 0;              // 0 bytes

      *((long long *)&outbuf[(out >> 3) << 3]) |= xor1 << ((out & 0x7) << 3);
      if (0 == (out & 0x7))
        xor1 = 0;
      *((long long *)&outbuf[((out >> 3) << 3) + 8]) = (unsigned long long)xor1 >> (64 - ((out & 0x7) << 3));

      out += bcode + (bcode >> 2);
      outbuf[6 + (i >> 1)] = code | bcode;
    }
    if (0 != (intot & 1)) {
      out -= bcode + (bcode >> 2);
    }
    outbuf[0] = intot;
    outbuf[1] = intot >> 8;
    outbuf[2] = intot >> 16;
    outbuf[3] = out;
    outbuf[4] = out >> 8;
    outbuf[5] = out >> 16;
    ioc = fwrite(outbuf, 1, out, stdout);
    assert(ioc == out);
    intot = fread(inbuf, 8, SIZE, stdin);
  }
}

static void Decompress()
{
  register long i, in, intot, hash, dhash, code, bcode, predsizem1, end, tmp, ioc;
  register long long val, lastval, stride, pred1, pred2, next;
  register long long *fcm, *dfcm;
  long long outbuf[SIZE];
  unsigned char inbuf[(SIZE / 2) + (SIZE * 8) + 6 + 2];

  assert(0 == ((long)inbuf & 0x7));

  ioc = fread(inbuf, 1, 7, stdin);
  if (1 != ioc) {
    assert(7 == ioc);
    predsizem1 = inbuf[0];
    predsizem1 = (1L << predsizem1) - 1;

    hash = 0;
    dhash = 0;
    lastval = 0;
    pred1 = 0;
    pred2 = 0;
    fcm = (long long *)calloc(predsizem1 + 1, 8);
    assert(NULL != fcm);
    dfcm = (long long *)calloc(predsizem1 + 1, 8);
    assert(NULL != dfcm);

    intot = inbuf[3];
    intot = (intot << 8) | inbuf[2];
    intot = (intot << 8) | inbuf[1];
    in = inbuf[6];
    in = (in << 8) | inbuf[5];
    in = (in << 8) | inbuf[4];
    assert(SIZE >= intot);
    do {
      end = fread(inbuf, 1, in, stdin);
      assert((end + 6) >= in);
      in = (intot + 1) >> 1;
      for (i = 0; i < intot; i += 2) {
        code = inbuf[i >> 1];

        val = *((long long *)&inbuf[(in >> 3) << 3]);
        next = *((long long *)&inbuf[((in >> 3) << 3) + 8]);
        tmp = (in & 0x7) << 3;
        val = (unsigned long long)val >> tmp;
        next <<= 64 - tmp;
        if (0 == tmp)
          next = 0;
        val |= next;

        bcode = (code >> 4) & 0x7;
        val &= mask[bcode];
        in += bcode + (bcode >> 2);

        if (0 != (code & 0x80))
          pred1 = pred2;
        val ^= pred1;

        fcm[hash] = val;
        hash = ((hash << 6) ^ ((unsigned long long)val >> 48)) & predsizem1;
        pred1 = fcm[hash];

        stride = val - lastval;
        dfcm[dhash] = stride;
        dhash = ((dhash << 2) ^ ((unsigned long long)stride >> 40)) & predsizem1;
        pred2 = val + dfcm[dhash];
        lastval = val;

        outbuf[i] = val;

        val = *((long long *)&inbuf[(in >> 3) << 3]);
        next = *((long long *)&inbuf[((in >> 3) << 3) + 8]);
        tmp = (in & 0x7) << 3;
        val = (unsigned long long)val >> tmp;
        next <<= 64 - tmp;
        if (0 == tmp)
          next = 0;
        val |= next;

        bcode = code & 0x7;
        val &= mask[bcode];
        in += bcode + (bcode >> 2);

        if (0 != (code & 0x8))
          pred1 = pred2;
        val ^= pred1;

        fcm[hash] = val;
        hash = ((hash << 6) ^ ((unsigned long long)val >> 48)) & predsizem1;
        pred1 = fcm[hash];

        stride = val - lastval;
        dfcm[dhash] = stride;
        dhash = ((dhash << 2) ^ ((unsigned long long)stride >> 40)) & predsizem1;
        pred2 = val + dfcm[dhash];
        lastval = val;

        outbuf[i + 1] = val;
      }
      ioc = fwrite(outbuf, 8, intot, stdout);
      assert(ioc == intot);
      intot = 0;
      if ((end - 6) >= in) {
        intot = inbuf[in + 2];
        intot = (intot << 8) | inbuf[in + 1];
        intot = (intot << 8) | inbuf[in];
        end = inbuf[in + 5];
        end = (end << 8) | inbuf[in + 4];
        end = (end << 8) | inbuf[in + 3];
        in = end;
      }
      assert(SIZE >= intot);
    } while (0 < intot);
  }
}

int main(int argc, char *argv[])
{
  long val, ioc;

  assert(4 <= sizeof(long));
  assert(8 == sizeof(long long));
  assert(0 < SIZE);
  assert(0 == (SIZE & 0xf));
  val = 1;
  assert(1 == *((char *)&val));

  if (argc > 1) {
    val = -1;
    val = atol(argv[1]);
    assert(0 <= val);
    Compress(val);
    ioc = fread(&val, 1, 1, stdin);
    assert(0 == ioc);
  } else {
    Decompress();
  }

  return 0;
}
