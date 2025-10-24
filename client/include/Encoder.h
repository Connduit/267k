/* Encoder.h */
#ifndef ENCODER_H
#define ENCODER_H

#include <cstring>
#include <cstdint>
#include <memory>

class Encoder
{
public:
	//TODO: match function signature
	//virtual bool encode() = 0;
	//virtual bool decode() = 0;
private:
};


class B64Encoder : public Encoder
{
public:
	char* encode(const unsigned char* data, size_t input_len);
	int decode(const char* in, uint8_t* out);
	//bool decode();
private:
};

//class HexEncoder : public Encoder

typedef std::unique_ptr<Encoder> EncoderUniquePtr;
#endif
