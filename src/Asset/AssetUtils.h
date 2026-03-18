#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>

namespace Engine::Assets::AssetsUtils {
    using AssetID = uint64_t;

    // Unique identifier for asset types
    enum class AssetType : uint32_t {
        None = 0,
        Texture,
        StaticMesh,
        SkeletalMesh, 
        Animation,    
        Material,     
        Audio,
        Prefab,
        Skeleton
    };

    const std::vector<std::pair<AssetType, std::string>> classAssetTypeToString {
        {AssetType::None, "None"},
        {AssetType::Texture, "Texture"},
        {AssetType::StaticMesh, "StaticMesh"},
        {AssetType::SkeletalMesh, "SkeletalMesh"}, 
        {AssetType::Animation, "Animation"},    
        {AssetType::Material, "Material"},     
        {AssetType::Audio, "Audio"},
        {AssetType::Prefab, "Prefab"},
        {AssetType::Skeleton, "Skeleton"}
    };

    // -------------------------------------------------------------------------
    // FILE LAYOUT (Binary Format)
    // -------------------------------------------------------------------------
    #pragma pack(push, 1)

    struct AssetHeader {
        char signature[4] = {'P', 'A', 'S', '1'}; 
        uint32_t version = 2;
        AssetID assetId;        
        AssetType type;         
        uint32_t compressionMode; // 0 = None, 1 = LZ4
        uint64_t payloadSize;     
        uint64_t uncompressedSize; 

        // --- NEW: PREVIEW METADATA ---
        uint64_t previewSize = 0;      // Size of the raw RGBA preview blob appended at EOF
        uint64_t previewOffset = 0;    // Absolute offset to the preview blob
        uint32_t previewWidth = 0;
        uint32_t previewHeight = 0;
    };

    struct TextureHeader {
        uint32_t width;
        uint32_t height;
        uint32_t format;    
        uint32_t mipCount;  
        uint32_t mipOffsets[14]; 
        uint32_t mipSizes[14];
    };

    struct ModelHeader {
        uint32_t meshCount;
        uint32_t materialCount;
        float aabbMin[3];
        float aabbMax[3];
        float centroid[3];
    };
    
    struct SkeletonHeader {
        uint32_t nodeCount;
    };

    struct MeshEntryHeader {
        uint32_t vertexCount;
        uint32_t indexCount;
        uint32_t materialIndex; 
        uint32_t boneCount;      
        uint64_t vertexDataOffset; 
        uint64_t indexDataOffset;  
    };

    struct NodeDataHeader {
        int32_t parentIndex; 
        float transform[16]; 
        uint32_t nameLength;
    };

    struct MaterialHeader {
        float baseColor[4];
        float metallic;
        float roughness;
        float emissive;
        // Replaced AssetID with 128-byte char arrays for paths
        char albedoPath[128];
        char normalPath[128];
        char metallicRoughnessPath[128];
        char emissivePath[128];
    };
    
    struct AudioHeader {
        uint32_t sampleRate;
        uint32_t channels;
        uint32_t format;        // 0 = PCM
        uint32_t bitsPerSample; // <--- ADDED: e.g. 32 for Float
        uint64_t sampleCount;
        float duration;
    };

    struct AnimationClipHeader {
        float duration;
        float ticksPerSecond;
        uint32_t channelCount;
        uint32_t nameLength;
    };

    struct AnimationChannelHeader {
        uint32_t nodeIndex; 
        uint32_t positionKeyCount;
        uint32_t rotationKeyCount;
        uint32_t scalingKeyCount;
    };

    struct VectorKey {
        double time;
        float value[3];
    };

    struct QuatKey {
        double time;
        float value[4];
    };

    #pragma pack(pop)

    // --- ADDED: Missing CPU data holders for complete deserialization ---
    struct CPUMaterialData {
        glm::vec4 baseColor;
        float metallic;
        float roughness;
        float emissive;
        // Use std::string for CPU side convenience
        std::string albedoPath;
        std::string normalPath;
        std::string metallicRoughnessPath;
        std::string emissivePath;
    };

    struct CPUAudioData {
        uint32_t sampleRate;
        uint32_t channels;
        uint32_t format;
        uint32_t bitsPerSample;
        uint64_t sampleCount;
        float duration;
        std::vector<float> pcmData; // Assuming Float32 PCM
    };

    struct CPUSkeletonData {
        struct Node {
            std::string name;
            glm::mat4 transform;
            int parentIndex;
        };
        std::vector<Node> nodes;
    };

    struct CPUAnimationData {
        std::string name;
        float duration;
        float ticksPerSecond;
        struct Channel {
            uint32_t nodeIndex;
            std::vector<VectorKey> positionKeys;
            std::vector<QuatKey> rotationKeys;
            std::vector<VectorKey> scalingKeys;
        };
        std::vector<Channel> channels;
    };
    
    

    

    // -------------------------------------------------------------------------
    // RUNTIME / INTERMEDIATE STRUCTURES
    // -------------------------------------------------------------------------
    namespace Runtime {
        struct StaticVertex {
            float px, py, pz;
            float nx, ny, nz;
            float u, v;
            float tx, ty, tz; 
        };

        struct SkinnedVertex {
            float px, py, pz;
            float nx, ny, nz;
            float u, v;
            float tx, ty, tz;
            int32_t boneIndices[4]; 
            float boneWeights[4];   
        };
    }

    

    // -------------------------------------------------------------------------
    // CPU DATA HOLDERS 
    // -------------------------------------------------------------------------
    struct CPUTextureData {
        std::vector<unsigned char> pixels;
        int width, height;
        uint32_t format;
        uint32_t internalFormat = 0;
        bool isValid = false;
    };

    struct CPUMeshData {
        std::vector<Runtime::StaticVertex> staticVertices;
        std::vector<Runtime::SkinnedVertex> skinnedVertices;
        std::vector<uint32_t> indices;
        int materialIndex = -1;
        bool hasBones = false;
    };

    struct CPUModelData {
        std::vector<CPUMeshData> meshes;
        std::string path;
        glm::vec3 minBound;
        glm::vec3 maxBound;
        glm::vec3 centroid;
    };
} // namespace Engine::Assets::AssetUtils