/* Encrypter.cpp */

#include "Encryptor.h"

int encrypt_aes_256_gcm(const unsigned char *plaintext, int plaintext_len,
		const unsigned char *key, const unsigned char *iv,
		encrypted_message *output) {
	EVP_CIPHER_CTX *ctx;
	int len;

	if(!(ctx = EVP_CIPHER_CTX_new()))
		return -1;

	// Initialize encryption
	if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
		return -1;

	// Set IV length
	if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, NULL))
		return -1;

	// Initialize key and IV
	if(1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv))
		return -1;

	// Encrypt
	if(1 != EVP_EncryptUpdate(ctx, output->ciphertext, &len, plaintext, plaintext_len))
		return -1;
	output->ciphertext_len = len;

	// Finalize
	if(1 != EVP_EncryptFinal_ex(ctx, output->ciphertext + len, &len))
		return -1;
	output->ciphertext_len += len;

	// Get authentication tag
	if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, output->tag))
		return -1;

	EVP_CIPHER_CTX_free(ctx);
	return 0;

}

// RSA
/*
 * Encrypt data using RSA public key
 */
int rsa_encrypt(EVP_PKEY *public_key, 
		const unsigned char *plaintext, int plaintext_len,
		unsigned char **ciphertext) 
{
	EVP_PKEY_CTX *ctx;
	size_t ciphertext_len;

	/* Create context */
	ctx = EVP_PKEY_CTX_new(public_key, NULL);
	if (!ctx) return -1;

	/* Initialize encryption */
	if (EVP_PKEY_encrypt_init(ctx) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;

	}

	/* Set padding (OAEP recommended) */
	if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;

	}

	/* Determine buffer size */
	if (EVP_PKEY_encrypt(ctx, NULL, &ciphertext_len, plaintext, plaintext_len) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;

	}

	/* Allocate ciphertext buffer */
	*ciphertext = malloc(ciphertext_len);
	if (!*ciphertext) {
		EVP_PKEY_CTX_free(ctx);
		return -1;

	}

	/* Perform encryption */
	if (EVP_PKEY_encrypt(ctx, *ciphertext, &ciphertext_len, 
				plaintext, plaintext_len) <= 0) {
		free(*ciphertext);
		EVP_PKEY_CTX_free(ctx);
		return -1;

	}

	EVP_PKEY_CTX_free(ctx);
	return ciphertext_len;
}
