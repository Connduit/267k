/* decode.h */
// break decoding across different functions to make it harder to detect?
// maybe i don't have to worry about b64 being easy to detect cuz
// it's ok for av to know we're sending a message? just have to make 
// the message/sending look like a normal program

// https://github.com/Adaptix-Framework/AdaptixC2/blob/main/Extenders/agent_beacon/src_beacon/beacon/Encoders.cpp

#include <stdint.h>

// Simple LUT as a string (very obvious in binary)
const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; // default_table

// TODO: pretty code water way of encoding
int base64_decode(const char *in, uint8_t *out) {
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
