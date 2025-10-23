#include "MessagePublisher.h"

MessagePublisher::MessagePublisher(
		Encryptor& encryptor,
		Encoder& encoder,
		Serializer& serializer)
		:
		encryptor_(encryptor),
		encoder_(encoder),
		serializer_(serializer)
{}
