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

//#include <iostream>

//const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; // default_table
const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; // default_table

// if i want to rewrite this with the input length unknown, will have to use strlen... 
// hopefully by then ill have my own strlen func written
//char* B64Encoder::encode(const unsigned char* data, size_t input_len)
bool B64Encoder::encode(std::vector<uint8_t>& inMsg, std::string& outMsg)
{
	outMsg.clear();
	int val = 0, valb = -6;
	for (uint8_t c : inMsg) 
	{
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) 
		{
			outMsg.push_back(base64_chars[(val >> valb) & 0x3F]);
			valb -= 6;
		}

	}

	if (valb > -6) 
	{
		outMsg.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
	}

	while (outMsg.size() % 4) 
	{
		outMsg.push_back('=');
	}
	return false;
}



/* decode.h */
// break decoding across different functions to make it harder to detect?
// maybe i don't have to worry about b64 being easy to detect cuz
// it's ok for av to know we're sending a message? just have to make 
// the message/sending look like a normal program

// https://github.com/Adaptix-Framework/AdaptixC2/blob/main/Extenders/agent_beacon/src_beacon/beacon/Encoders.cpp

//#include <stdint.h>

// Simple LUT as a string (very obvious in binary)
//const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; // default_table

// TODO: pretty code water way of encoding
bool B64Encoder::decode(std::string& inMsg, std::vector<uint8_t>& outMsg)
{
	outMsg.clear();
	std::vector<int> T(256, -1);
	for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

	int val = 0, valb = -8;
	for (unsigned char c : inMsg) {
		if (T[c] == -1) {
			if (c == '=') break;
			//throw std::invalid_argument("Invalid Base64 character");
			// TODO:

		}
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0) {
			outMsg.push_back((val >> valb) & 0xFF);
			valb -= 8;

		}

	}
}
