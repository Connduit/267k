/* MessageHandler.h */

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

class MessageHandler
{
public:
    MessageHandler(
            CompressorUniquePtr compressorPtr,
            EncrypterUniquePtr encrypterPtr,
            EncoderUniquePtr encoderPtr,
            SerializerUniquePtr serializerPtr,
            Config* config);
    
    bool sendMessage(); // overload this function?
    bool recvMessage(); // overload this function?
            
private:
    CompressorUniquePtr compressorPtr_;
    EncrypterUniquePtr encrypterPtr_;
    EncoderUniquePtr encoderPtr_;
    SerializerUniquePtr serializerPtr_;
    Config config_;
};

