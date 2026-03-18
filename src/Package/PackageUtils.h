#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace Engine::Systems::PackageUtils {
    struct PakEntry {
        uint64_t offset;
        uint64_t size;
        uint64_t compressedSize;
    };

    // Abstract interface for a readable stream
    class IFileStream {
    public:
        virtual ~IFileStream() = default;
        virtual size_t Read(void* buffer, size_t size) = 0;
        virtual bool Seek(int64_t offset, int origin) = 0; // origin: 0=SET, 1=CUR, 2=END
        virtual size_t Tell() = 0;
        virtual size_t Size() = 0;
        virtual bool IsValid() = 0;
    };

    class PakFileStream : public IFileStream {
        std::ifstream m_File;
        PakEntry m_Entry;
        std::vector<unsigned char> m_KeyBlob;
        std::vector<unsigned char> m_BaseIV; // The IV from the file header
        size_t m_DataStartOffset; // Physical offset where encrypted data starts
        size_t m_CurrentPos = 0;

    public:
        PakFileStream(const std::string& pakPath, const PakEntry& entry, const std::string& password);
        size_t Read(void* buffer, size_t size) override;
        bool Seek(int64_t offset, int origin) override;
        size_t Tell() override { return m_CurrentPos; }
        size_t Size() override { return m_Entry.size; } // Uncompressed/Decrypted size
        bool IsValid() override { return m_File.is_open(); }
    };
} // namespace Engine::Systems::PackageUtils