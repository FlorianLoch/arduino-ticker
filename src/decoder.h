#include <Arduino.h>
#include <AES.h>
#include <MD5.h>
#include "helper.h"

struct message_t
{
  String type; // currently there is only "msg" for "message" and "err" in case of an error during decoding
  String payload;
};

class Decoder
{
private:
  uint8_t *key;
  String keyStr;
  AES aes;

  void unescape(String &str)
  {
    str.replace("\\\\", "\\");
    str.replace("\\n", "\n");
  };

  bool validateHMAC(uint8_t *buffer, size_t length, uint8_t *hmac)
  {
    uint8_t *bufferAndKey = (uint8_t *)malloc(length + this->keyStr.length());
    memcpy(bufferAndKey, buffer, length);
    this->keyStr.getBytes(bufferAndKey + length, this->keyStr.length() + 1); // TODO Check this is a bug that needs us to + 1 altough length() should be just fine

    // Serial.println("Key");
    // printHex(bufferAndKey, length + this->keyStr.length());

    uint8_t *hash = (uint8_t *)MD5::make_hash((char *)bufferAndKey, length + this->keyStr.length());

    // Serial.println("Memcmp");
    // Serial.println(memcmp(hash, hmac, 16));

    return memcmp(hash, hmac, 16) == 0;
  };

public:
  Decoder(String key) : keyStr{key}
  {
    this->key = (uint8_t *)MD5::make_hash((char *)key.c_str());
  };

  message_t decode(uint8_t *encoded, size_t length)
  {
    if (length % 16 != 0)
    {
      return message_t{"err", "Cannot decode buffer because its length isn't a multiple of 16 bytes."};
    }

    uint8_t *decrypted = decryptAES128CBC(encoded, length);

    uint8_t *hmac = decrypted + length - 16 - 16;

    if (!(this->validateHMAC(decrypted, length - 16 - 16, hmac)))
    {
      // TODO throw an exception etc instead to prevent denial of service... Or simply an empty struct
      return message_t{"err", "HMAC of received message invalid."};
    }

    String decryptedStr = String((char *)decrypted);

    size_t firstNL = decryptedStr.indexOf('\n');
    size_t secondNL = decryptedStr.indexOf('\n', firstNL + 1);

    if (firstNL == -1 || secondNL == -1)
    {
      return message_t{"err", "Did not find two sections in decrypted message."};
    }

    message_t msg;
    msg.type = decryptedStr.substring(0, firstNL);
    msg.payload = decryptedStr.substring(firstNL + 1, secondNL);
    this->unescape(msg.type);
    this->unescape(msg.payload);

    return msg;
  };

  uint8_t *decryptAES128CBC(uint8_t *buffer, size_t length)
  {
    //uint8_t* iv = buffer;
    uint8_t *cipher = buffer + 16; // first 16 bytes contain the initialization vector
    uint8_t *decrypted = new uint8_t[length - 16 /* 128 bit block size; first bytes are iv */];
    uint8_t *iv = (uint8_t *)buffer;

    aes.do_aes_decrypt(cipher, length - 16, decrypted, this->key, 128, iv);

    return decrypted;
  };
};
