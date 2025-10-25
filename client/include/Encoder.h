/* Encoder.h */
#ifndef ENCODER_H
#define ENCODER_H

#include <string>
#include <cstdint>
#include <memory>
#include <vector>

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
	//bool encode(const std::vector<uint8_t>& inMsg, std::vector<uint8_t>& outMsg);
	bool encode(std::vector<uint8_t>& inMsg, std::string& outMsg);
	//bool decode(const std::vector<uint8_t>& inMsg, std::vector<uint8_t>& outMsg);
	bool decode(std::string& inMsg, std::vector<uint8_t>& outMsg);
private:
};

//class HexEncoder : public Encoder

typedef std::unique_ptr<Encoder> EncoderUniquePtr;
#endif
