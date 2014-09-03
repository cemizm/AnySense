#ifndef FIFO_H_
#define FIFO_H_

#include <stdint.h>

typedef struct {
	uint8_t _read;
	uint8_t _write;
	uint8_t _buffer[64];
} FIFO64_t;

typedef struct {
	uint8_t _read;
	uint8_t _write;
	uint8_t _buffer[128];
} FIFO128_t;

typedef struct {
	uint8_t _read;
	uint8_t _write;
	uint8_t _buffer[256];
} FIFO256_t;


typedef struct {
	uint8_t _read;
	uint8_t _write;
	uint8_t _buffer[512];
} FIFO512_t;


typedef struct {
	uint8_t _read;
	uint8_t _write;
	uint8_t _buffer[1024];
} FIFO1024_t;


#define FIFO_init(fifo)		{ fifo._read = 0; fifo._write = 0; }

#define FIFO_available(fifo)	( fifo._read != fifo._write )

#define FIFO_read(fifo, size) (						\
	(FIFO_available(fifo)) ?					\
	fifo._buffer[fifo._read = (fifo._read + 1) & (size-1)] : 0	\
)

#define FIFO_write(fifo, data, size) {								\
	uint8_t tmphead = ( fifo._write + 1 ) & (size-1); 	/* calculate buffer index */	\
	if(tmphead != fifo._read) {				/* if buffer is not full */	\
		fifo._write = tmphead;				/* store new index */		\
		fifo._buffer[tmphead] = data;			/* store data in buffer */	\
	}											\
}


#define FIFO64_read(fifo)			FIFO_read(fifo, 64)
#define FIFO64_write(fifo, data)		FIFO_write(fifo, data, 64)

#define FIFO128_read(fifo)			FIFO_read(fifo, 128)
#define FIFO128_write(fifo, data)		FIFO_write(fifo, data, 128)

#define FIFO512_read(fifo)			FIFO_read(fifo, 512)
#define FIFO512_write(fifo, data)		FIFO_write(fifo, data, 512)

#define FIFO1024_read(fifo)			FIFO_read(fifo, 1024)
#define FIFO1024_write(fifo, data)		FIFO_write(fifo, data, 1024)

#endif /*FIFO_H_*/
