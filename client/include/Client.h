#include "Compressor.h"
#include "C2Profile.h"
#include "Encoder.h"
#include "Encrypter.h"
#include "Serializer.h"

#include <memory>

typedef std::unique_ptr<Compressor> CompressorUniquePtr;
typedef std::unique_ptr<Encoder> EncoderUniquePtr;
typedef std::unique_ptr<Encrypter> EncrypterUniquePtr;
typedef std::unique_ptr<Serializer> SerializerUniquePtr;

class Client
{
public:
    Client(Config config);
    /*Client(
            CompressorUniquePtr compressorPtr,
            EncrypterUniquePtr encrypterPtr,
            EncoderUniquePtr encoderPtr,
            SerializerUniquePtr serializerPtr,
            Config* config);*/
private:
    CompressorUniquePtr compressorPtr_;
    EncrypterUniquePtr encrypterPtr_;
    EncoderUniquePtr encoderPtr_;
    SerializerUniquePtr serializerPtr_;
    MessageConsumer messageConsumer_;
    MessagePublisher messagePublisher_;
    // MessageHandler messageHandler_; 
    Config config_;
};
