/* Encoder.h */
#ifndef ENCODER_H
#define ENCODER_H
#include <cstring>
#include <memory>

class Encoder
{
public:
	virtual void encode();
	virtual void decode();
private:
};

typedef std::unique_ptr<Encoder> EncoderUniquePtr;
#endif
