#include <acknext.h>
#include <zlib.h>
#include <stdlib.h>
#include <string.h>

#define BLOCKSIZE 16384

struct block
{
	size_t length;
	uint8_t buffer[BLOCKSIZE];
	struct block * next;
};

static struct block * block_alloc()
{
	struct block * b = malloc(sizeof(struct block));
	b->next = NULL;
	b->length = 0;
	return b;
}

BLOB * blob_inflate(BLOB const * blob)
{
    z_stream strm  = {0};
	strm.total_out = 0;
	strm.total_in = 0;

    strm.avail_in  = blob->size;
	strm.next_in   = (Bytef *) blob->data;

    strm.zalloc = Z_NULL;
    strm.zfree  = Z_NULL;
    strm.opaque = Z_NULL;

    int err = -1;

	struct block * first = block_alloc();

	struct block * current = first;
	strm.avail_out = BLOCKSIZE;
    strm.next_out  = (Bytef *) current->buffer;

    err = inflateInit2(&strm, (15 + 32)); //15 window bits, and the +32 tells zlib to to detect if using gzip or zlib
    if (err == Z_OK)
	{
		do
		{
			err = inflate(&strm, Z_SYNC_FLUSH);
			if(err == Z_OK || err == Z_STREAM_END) {
				current->length = BLOCKSIZE - strm.avail_out;
				if(current->length > 0) {
					current->next = block_alloc();
					current = current->next;
				}
				strm.avail_out = BLOCKSIZE;
			    strm.next_out  = (Bytef *) current->buffer;
			}
		} while(err == Z_OK);
    }
    inflateEnd(&strm);

	BLOB * result = NULL;

	if(err == Z_STREAM_END)
	{
		result = blob_create(strm.total_out);
		uint8_t * output = result->data;
		for(struct block * it = first; it != NULL; it = it->next)
		{
			memcpy(output, it->buffer, it->length);
			output += it->length;
		}
	}

	for(struct block * it = first; it != NULL;)
	{
		struct block * obj = it;
		it = it->next;
		free(obj);
	}

	return result;
}
