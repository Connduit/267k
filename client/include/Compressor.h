/* Compressor.h */


#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <memory>

class Compressor
{
public:
	virtual void compress();
	virtual void decompress();
private:
};


typedef std::unique_ptr<Compressor> CompressorUniquePtr;

#endif
