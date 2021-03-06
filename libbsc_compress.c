/*
 * This file is a part of Pcompress, a chunked parallel multi-
 * algorithm lossless compression and decompression program.
 *
 * Copyright (C) 2012 Moinak Ghosh. All rights reserved.
 * Use is subject to license terms.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * moinakg@belenix.org, http://moinakg.wordpress.com/
 *      
 * This program includes partly-modified public domain source
 * code from the LZMA SDK: http://www.7-zip.org/sdk.html
 */

#include <sys/types.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <utils.h>
#include <pcompress.h>
#include <allocator.h>
#include <libbsc.h>

// 1G
#define	BSC_MAX_CHUNK	1073741824L

struct libbsc_params {
	int lzpHashSize;
	int lzpMinLen;
	int bscCoder;
	int features;
};

static void
libbsc_err(int err) {
	switch (err) {
	    case LIBBSC_BAD_PARAMETER:
		fprintf(stderr, "LIBBSC: Bad Parameter.\n");
		break;
	    case LIBBSC_NOT_ENOUGH_MEMORY:
		fprintf(stderr, "LIBBSC: Out of memory.\n");
		break;
	    case LIBBSC_NOT_SUPPORTED:
		fprintf(stderr, "LIBBSC: Using unsupported feature.\n");
		break;
	    case LIBBSC_UNEXPECTED_EOB:
		fprintf(stderr, "LIBBSC: Unexpected end of block.\n");
		break;
	    case LIBBSC_DATA_CORRUPT:
		fprintf(stderr, "LIBBSC: Corrupt data.\n");
		break;
	}
}

void
libbsc_stats(int show)
{
}

/*
 * BSC uses OpenMP where it is tricky to control thread count
 * deterministically. We only use multithread capability in BSC
 * when compressing entire file in a single chunk.
 */
void
libbsc_props(algo_props_t *data, int level, ssize_t chunksize) {
	data->compress_mt_capable = 0;
	data->decompress_mt_capable = 0;
	data->single_chunk_mt_capable = 1;
	data->buf_extra = 0;
	data->c_max_threads = 8;
	data->d_max_threads = 8;
}

int
libbsc_init(void **data, int *level, int nthreads, ssize_t chunksize)
{
	struct libbsc_params *bscdat;
	int rv;

	if (chunksize > BSC_MAX_CHUNK) {
		fprintf(stderr, "Max allowed chunk size for LIBBSC is: %d \n",
		    BSC_MAX_CHUNK);
		return (1);
	}
	bscdat = slab_alloc(NULL, sizeof (struct libbsc_params));

	bscdat->features = LIBBSC_FEATURE_FASTMODE;
	if (nthreads > 1)
		bscdat->features |= LIBBSC_FEATURE_MULTITHREADING;

	if (*level > 9) *level = 9;

	bscdat->lzpHashSize = LIBBSC_DEFAULT_LZPHASHSIZE + (*level - 1);
	bscdat->bscCoder = LIBBSC_CODER_QLFC_STATIC;
	if (*level == 0) {
		bscdat->lzpMinLen = 32;

	} else if (*level < 3) {
		bscdat->lzpMinLen = 64;

	} else if (*level < 5) {
		bscdat->lzpMinLen = 128;
		bscdat->bscCoder = LIBBSC_CODER_QLFC_ADAPTIVE;

	} else {
		bscdat->lzpMinLen = 200;
		bscdat->bscCoder = LIBBSC_CODER_QLFC_ADAPTIVE;
	}
	*data = bscdat;
	rv = bsc_init(bscdat->features);
	if (rv != LIBBSC_NO_ERROR) {
		libbsc_err(rv);
		return (-1);
	}

	return (0);
}

int
libbsc_deinit(void **data)
{
	struct libbsc_params *bscdat = (struct libbsc_params *)(*data);
	
	if (bscdat) {
		slab_free(NULL, bscdat);
	}
	*data = NULL;
	return (0);
}

int
libbsc_compress(void *src, size_t srclen, void *dst, size_t *dstlen,
	       int level, uchar_t chdr, void *data)
{
	int rv;
	struct libbsc_params *bscdat = (struct libbsc_params *)data;

	rv = bsc_compress(src, dst, srclen, bscdat->lzpHashSize, bscdat->lzpMinLen,
	    LIBBSC_BLOCKSORTER_BWT, bscdat->bscCoder, bscdat->features);
	if (rv < 0) {
		libbsc_err(rv);
		return (-1);
	}
	*dstlen = rv;
	return (0);
}

int
libbsc_decompress(void *src, size_t srclen, void *dst, size_t *dstlen,
		 int level, uchar_t chdr, void *data)
{
	int rv;
	struct libbsc_params *bscdat = (struct libbsc_params *)data;

	rv = bsc_decompress(src, srclen, dst, *dstlen, bscdat->features);
	if (rv != LIBBSC_NO_ERROR) {
		libbsc_err(rv);
		return (-1);
	}
	return (0);
}
