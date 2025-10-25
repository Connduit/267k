/* Encryptor.h */
/* encryption is used to protect sensitive user information. ensures only authorized users can read it. keeps communication and messages private */

// aes (symmetric) or rsa (asymmetric)?

// https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197-upd1.pdf
// void/int
/*
int aes128_encrypt(data, key, result)
{
    
}
*/

//aes256_encrypt() -> iv + key + plaintext = ciphertext + tag

//rsa_encrypt()
#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

// NOTE: for now just use openssl headers for encryption

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/rsa.h> // RSA
#include <openssl/pem.h> // RSA

#include <vector>
#include <string>

// TODO: move this struct somewhere else?
typedef struct {
	unsigned char ciphertext[1024];
	unsigned char tag[16];  // Authentication tag
	int ciphertext_len;
} encrypted_message;


#include <memory>


class Encryptor
{
public:
	// constructor
	// deconstructor
	// TODO: child classes need to take in the same parameters
	//virtual bool encrypt() = 0;
	//virtual bool decrypt() = 0;
	//virtual ~Encryptor() = default; // TODO: what does default do?
private:
};

typedef std::unique_ptr<Encryptor> EncryptorUniquePtr;

/*
class AesEncryptor : public Encryptor
{
public:
	bool encrypt (
		const unsigned char* plaintext, 
		int plaintext_len, 
		const unsigned char* key, 
		const unsigned char* iv, 
		encrypted_message* output); // override;

	bool decrypt(
		const unsigned char* ciphertext, 
		int ciphertext_len, 
		const unsigned char* key, 
		const unsigned char* iv, 
		const unsigned char* tag, 
		unsigned char* plaintext);
private:
};

class RsaEncryptor : public Encryptor
{
public:
	bool encrypt(EVP_PKEY* public_key, const unsigned char* plaintext, int plaintext_len, unsigned char** ciphertext);
	bool decrypt(EVP_PKEY* private_key,
		const unsigned char* ciphertext, int ciphertext_len,
		unsigned char** plaintext);
private:
};*/

class XorEncryptor : public Encryptor
{
public:
	//XorEncryptor();
	XorEncryptor(const std::vector<uint8_t>& key) : key_(key) {} // TODO: is const needed?
	XorEncryptor(const std::string& key) : key_(key.begin(), key.end()) {}
	bool encrypt(const std::vector<uint8_t>& data, std::vector<uint8_t>& cipher);
	bool decrypt(const std::vector<uint8_t>& cipher, std::vector<uint8_t>& data);
private:
	//const std::vector<uint8_t> key_; // change to reference?
	const std::vector<uint8_t> key_; // change to reference?
};

#endif
