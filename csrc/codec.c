// Copyright: (c) Magnus Therning, 2012
// License: BSD3, found in the LICENSE file

#include <assert.h>
#include <string.h>

#include "codec.h"

static uint8_t const qp_decmap[] = {
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
};

int qp_dec_c(uint8_t const *src, size_t srclen,
    uint8_t *dst, size_t *dstlen,
    uint8_t const **rem, size_t *remlen)
{
    assert(src || srclen == 0);
    assert(dst);
    assert(dstlen);
    assert(rem);
    assert(remlen);

    size_t od = *dstlen, i;
    int res = 0;

    for(i = 0, *dstlen = 0; i < srclen && *dstlen < od; i++, (*dstlen)++) {
        if((9 == src[i]) || (10 == src[i]) || (13 == src[i]) ||
            (32 <= src[i] && src[i] <= 60) ||
            (62 <= src[i] && src[i] <= 126)) {
            dst[*dstlen] = src[i];
        } else if('=' == src[i]) {
            if(i + 2 >= srclen) { res = 0; goto exit; }

            // We want to append nothing if we see '=\r\n'
            if (src[i + 1] == '\r' && src[i + 2] == '\n') {
              (*dstlen)--;
            } else {
              uint8_t o0 = qp_decmap[src[i + 1]], o1 = qp_decmap[src[i + 2]];
              if((o0 | o1) & 0xf0) { res = 1; break; }
              dst[*dstlen] = o0 << 4 | o1;
            }

            i += 2;
        } else { res = 1; goto exit; }
    }

exit:
    *rem = src + i;
    *remlen = srclen -i;

    return(res);
}
