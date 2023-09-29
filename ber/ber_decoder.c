#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "ber_types.h"
#include "ber_decoder.h"

uint8_t ber_decoder_init(BER_Decoder *d, uint8_t *b, uint64_t len) {
	d->block = b;
	d->length = len;
	d->pos = 0;
	d->eoc = 0;
	d->error = BERR_OK;
	return 1;
}

uint8_t ber_decoder_read1(BER_Decoder *d) {
	if (d->eoc) {
		d->error = BERR_READ_EOC;
		return 0;
	}
	uint8_t b = d->block[d->pos++];
	if (d->pos >= d->length)
		d->eoc = 1;
	return b;
}

void ber_decoder_read(BER_Decoder *d, uint8_t *dst, uint64_t length) {
	if (d->eoc) {
		d->error = BERR_READ_EOC;
		return;
	}
	if (d->pos + length > d->length) {
		d->error = BERR_NODATA;
		return;
	}
	memcpy(dst, d->block + d->pos, length);
	d->pos += length;
	if (d->pos >= d->length)
		d->eoc = 1;
}

uint8_t ber_decoder_check_ber_eoc(BER_Decoder *d) {
	uint8_t tags[2];
	ber_decoder_read(d, tags, 2);
	if (d->error)
		return 0;
	if ((uint16_t) *tags == 0)
		return 1;
	return 0;
}

uint8_t ber_decoder_check_eoc(BER_Decoder *d) {
	return d->eoc;
}
