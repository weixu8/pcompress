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

#ifndef	_PCOMPRESS_H
#define	_PCOMPRESS_H

#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

#ifdef	__cplusplus
extern "C" {
#endif

#include <rabin_dedup.h>

#define	CHUNK_FLAG_SZ	1
#define	ALGO_SZ		8
#define	MIN_CHUNK	2048
#define	VERSION		3
#define	FLAG_DEDUP	1
#define	FLAG_DEDUP_FIXED	1
#define	FLAG_SINGLE_CHUNK	2
#define	UTILITY_VERSION	"0.8.1"

#define	COMPRESSED	1
#define	UNCOMPRESSED	0
#define	CHSIZE_MASK	0x80
#define	BZIP2_A_NUM	16
#define	LZMA_A_NUM	32
#define	CHUNK_FLAG_DEDUP	2
#define	CHUNK_FLAG_PREPROC	4
#define	COMP_EXTN	".pz"

#define	PREPROC_TYPE_LZP	1
#define	PREPROC_COMPRESSED	128

/*
 * Sizes of chunk header components.
 */
#define	COMPRESSED_CHUNKSZ	(sizeof (uint64_t))
#define	ORIGINAL_CHUNKSZ	(sizeof (uint64_t))
#define	CHUNK_HDR_SZ		(COMPRESSED_CHUNKSZ + cksum_bytes + ORIGINAL_CHUNKSZ + CHUNK_FLAG_SZ)

/*
 * lower 3 bits in higher nibble indicate chunk compression algorithm
 * in adaptive modes.
 */
#define	COMPRESS_LZMA	1
#define	COMPRESS_BZIP2	2
#define	COMPRESS_PPMD	3
#define	CHDR_ALGO_MASK	7

extern uint32_t zlib_buf_extra(ssize_t buflen);
extern int lz4_buf_extra(ssize_t buflen);

extern int zlib_compress(void *src, size_t srclen, void *dst,
	size_t *destlen, int level, uchar_t chdr, void *data);
extern int lzma_compress(void *src, size_t srclen, void *dst,
	size_t *destlen, int level, uchar_t chdr, void *data);
extern int bzip2_compress(void *src, size_t srclen, void *dst,
	size_t *destlen, int level, uchar_t chdr, void *data);
extern int adapt_compress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int ppmd_compress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int lz_fx_compress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int lz4_compress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int none_compress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);

extern int zlib_decompress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int lzma_decompress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int bzip2_decompress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int adapt_decompress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int ppmd_decompress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int lz_fx_decompress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int lz4_decompress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int none_decompress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);

extern int adapt_init(void **data, int *level, int nthreads, ssize_t chunksize);
extern int adapt2_init(void **data, int *level, int nthreads, ssize_t chunksize);
extern int lzma_init(void **data, int *level, int nthreads, ssize_t chunksize);
extern int ppmd_init(void **data, int *level, int nthreads, ssize_t chunksize);
extern int bzip2_init(void **data, int *level, int nthreads, ssize_t chunksize);
extern int zlib_init(void **data, int *level, int nthreads, ssize_t chunksize);
extern int lz_fx_init(void **data, int *level, int nthreads, ssize_t chunksize);
extern int lz4_init(void **data, int *level, int nthreads, ssize_t chunksize);
extern int none_init(void **data, int *level, int nthreads, ssize_t chunksize);

extern void lzma_props(algo_props_t *data, int level, ssize_t chunksize);
extern void lzma_mt_props(algo_props_t *data, int level, ssize_t chunksize);
extern void lz4_props(algo_props_t *data, int level, ssize_t chunksize);

extern int zlib_deinit(void **data);
extern int adapt_deinit(void **data);
extern int lzma_deinit(void **data);
extern int ppmd_deinit(void **data);
extern int lz_fx_deinit(void **data);
extern int lz4_deinit(void **data);
extern int none_deinit(void **data);

extern void adapt_stats(int show);
extern void ppmd_stats(int show);
extern void lzma_stats(int show);
extern void bzip2_stats(int show);
extern void zlib_stats(int show);
extern void lz_fx_stats(int show);
extern void lz4_stats(int show);
extern void none_stats(int show);

#ifdef ENABLE_PC_LIBBSC
extern int libbsc_compress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int libbsc_decompress(void *src, size_t srclen, void *dst,
	size_t *dstlen, int level, uchar_t chdr, void *data);
extern int libbsc_init(void **data, int *level, int nthreads, ssize_t chunksize);
extern void libbsc_props(algo_props_t *data, int level, ssize_t chunksize);
extern int libbsc_deinit(void **data);
extern void libbsc_stats(int show);
#endif

/*
 * Per-thread data structure for compression and decompression threads.
 */
struct cmp_data {
	uchar_t *cmp_seg;
	uchar_t *compressed_chunk;
	uchar_t *uncompressed_chunk;
	dedupe_context_t *rctx;
	ssize_t rbytes;
	ssize_t chunksize;
	ssize_t len_cmp;
	uchar_t checksum[CKSUM_MAX_BYTES];
	int level;
	unsigned int id;
	compress_func_ptr compress;
	compress_func_ptr decompress;
	int cancel;
	sem_t start_sem;
	sem_t cmp_done_sem;
	sem_t write_done_sem;
	void *data;
	pthread_t thr;
};

#ifdef	__cplusplus
}
#endif

#endif
