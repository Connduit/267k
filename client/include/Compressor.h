/* Compressor.h */

class Compressor
{
public:
	virtual void compress();
	virtual void decompress();
private:
};


typedef std::unique_ptr<Compressor> CompressorUniquePtr;
