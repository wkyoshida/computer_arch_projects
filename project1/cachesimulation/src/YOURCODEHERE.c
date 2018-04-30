#include "YOURCODEHERE.h"

/*
  Sets the following fields in cache "acache"

  acache->numways : The number of ways
  acache->blocksize: The size of one block, in bytes
  acache->numsets: The number of sets in the cache
  acache->BO: The number of bits of block offset
  acache->TO: The number of bits of tag offset (i.e. the number of bits to shift the tag so that its lowest bit is at position 0)
  acache->VAImask: An andmask the size of the index field
  acache->VATmask: An andmask the size of the tag field

 */
void setSizesOffsetsAndMaskFields(cache* acache, unsigned int size, unsigned int assoc, unsigned int blocksize){
  unsigned int localVAbits=8*sizeof(uint64_t*);
  if (localVAbits!=64){
    fprintf(stderr,"Running non-portable code on unsupported platform, terminating. Please use designated machines.\n");
    exit(-1);
  }

  unsigned int numSet;
  unsigned int sizeIndex;

  acache->numways = assoc;
  acache->blocksize = blocksize;
  acache->numsets = (size / blocksize) / assoc;
  acache->BO = 0;

  // get the log2 of blocksize
  while (blocksize >= 2) {
    blocksize /= 2;
    acache->BO++;
  }

  sizeIndex = 0;
  numSet = acache->numsets;

  // get the log2 of numSet
  while (numSet >= 2) {
    numSet /= 2;
    sizeIndex++;
  }

  acache->VAImask = 0;

  // create index andmask
  for (int curBitI = 0; curBitI < sizeIndex; ++curBitI) {
    acache->VAImask |= ((uint64_t)1 << curBitI);
  }

  acache->TO = acache->BO + sizeIndex;
  unsigned int sizeTag = localVAbits - acache->TO;
  acache->VATmask = 0;

  // create tag andmask
  for (int curBitT = 0; curBitT < sizeTag; ++curBitT) {
    acache->VATmask |= ((uint64_t)1 << curBitT);
  }
}

// returns the set index, given an address
unsigned long long getindex(cache* acache, unsigned long long address){

  unsigned long long index;
  index = address >> acache->BO;
  index &= acache->VAImask;

  return index;
}

// returns the tag, given an address
unsigned long long gettag(cache* acache, unsigned long long address){

  unsigned long long tag;
  tag = address >> acache->TO;
  tag &= acache->VATmask;

  return tag;
}

// writes back the entire cache line at index, waynum into the next level of cache (e.g. L1->L2 or L2->Mem, etc.)
void writeback(cache* acache, unsigned int index, unsigned int waynum){

  unsigned long long wordSize = sizeof(unsigned long long);
  unsigned long long address = 0;
  unsigned long long addressIndex = index << acache->BO;
  unsigned long long addressTag = acache->sets[index].blocks[waynum].tag << acache->TO;
  address |= addressIndex;
  address |= addressTag; // get address value

  for(int curword = 0; curword < (acache->blocksize / wordSize); curword++){
    unsigned long long word = acache->sets[index].blocks[waynum].datawords[curword];
    performaccess(acache->nextcache, address, wordSize, 1, word, curword); // send/write data
    address += wordSize;
  }
}

// fills (i.e. reads) the entire cache line that contains address "address" from the next level of cache/memory and places the read values into the current level of cache at index, waynum
void fill(cache * acache, unsigned int index, unsigned int waynum, unsigned long long address){

  unsigned long long wordSize = sizeof(unsigned long long);
  unsigned long long buildTag = acache->VATmask << acache->TO;
  unsigned long long buildIndex = acache->VAImask << acache->BO;
  unsigned long long buildTI = buildTag | buildIndex;
  address &= buildTI; // get address value

  for(int curword = 0; curword < (acache->blocksize / wordSize); curword++){
    unsigned long long word = performaccess(acache->nextcache, address, wordSize, 0, 0, curword); // receive/read data
    acache->sets[index].blocks[waynum].datawords[curword] = word; // update datawords content
    unsigned long long tag = gettag(acache, address);
    acache->sets[index].blocks[waynum].tag = tag; // update tag value
    address += wordSize;
  }
}
