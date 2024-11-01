﻿#include <boss.h>

#if !defined(BOSS_NEED_ADDON_SSL) || (BOSS_NEED_ADDON_SSL != 0 && BOSS_NEED_ADDON_SSL != 1)
    #error BOSS_NEED_ADDON_SSL macro is invalid use
#endif
bool __LINK_ADDON_SSL__() {return true;} // 링크옵션 /OPT:NOREF가 안되서 임시코드

#if BOSS_NEED_ADDON_SSL

#include "boss_addon_ssl.hpp"

extern "C"
{
    #include <addon/openssl-1.1.1a_for_boss/include/openssl/bio.h>
    #include <addon/openssl-1.1.1a_for_boss/include/openssl/buffer.h>
    #include <addon/openssl-1.1.1a_for_boss/include/openssl/crypto.h>
    #include <addon/openssl-1.1.1a_for_boss/include/openssl/aes.h>
    #include <addon/openssl-1.1.1a_for_boss/include/openssl/evp.h>
    #include <addon/openssl-1.1.1a_for_boss/include/openssl/err.h>
    #include <addon/openssl-1.1.1a_for_boss/include/openssl/ssl.h>
    #include <addon/openssl-1.1.1a_for_boss/include/openssl/rand.h>
}

#include <boss.hpp>
#include <platform/boss_platform.hpp>

// 등록과정
namespace BOSS
{
    BOSS_DECLARE_ADDON_FUNCTION(Ssl, CreateMD5, id_md5, void)
    BOSS_DECLARE_ADDON_FUNCTION(Ssl, ReleaseMD5, chars, id_md5)
    BOSS_DECLARE_ADDON_FUNCTION(Ssl, UpdateMD5, void, id_md5, bytes, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Ssl, ToMD5, chars, bytes, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Ssl, ToSHA256, chars, bytes, sint32, bool)
    BOSS_DECLARE_ADDON_FUNCTION(Ssl, ToBASE64, chars, bytes, sint32)
    BOSS_DECLARE_ADDON_FUNCTION(Ssl, FromBASE64, buffer, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Ssl, ToAES128, chars, bytes, sint32, bool, chars, chars)
    BOSS_DECLARE_ADDON_FUNCTION(Ssl, GenUUID, chars, void)

    static autorun Bind_AddOn_Ssl()
    {
        Core_AddOn_Ssl_CreateMD5() = Customized_AddOn_Ssl_CreateMD5;
        Core_AddOn_Ssl_ReleaseMD5() = Customized_AddOn_Ssl_ReleaseMD5;
        Core_AddOn_Ssl_UpdateMD5() = Customized_AddOn_Ssl_UpdateMD5;
        Core_AddOn_Ssl_ToMD5() = Customized_AddOn_Ssl_ToMD5;
        Core_AddOn_Ssl_ToSHA256() = Customized_AddOn_Ssl_ToSHA256;
        Core_AddOn_Ssl_ToBASE64() = Customized_AddOn_Ssl_ToBASE64;
        Core_AddOn_Ssl_FromBASE64() = Customized_AddOn_Ssl_FromBASE64;
        Core_AddOn_Ssl_ToAES128() = Customized_AddOn_Ssl_ToAES128;
        Core_AddOn_Ssl_GenUUID() = Customized_AddOn_Ssl_GenUUID;
        return true;
    }
    static autorun _ = Bind_AddOn_Ssl();
}

// 구현부
namespace BOSS
{
    size_t BASE64Class::CalcedDecodeLength(const char* b64input)
    {
        size_t len = boss_strlen(b64input), padding = 0;
        if(b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
            padding = 2;
        else if(b64input[len-1] == '=') //last char is =
            padding = 1;
        return (len * 3) / 4 - padding;
    }

    buffer BASE64Class::Base64Decode(chars b64message)
    {
        BIO *bio, *b64;
        int decodeLen = CalcedDecodeLength(b64message);
        buffer Result = Buffer::Alloc(BOSS_DBG decodeLen);

        bio = BIO_new_mem_buf(b64message, -1);
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_push(b64, bio);

        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
        int decodeLen2 = BIO_read(bio, Result, boss_strlen(b64message));
        //length should equal decodeLen, else something went horribly wrong
        BOSS_ASSERT("Base64Decode의 치명적인 오류입니다", decodeLen == decodeLen2);
        BIO_free_all(bio);
        return Result;
    }

    String BASE64Class::Base64Encode(bytes data, size_t length)
    {
        BIO *bio, *b64;
        BUF_MEM* bufferPtr;

        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);

        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
        BIO_write(bio, data, length);
        BIO_flush(bio);
        BIO_get_mem_ptr(bio, &bufferPtr);
        BIO_set_close(bio, BIO_NOCLOSE);
        BIO_free_all(bio);
        return String((chars) bufferPtr->data, bufferPtr->length);
    }

    id_md5 Customized_AddOn_Ssl_CreateMD5(void)
    {
        MD5Class* NewMD5 = new MD5Class();

        if(!MD5_Init(&NewMD5->mCTX))
        {
            delete NewMD5;
            return nullptr;
        }

        return (id_md5) NewMD5;
    }

    chars Customized_AddOn_Ssl_ReleaseMD5(id_md5 md5)
    {
        MD5Class* OldMD5 = (MD5Class*) md5;
        if(!OldMD5) return "";

        MD5_Final(OldMD5->mDigest, &OldMD5->mCTX);
        OPENSSL_cleanse(&OldMD5->mCTX, sizeof(OldMD5->mCTX));

        static char Result[2 * MD5_DIGEST_LENGTH + 1];
        for(sint32 i = 0; i < MD5_DIGEST_LENGTH; ++i)
            boss_snprintf(&Result[2 * i], 3, "%02x", (uint32) OldMD5->mDigest[i]);
        delete OldMD5;
        return Result;
    }

    void Customized_AddOn_Ssl_UpdateMD5(id_md5 md5, bytes binary, sint32 length)
    {
        MD5Class* CurMD5 = (MD5Class*) md5;
        if(!CurMD5) return;

        MD5_Update(&CurMD5->mCTX, binary, length);
    }

    chars Customized_AddOn_Ssl_ToMD5(bytes binary, sint32 length)
    {
        unsigned char Digest[MD5_DIGEST_LENGTH];
        MD5(binary, length, Digest);

        static char Result[2 * MD5_DIGEST_LENGTH + 1];
        for(sint32 i = 0; i < MD5_DIGEST_LENGTH; ++i)
            boss_snprintf(&Result[2 * i], 3, "%02x", (uint32) Digest[i]);
        return Result;
    }

    chars Customized_AddOn_Ssl_ToSHA256(bytes binary, sint32 length, bool base64)
    {
        unsigned char Digest[SHA256_DIGEST_LENGTH];
        SHA256(binary, length, Digest);

        if(base64)
        {
            static String Result;
            Result = BASE64Class::Base64Encode(Digest, SHA256_DIGEST_LENGTH);
            return Result;
        }

        static char Result[2 * SHA256_DIGEST_LENGTH + 1];
        for(sint32 i = 0; i < SHA256_DIGEST_LENGTH; ++i)
            boss_snprintf(&Result[2 * i], 3, "%02x", (uint32) Digest[i]);
        return Result;
    }

    chars Customized_AddOn_Ssl_ToBASE64(bytes binary, sint32 length)
    {
        static String Result;
        Result = BASE64Class::Base64Encode(binary, length);
        return Result;
    }

    buffer Customized_AddOn_Ssl_FromBASE64(chars base64)
    {
        return BASE64Class::Base64Decode(base64);
    }

    chars Customized_AddOn_Ssl_ToAES128(bytes binary, sint32 length, bool base64, chars key, chars iv)
    {
        EVP_CIPHER_CTX ctx;
        memset(&ctx, 0, sizeof(EVP_CIPHER_CTX));

        EVP_CIPHER_CTX_init(&ctx);
	    EVP_EncryptInit(&ctx, EVP_aes_128_cbc(), (bytes) key, (bytes) iv);

        sint32 BufferSize = length + EVP_MAX_BLOCK_LENGTH;
        uint08* Cipher = new uint08[BufferSize];
        memset(Cipher, 0, length);

        sint32 UpdateLength = 0, FinalUpdate = 0;
        EVP_EncryptUpdate(&ctx, Cipher, &UpdateLength, binary, length);
	    EVP_EncryptFinal(&ctx, Cipher + UpdateLength, &FinalUpdate);
        const sint32 CipherSize = UpdateLength + FinalUpdate;
	    EVP_CIPHER_CTX_cleanup(&ctx);

        if(base64)
        {
            static String Result;
            Result = BASE64Class::Base64Encode(Cipher, CipherSize);
            delete[] Cipher;
            return Result;
        }

        static String Result;
        for(sint32 i = 0; i < CipherSize; ++i)
        {
            char OneHex[3];
            boss_snprintf(OneHex, 3, "%02x", (uint32) Cipher[i]);
            Result += OneHex;
        }
        delete[] Cipher;
        return Result;
    }

    chars Customized_AddOn_Ssl_GenUUID(void)
    {
        static String Result;
        union
        {
            struct
            {
                uint32_t mTimeLow;
                uint16_t mTimeMid;
                uint16_t mTimeHighAndVersion;
                uint8_t mClkSeqHighRes;
                uint8_t mClkSeqLow;
                uint8_t mNode[6];
            };
            uint8_t mRnd[16];
        } NewUuid;

        if(RAND_bytes(NewUuid.mRnd, sizeof(NewUuid)) != 0)
        {
            // Refer Section 4.2 of RFC-4122
            // https://tools.ietf.org/html/rfc4122#section-4.2
            NewUuid.mClkSeqHighRes = (uint8_t) ((NewUuid.mClkSeqHighRes & 0x3F) | 0x80);
            NewUuid.mTimeHighAndVersion = (uint16_t) ((NewUuid.mTimeHighAndVersion & 0x0FFF) | 0x4000);
            Result = String::Format("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                NewUuid.mTimeLow, NewUuid.mTimeMid, NewUuid.mTimeHighAndVersion,
                NewUuid.mClkSeqHighRes, NewUuid.mClkSeqLow,
                NewUuid.mNode[0], NewUuid.mNode[1], NewUuid.mNode[2],
                NewUuid.mNode[3], NewUuid.mNode[4], NewUuid.mNode[5]);
        }
        else Result.Empty();
        return Result;
    }
}

#endif
