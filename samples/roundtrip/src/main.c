/* SPDX-License-Identifier: Apache-2.0 */

#include <stdlib.h>
#include <string.h>

#include <zephyr/sys/printk.h>

#include <uzlib.h>

#define HASH_BITS 8U
#define GZIP_HEADER_SIZE 10U
#define GZIP_TRAILER_SIZE 8U

static const uint8_t message[] =
	"Zephyr uses uzlib to compress this message and decompress it again. ";
static uzlib_hash_entry_t hash_table[1U << HASH_BITS];

static void write_le32(uint8_t *dest, uint32_t value)
{
	dest[0] = (uint8_t)value;
	dest[1] = (uint8_t)(value >> 8);
	dest[2] = (uint8_t)(value >> 16);
	dest[3] = (uint8_t)(value >> 24);
}

int main(void)
{
	static const uint8_t gzip_header[GZIP_HEADER_SIZE] = {
		0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03,
	};
	struct uzlib_comp comp = {0};
	struct uzlib_uncomp uncomp;
	uint8_t gzip_stream[sizeof(message) * 2U + GZIP_HEADER_SIZE + GZIP_TRAILER_SIZE];
	uint8_t output[sizeof(message)];
	const size_t message_size = sizeof(message) - 1U;
	size_t trailer_offset;
	size_t gzip_size;
	uint32_t crc;
	int result;

	comp.hash_table = hash_table;
	comp.hash_bits = HASH_BITS;
	comp.dict_size = 1U << HASH_BITS;

	zlib_start_block(&comp);
	uzlib_compress(&comp, message, (unsigned int)message_size);
	zlib_finish_block(&comp);
	if (comp.outbuf == NULL || comp.outlen <= 0) {
		printk("uzlib compression failed\n");
		return -1;
	}

	trailer_offset = GZIP_HEADER_SIZE + (size_t)comp.outlen;
	if (trailer_offset + GZIP_TRAILER_SIZE > sizeof(gzip_stream)) {
		printk("uzlib compressed output exceeds buffer\n");
		free(comp.outbuf);
		return -1;
	}

	memcpy(gzip_stream, gzip_header, sizeof(gzip_header));
	memcpy(&gzip_stream[GZIP_HEADER_SIZE], comp.outbuf, (size_t)comp.outlen);
	crc = ~uzlib_crc32(message, (unsigned int)message_size, ~0U);
	write_le32(&gzip_stream[trailer_offset], crc);
	write_le32(&gzip_stream[trailer_offset + sizeof(uint32_t)], (uint32_t)message_size);
	gzip_size = trailer_offset + GZIP_TRAILER_SIZE;
	free(comp.outbuf);

	uzlib_init();
	uzlib_uncompress_init(&uncomp, NULL, 0);
	uncomp.source = gzip_stream;
	uncomp.source_limit = gzip_stream + gzip_size;
	uncomp.source_read_cb = NULL;

	result = uzlib_gzip_parse_header(&uncomp);
	if (result != TINF_OK) {
		printk("uzlib gzip header error: %d\n", result);
		return -1;
	}

	uncomp.dest_start = output;
	uncomp.dest = output;
	uncomp.dest_limit = output + sizeof(output);
	do {
		result = uzlib_uncompress_chksum(&uncomp);
	} while (result == TINF_OK);
	if (result != TINF_DONE) {
		printk("uzlib decompression error: %d\n", result);
		return -1;
	}

	if ((size_t)(uncomp.dest - output) != message_size ||
	    memcmp(output, message, message_size) != 0) {
		printk("uzlib round-trip output mismatch\n");
		return -1;
	}

	printk("uzlib round trip: %zu bytes compressed to %zu bytes, %zu bytes restored\n",
	       message_size, gzip_size, (size_t)(uncomp.dest - output));
	return 0;
}