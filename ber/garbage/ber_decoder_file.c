#include <stdio.h>
#include <stdint.h>

#include "util.h"
#include "ber_decoder.h"
#include "ber_decoder_file.h"

uint8_t ber_decoder_file_init(BER_DecoderFile *d, char *path) {
	d->type = BER_DEC_FILE;
	d->stream = fopen(path, "rb");
	if (!d->stream)
		return 0;
	d->pos = 0;
	d->eoc = 0;
	d->error = BERR_OK;
	return 1;
}

uint8_t read1_nonsync(BER_DecoderFile *d) {
	int b = fgetc(d->stream);
	if (b == EOF) {
		if (!feof(d->stream)) {
			d->error = BERR_READ;
			return 0;
		}
		d->error = BERR_READ_EOC;
		return 0;
	}
	return (uint8_t) b;
}

void sync_stream(BER_DecoderFile *d) {
	fseek(d->stream, SEEK_SET, d->pos);
}

uint8_t ber_decoder_file_read1(BER_DecoderFile *d) {
	uint8_t b = CEU8(d, read1_nonsync(d), return 0);
	d->pos++;
	return b;
}

void ber_decoder_file_read(BER_DecoderFile *d, uint8_t *dst, size_t length) {
	size_t bytes_read = fread(dst, 1, length, d->stream);
	d->pos += bytes_read;
	if (bytes_read < length) {
		if (!feof(d->stream)) {
			d->error = BERR_READ;
			return;
		}
		d->error = BERR_READ_EOC;
	}
}

uint8_t ber_decoder_file_check_ber_eoc(BER_DecoderFile *d) {
	uint8_t oct = CEU8(d, read1_nonsync(d), return 0);
	oct += CEU8(d, read1_nonsync(d), return 0);
	sync_stream(d);
	return !!oct; // if oct is 0, we haven't reached end of content
}

uint8_t ber_decoder_file_check_eoc(BER_DecoderFile *d) {
	read1_nonsync(d);
	if (d->error == BERR_READ_EOC) {
		d->error = BERR_OK;
		d->eoc = 1;
		return 1;
	}
	if (!d->error)
		sync_stream(d);
	return 0;
}
