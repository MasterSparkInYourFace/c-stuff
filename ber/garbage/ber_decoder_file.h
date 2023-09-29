#ifndef BER_DECODER_FILE_H
#define BER_DECODER_FILE_H

#include <stdio.h>

#include "ber_decoder.h"

typedef struct BER_DecoderFile_s {
	BER_DecoderType type; // FILE
	FILE *stream;
	uint8_t garbage[8];
	uint64_t pos;
	uint8_t eoc;
	BER_DecoderError error;
} BER_DecoderFile;

uint8_t ber_decoder_file_init(BER_DecoderFile *, char *);
uint8_t ber_decoder_file_read1(BER_DecoderFile *);
void    ber_decoder_file_read(BER_DecoderFile *, uint8_t *, size_t);
uint8_t ber_decoder_file_check_ber_eoc(BER_DecoderFile *);
uint8_t ber_decoder_file_check_eoc(BER_DecoderFile *);

#endif // BER_DECODER_FILE_H
