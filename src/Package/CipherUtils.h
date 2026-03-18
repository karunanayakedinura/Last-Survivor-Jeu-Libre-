#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#ifndef ENGINE_API
    #define ENGINE_API
#endif

namespace Engine::Systems::CipherUtils {
    class ENGINE_API Cipher {
    public:
        // Constants for the encrypted file format
        static constexpr size_t SALT_SIZE = 16;      // 128-bit Salt
        static constexpr size_t IV_SIZE = 16;        // 128-bit AES Block
        static constexpr size_t HMAC_SIZE = 32;      // SHA-256 Output
        static constexpr size_t KEY_SIZE = 32;       // AES-256
        static constexpr size_t PBKDF2_ITERATIONS = 600000; // High iteration count for security

        /**
         * @brief Encrypts data using AES-256-CBC + HMAC-SHA256 + PBKDF2.
         * * Format: [Salt (16)] [IV (16)] [HMAC (32)] [Ciphertext (padded)]
         * * @param data Raw binary data.
         * @param passphrase User password (will be salted and hashed).
         * @return std::vector<unsigned char> Encrypted blob.
         */
        static std::vector<unsigned char> EncryptAES256(const std::vector<unsigned char>& data, const std::string& passphrase);

        /**
         * @brief Decrypts data verified by HMAC-SHA256.
         * Throws std::runtime_error if integrity check fails or password is wrong.
         * * @param data Encrypted blob.
         * @param passphrase User password.
         * @return std::vector<unsigned char> Decrypted raw data.
         */
        static std::vector<unsigned char> DecryptAES256(const std::vector<unsigned char>& data, const std::string& passphrase);



        // NEW: Expose Key Derivation
        static std::vector<unsigned char> DeriveKey(const std::string& passphrase, const std::vector<unsigned char>& salt);

        // NEW: Low-level chunk decryption (AES-CBC)
        // keyBlob: The result of DeriveKey
        // iv: The 16-byte initialization vector for this specific chunk
        // inData/outData: Pointers to buffer (size must be multiple of 16)
        static void DecryptChunkAES256(const std::vector<unsigned char>& keyBlob, const std::vector<unsigned char>& iv, const void* inData, void* outData, size_t size);
    };
}