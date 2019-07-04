#include "ByteBuffer.h"

SharedPool<ByteBuffer> ByteBuffer::_byteBuffer128;
SharedPool<ByteBuffer> ByteBuffer::_byteBuffer512;
SharedPool<ByteBuffer> ByteBuffer::_byteBuffer1024;
SharedPool<ByteBuffer> ByteBuffer::_byteBuffer4096;
SharedPool<ByteBuffer> ByteBuffer::_byteBuffer8192;
