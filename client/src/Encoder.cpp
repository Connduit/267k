/* Encoder.cpp */
/* encode json into base64 */
/* encoding is needed for http, smtp, json, xml, html, urls,  (but not tcp) */
/* tcp/udp, smb, dns, ftp, and other binary traffic protocols don't need encoding */


/* define differnt encoding formats in here... start with TLV */


// TODO:
// char table eventually obfuscated by maybe building the table dynamically at runtime,
// splitting the table into separate parts and then reconstructing them at runtime,
// encrypting the table, or mathematically generating the characters for the table?
// https://github.com/Adaptix-Framework/AdaptixC2/blob/main/Extenders/agent_beacon/src_beacon/beacon/Encoders.cpp

#include <Encoder.h>
// base64 or hex encoding
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; // default_table

// if i want to rewrite this with the input length unknown, will have to use strlen... 
// hopefully by then ill have my own strlen func written
char* B64Encoder::encode(const unsigned char* data, size_t input_len)
{
	size_t output_len = 4 * ((input_len + 2) / 3);
	char* encoded = (char *)malloc(output_len + 1);
	if (!encoded)
	{
		return NULL;
	}	

	for (size_t i = 0, j = 0; i < input_len;) {
		// Get three bytes
		uint32_t octet_a = i < input_len ? data[i++] : 0;
		uint32_t octet_b = i < input_len ? data[i++] : 0;
		uint32_t octet_c = i < input_len ? data[i++] : 0;

		// Combine into 24-bit number
		uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

		// Split into four 6-bit indices
		encoded[j++] = b64_table[(triple >> 18) & 0x3F];
		encoded[j++] = b64_table[(triple >> 12) & 0x3F];
		encoded[j++] = b64_table[(triple >> 6) & 0x3F];
		encoded[j++] = b64_table[triple & 0x3F];
	}

	// Add padding for incomplete triplets
	if (input_len % 3 == 1) 
	{
		encoded[output_len - 1] = '=';
		encoded[output_len - 2] = '=';
	} 
	else if (input_len % 3 == 2) 
	{
		encoded[output_len - 1] = '=';
	}

	encoded[output_len] = '\0';
	return encoded;
}



/* decode.h */
// break decoding across different functions to make it harder to detect?
// maybe i don't have to worry about b64 being easy to detect cuz
// it's ok for av to know we're sending a message? just have to make 
// the message/sending look like a normal program

// https://github.com/Adaptix-Framework/AdaptixC2/blob/main/Extenders/agent_beacon/src_beacon/beacon/Encoders.cpp

#include <stdint.h>

// Simple LUT as a string (very obvious in binary)
//const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; // default_table

// TODO: pretty code water way of encoding
int B64Encoder::decode(const char *in, uint8_t *out) {
	int i = 0, j = 0;
	uint8_t v[4];

	while (in[i] && in[i+1] && in[i+2] && in[i+3]) 
	{
		// Convert chars to values
		for (int k = 0; k < 4; k++) 
		{
			const char *pos = strchr(b64_table, in[i+k]);
			v[k] = pos ? (pos - b64_table) : 0;
		}

		// Reconstruct bytes
		out[j++] = (v[0] << 2) | (v[1] >> 4);
		out[j++] = (v[1] << 4) | (v[2] >> 2);
		out[j++] = (v[2] << 6) | v[3];

		i += 4;
	}
	return j;
}
