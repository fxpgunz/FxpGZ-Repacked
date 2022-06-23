#ifndef MBLOBARRAY_H
#define MBLOBARRAY_H


void* MMakeBlobArray(int nOneBlobSize, int nBlobCount);
void MEraseBlobArray(void* pBlob);
int MGetOneBlobSize(void* pBlob);
void* MGetBlobArrayElement(void* pBlob, int i);
int MGetBlobArrayCount(void* pBlob);
int MGetBlobArraySize(void* pBlob);
void* MGetBlobArrayPointer(void* pBlob);

#endif