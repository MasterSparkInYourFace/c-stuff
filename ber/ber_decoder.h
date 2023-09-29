#ifndef BER_DECODER_H
#define BER_DECODER_H

#include <stdint.h>

#include "ber_types.h"

typedef enum BER_DecoderError_e {
	BERR_OK,
	BERR_READ_EOC,
	BERR_READ,
	BERR_ALLOC,
	BERR_REALLOC,
	BERR_NODATA
} BER_DecoderError;

typedef struct BER_Decoder_s {
	uint8_t *block;
	uint64_t length;
	uint64_t pos;
	uint8_t eoc;
	BER_DecoderError error;
} BER_Decoder;

uint8_t ber_decoder_init(BER_Decoder *, uint8_t *, uint64_t);
uint8_t ber_decoder_read1(BER_Decoder *);
void    ber_decoder_read(BER_Decoder *, uint8_t *, uint64_t);
uint8_t ber_decoder_check_ber_eoc(BER_Decoder *);
uint8_t ber_decoder_check_eoc(BER_Decoder *);

#endif // BER_DECODER_H
