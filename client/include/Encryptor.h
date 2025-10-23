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
	virtual ~Encryptor() = default; // TODO: what does default do?
	virtual void encrypt();
	virtual void decrypt();
private:
};

typedef std::unique_ptr<Encryptor> EncryptorUniquePtr;

class AesEncryptor : public Encryptor
{
public:
private:
};


class XorEncryptor : public Encryptor
{
public:
private:
};

#endif
