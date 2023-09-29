#ifndef BER_TYPES_H
#define BER_TYPES_H

#include <stdint.h>

typedef enum BER_TagClass_e {
	BER_CLASS_UNIVERSAL,
	BER_CLASS_APPLICATION,
	BER_CLASS_CONTEXT,
	BER_CLASS_PRIVATE
} BER_TagClass;

typedef enum BER_TagType_e {
	BER_TYPE_EOC,
	BER_TYPE_BOOLEAN,
	BER_TYPE_INTEGER,
	BER_TYPE_BIT_STRING,
	BER_TYPE_OCTET_STRING,
	BER_TYPE_NULL,
	BER_TYPE_OBJECT_IDENTIFIER,
	BER_TYPE_OBJECT_DESCRIPTOR,
	BER_TYPE_EXTERNAL,
	BER_TYPE_REAL,
	BER_TYPE_ENUMERATED,
	BER_TYPE_EMBEDDED_PDV,
	BER_TYPE_UTF8_STRING,
	BER_TYPE_RELATIVE_OID,
	BER_TYPE_TIME,
	BER_TYPE_RESERVED,
	BER_TYPE_SEQUENCE,
	BER_TYPE_SET,
	BER_TYPE_NUMERIC_STRING,
	BER_TYPE_PRINTABLE_STRING,
	BER_TYPE_T61_STRING,
	BER_TYPE_VIDEOTEX_STRING,
	BER_TYPE_IA5_STRING,
	BER_TYPE_UTC_TIME,
	BER_TYPE_GENERALIZED_TIME,
	BER_TYPE_GRAPHIC_STRING,
	BER_TYPE_VISIBLE_STRING,
	BER_TYPE_GENERAL_STRING,
	BER_TYPE_UNIVERSAL_STRING,
	BER_TYPE_CHARACTER_STRING,
	BER_TYPE_BMP_STRING,
	BER_TYPE_DATE,
	BER_TYPE_TIME_OF_DAY,
	BER_TYPE_DATE_TIME,
	BER_TYPE_DURATION,
	BER_TYPE_OID_IRI,
	BER_TYPE_RELATIVE_OID_IRI,
} BER_TagType;

extern const char *ber_tagclass_names[];
extern const char *ber_tagtype_names[];

typedef struct BER_Tag_s {
	BER_TagClass class: 2;
	uint8_t construct: 1;
	uint8_t longform_type: 1;
	BER_TagType type;
} BER_Tag;

typedef struct BER_Length_s {
	uint8_t definite: 1;
	uint8_t longform: 1;
	uint8_t reserved: 1;
	uint64_t length;
} BER_Length;

typedef struct BER_Value_s {
	BER_Tag tag;
	BER_Length length;
	uint8_t *data;
} BER_Value;

#endif // BER_TYPES_H
