/* Serializer.h */

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <memory>
class Serializer
{
public:
	virtual void serialize();
	virtual void deserialize();
private:
};

typedef std::unique_ptr<Serializer> SerializerUniquePtr;

#endif
