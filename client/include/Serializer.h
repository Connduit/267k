/* Serializer.h */

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "MessageTypes.h"

#include <vector>
#include <memory>

class Serializer
{
public:
	virtual ~Serializer() = default;
	virtual std::vector<uint8_t> serialize(const InternalMessage& msg) = 0;
	virtual InternalMessage deserialize(const std::vector<uint8_t>& data) = 0;
private:
};


class BinarySerializer : public Serializer
{
public:
	std::vector<uint8_t> serialize(const InternalMessage& msg);
	InternalMessage deserialize(const std::vector<uint8_t>& msg);
private:


    template<typename T>
    void append_bytes(std::vector<uint8_t>& buffer, const T& value)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
        buffer.insert(buffer.end(), bytes, bytes + sizeof(T));
    }

    template<typename T>
    T read_bytes(const std::vector<uint8_t>& data, size_t& offset)
    {
        T value;
        memcpy(&value, data.data() + offset, sizeof(T));
        return value;
    }

    uint16_t calculate_checksum(const std::vector<uint8_t>& data)
    {
        uint16_t checksum = 0;
        for (uint8_t byte : data)
        {
            checksum += byte;
        }
        return checksum;
    }
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
