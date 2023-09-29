#ifndef M5_UTIL_H
#define M5_UTIL_H

#include <stdint.h>

#define CEG(DECP, RET, CALL, FAILCODE) ({\
	RET __tmp = CALL; if ((DECP)->error) FAILCODE; __tmp;\
})
#define CEV(DECP, CALL, FAILCODE) ({\
	CALL; if ((DECP)->error) FAILCODE;\
})
#define CEU8(DECP, CALL, FAILCODE) CEG(DECP, uint8_t, CALL, FAILCODE)

uint64_t slurp(char *, uint8_t **, uint8_t *);

#endif // M5_UTIL_H
