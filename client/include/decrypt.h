/* decrypt.h */

#include <openssl/evp.h> // AES/RSA
#include <openssl/rand.h>
#include <openssl/rsa.h> // RSA
#include <openssl/pem.h> // RSA
#include <stdlib.h>


/*
 * Decrypt using AES-256-GCM
 * Returns plaintext length on success, -1 on authentication failure
 */
int decrypt_aes_256_gcm(const unsigned char *ciphertext, int ciphertext_len,
		const unsigned char *key, const unsigned char *iv,
		const unsigned char *tag, unsigned char *plaintext) 
{

	EVP_CIPHER_CTX *ctx;
	int len;
	int plaintext_len;
	int ret;

	/* Create and initialize context */
	if(!(ctx = EVP_CIPHER_CTX_new())) {
		// TODO: throw error
	}

	/* Initialize decryption operation */
	if(!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL)) {
		EVP_CIPHER_CTX_free(ctx);
		return -1;

	}

	/* Set IV length */
	if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, NULL)) {
		EVP_CIPHER_CTX_free(ctx);
		return -1;

	}

	/* Initialize key and IV */
	if(!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) {
		EVP_CIPHER_CTX_free(ctx);
		return -1;

	}

	/* Provide the ciphertext to be decrypted */
	if(!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
		EVP_CIPHER_CTX_free(ctx);
		return -1;

	}
	plaintext_len = len;

	/* Set expected authentication tag before finalizing */
	if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, (void*)tag)) {
		EVP_CIPHER_CTX_free(ctx);
		return -1;

	}

	/*
	 * Finalize decryption.
	 * This will return 0 if the tag verification fails
	 */
	ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
	EVP_CIPHER_CTX_free(ctx);

	if(ret > 0) {
		/* Success - tag verified */
		plaintext_len += len;
		return plaintext_len;

	} else {
		/* Authentication failed - message was tampered with */
		return -1;

	}

}

// RSA
/*
 * Decrypt data using RSA private key
 */
int rsa_decrypt(EVP_PKEY *private_key,
		const unsigned char *ciphertext, int ciphertext_len,
		unsigned char **plaintext) 
{
	EVP_PKEY_CTX *ctx;
	size_t plaintext_len;

	ctx = EVP_PKEY_CTX_new(private_key, NULL);
	if (!ctx) return -1;

	if (EVP_PKEY_decrypt_init(ctx) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;

	}

	if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;

	}

	/* Determine buffer size */
	if (EVP_PKEY_decrypt(ctx, NULL, &plaintext_len, ciphertext, ciphertext_len) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;

	}

	*plaintext = malloc(plaintext_len);
	if (!*plaintext) {
		EVP_PKEY_CTX_free(ctx);
		return -1;

	}

	/* Perform decryption */
	if (EVP_PKEY_decrypt(ctx, *plaintext, &plaintext_len, 
				ciphertext, ciphertext_len) <= 0) {
		free(*plaintext);
		EVP_PKEY_CTX_free(ctx);
		return -1;

	}

	EVP_PKEY_CTX_free(ctx);
	return plaintext_len;

}
