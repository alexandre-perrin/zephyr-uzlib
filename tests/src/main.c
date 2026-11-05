/* SPDX-License-Identifier: Apache-2.0 */

#include <zephyr/ztest.h>

#include <uzlib.h>

static const uint8_t gzip_data[] = {
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf3, 0x48,
	0xcd, 0xc9, 0xc9, 0x57, 0x48, 0x2b, 0xca, 0xcf, 0x55, 0x28, 0xad, 0xca,
	0xc9, 0x4c, 0x52, 0x04, 0x00, 0xf8, 0xa3, 0x7d, 0x29, 0x11, 0x00, 0x00,
	0x00,
};

static const char expected[] = "Hello from uzlib!";

ZTEST(uzlib, test_gzip_decompression)
{
	struct uzlib_uncomp uncomp;
	uint8_t output[sizeof(expected)] = {0};
	int result;

	uzlib_init();
	uzlib_uncompress_init(&uncomp, NULL, 0);
	uncomp.source = gzip_data;
	uncomp.source_limit = gzip_data + sizeof(gzip_data);
	uncomp.source_read_cb = NULL;

	result = uzlib_gzip_parse_header(&uncomp);
	zassert_equal(result, TINF_OK, "gzip header parse failed: %d", result);

	uncomp.dest_start = output;
	uncomp.dest = output;
	uncomp.dest_limit = output + sizeof(output);

	do {
		result = uzlib_uncompress_chksum(&uncomp);
	} while (result == TINF_OK);

	zassert_equal(result, TINF_DONE, "decompression failed: %d", result);

	zassert_equal((size_t)(uncomp.dest - output), sizeof(expected) - 1U,
		      "unexpected decompressed length");
	zassert_mem_equal(output, expected, sizeof(expected) - 1U,
			  "decompressed output mismatch");
}

ZTEST_SUITE(uzlib, NULL, NULL, NULL, NULL, NULL);
