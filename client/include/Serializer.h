/* Serializer.h */

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "MessageTypes.h"

#include <vector>
#include <memory>

class Serializer
{
public:
	//virtual bool serialize();
	//virtual bool deserialize();
	bool serialize() { return true; };
	bool deserialize() { return true; };
private:
};


class BinarySerializer : public Serializer
{
public:
	bool serialize(const InternalMessage& msg, std::vector<uint8_t>& outMsg);
	bool deserialize(const std::vector<uint8_t>& msg, InternalMessage& outMsg);
private:
};

/*
class JsonSerializer : public Serializer
{
public:
	bool serialize() { return true; };
	bool deserialize() { return true; };
private:
};
*/

// TODO: custom Serializer class
typedef std::unique_ptr<Serializer> SerializerUniquePtr;

#endif
