/* Serializer.h */

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "MessageTypes.h"

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

typedef std::unique_ptr<Serializer> SerializerUniquePtr;

#endif
