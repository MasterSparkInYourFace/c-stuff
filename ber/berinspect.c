#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "ber_types.h"
#include "ber_decoder.h"

struct PrintContext {
	uint64_t indent;
};

uint8_t der_decode_tag(BER_Decoder *d, BER_Tag *t) {
	t->longform_type = 0;
	uint8_t tag = CEU8(d, ber_decoder_read1(d), return 0);
	t->class     = (tag & 0b11000000) >> 6;
	t->construct = (tag & 0b00100000) >> 5;
	BER_TagType type = tag & 0b00011111;
	if (type == 31) {
		t->longform_type = 1;
		type = 0;
		do {
			tag = CEU8(d, ber_decoder_read1(d), return 0);
			type = (type << 7) | (tag & 127);
		} while (tag & 128); // MSB == 1
	}
	t->type = type;
	return 1;
}

uint8_t der_decode_length(BER_Decoder *d, BER_Length *l) {
	l->longform = 0;
	l->definite = 1;
	l->reserved = 0;
	l->length = 0;
	uint8_t initial = CEU8(d, ber_decoder_read1(d), return 0);
	uint8_t extra = (initial & 0x80) >> 7;
	uint8_t oct_count = initial & 0x7F;
	if (!extra) {
		l->length = oct_count;
		return 1;
	}
	if (!oct_count) {
		l->definite = 0;
		return 1;
	}
	if (oct_count == 0x7F) {
		l->reserved = 1;
		return 1;
	}
	l->longform = 1;
	do {
		l->length = (l->length << 8) | CEU8(d, ber_decoder_read1(d), return 0);
	} while (--oct_count);
	return 1;
}

uint8_t der_decode_value(BER_Decoder *d, BER_Value *v) {
#define POSTMALLOC_FAIL { free(v->data); return 0; }
	const size_t block_size = 4096;
	size_t current = block_size;
	uint64_t pos = 0;
	uint8_t *tmp;
	CEU8(d, der_decode_tag(d, &v->tag), return 0);
	CEU8(d, der_decode_length(d, &v->length), return 0);
	v->data = malloc(v->length.definite ? v->length.length : block_size);
	if (!v->data) {
		d->error = BERR_ALLOC;
		return 0;
	}
	if (v->length.definite) {
		CEV(d, ber_decoder_read(d, v->data, v->length.length), return 0);
		return 1;
	}
	while (!CEU8(d, ber_decoder_check_ber_eoc(d), POSTMALLOC_FAIL)) {
		v->data[pos++] = CEU8(d, ber_decoder_read1(d), POSTMALLOC_FAIL);
		if (pos >= current) {
			current += block_size;
			tmp = realloc(v->data, current);
			if (!tmp) {
				d->error = BERR_REALLOC;
				free(v->data);
				return 0;
			}
			v->data = tmp;
		}
	}
	v->length.length = pos;
	return 1;
#undef POSTMALLOC_FAIL
}

void indent(struct PrintContext *pri) {
	char tabs[pri->indent + 1];
	if (!pri->indent)
		return;
	memset(tabs, '\t', pri->indent);
	tabs[pri->indent] = 0;
	printf("%s", tabs);
}

void print_tag(BER_Tag *tag, struct PrintContext *pri) {
	indent(pri);
	puts("Tag {");
	pri->indent++;
	indent(pri);
	printf("(xx000000) Class: %d (%s)\n", tag->class,
		ber_tagclass_names[tag->class]);
	indent(pri);
	printf("(00x00000) Constructed: %d\n", tag->construct);
	indent(pri);
	printf("(%s) Type: %d (%s)\n",
		tag->longform_type ? "Long-form" : "000xxxxx",
		tag->type, ber_tagtype_names[tag->type]);
	pri->indent--;
	indent(pri);
	puts("}");
}

void print_length(BER_Length *length, struct PrintContext *pri) {
	indent(pri);
	if (length->reserved) {
		puts("Length: Reserved");
		return;
	}
	if (!length->definite) {
		printf("Length: Indefinite (%lu)\n", length->length);
		return;
	}
	printf("Length%s: %lu\n", length->longform ? " (Long-form)" : "", length->length);
}

void print_value(BER_Value *v) {
	struct PrintContext pri = { .indent = 0 };
	puts("Value {");
	pri.indent++;
	print_tag(&v->tag, &pri);
	print_length(&v->length, &pri);
	indent(&pri);
	puts("Data:");
	pri.indent++;
	for (uint8_t i = 0; i < v->length.length; i++) {
		if (!(i % 8)) {
			if (i)
				putchar('\n');
			indent(&pri);
		}
		printf("%02hhx ", v->data[i]);
	}
	if (v->length.length % 8)
		putchar('\n');
	puts("}");
}

void finalize(BER_Decoder *d) {
	free(d->block);
	switch (d->error) {
		case BERR_READ_EOC:
			puts("Unexpected end of file"); break;
		case BERR_READ:
			puts("Failed reading from file"); break;
		case BERR_ALLOC:
			puts("Memory allocation failed"); break;
		case BERR_REALLOC:
			puts("Memory re-allocation failed"); break;
		case BERR_NODATA:
			puts("Not enough data in buffer");
		default:
	}
}

int main(int argc, char **args) {
#define FAIL { finalize(&d); return 1; }
	if (argc < 2) {
		printf("usage: %s <BER file>\n", args[0]);
		return 1;
	}
	uint8_t *slurp_data;
	uint8_t slurp_fail;
	uint64_t size = slurp(args[1], &slurp_data, &slurp_fail);
	if (slurp_fail) {
		if (slurp_data)
			free(slurp_data);
		return 1;
	}
	BER_Decoder d;
	if (!ber_decoder_init(&d, slurp_data, size)) {
		printf("decoder init ('%s'): %m\n", args[1]);
		return 1;
	};
	BER_Value v;
	while (!ber_decoder_check_eoc(&d)) {
		CEU8(&d, der_decode_value(&d, &v), FAIL);
		print_value(&v);
		free(v.data);
	}
	free(slurp_data);
	return 0;
#undef FAIL
}
