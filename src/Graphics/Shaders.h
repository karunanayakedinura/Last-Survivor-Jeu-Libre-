#pragma once

namespace Engine::Graphics::Shaders {
    inline const char* debugVertShader = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 Color;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        Color = aColor;
        gl_Position = projection * view * vec4(aPos, 1.0);
    }
    )";

    inline const char* debugFragShader = R"(
    #version 330 core
    out vec4 FragColor;
    in vec3 Color;
    void main() {
        FragColor = vec4(Color, 1.0);
    }
    )";

    inline const char* pbrPreviewVert = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoords;
        out vec3 WorldPos;
        out vec3 Normal;
        out vec2 TexCoords;
        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;
        void main() {
            TexCoords = aTexCoords;
            WorldPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;  
            gl_Position = projection * view * vec4(WorldPos, 1.0);
        }
    )";

    inline const char* pbrPreviewFrag = R"(
        #version 330 core
        out vec4 FragColor;
        in vec3 WorldPos;
        in vec3 Normal;
        in vec2 TexCoords;
        
        // Uniforms matching Material::Bind
        uniform sampler2D uAlbedoTexture;
        uniform int uUseAlbedoMap;
        uniform vec4 uAlbedoFactor;
        uniform sampler2D uNormalMap;
        uniform int uUseNormalMap;
        uniform vec3 lightPos;
        uniform vec3 viewPos;

        void main() {
            vec3 albedo = (uUseAlbedoMap == 1) ? texture(uAlbedoTexture, TexCoords).rgb : uAlbedoFactor.rgb;
            vec3 norm = normalize(Normal);
            // [Add Normal Mapping logic here if desired]
            
            vec3 lightDir = normalize(lightPos - WorldPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * albedo;
            vec3 ambient = 0.1 * albedo;
            FragColor = vec4(ambient + diffuse, 1.0);
        }
    )";

    inline const char* unlitVert = R"(
    #version 330 core
    layout (location = 0) in vec3 inPos;
    layout (location = 1) in vec3 inNormal;
    layout (location = 2) in vec2 inUV;
    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;
    out vec3 fragPosWorld;
    out vec3 fragNormalWorld;
    out vec2 fragUV;
    void main() {
        fragPosWorld = (uModel * vec4(inPos, 1.0)).xyz;
        // Simple Normal Transform (No Normal Map logic needed for bake geometry)
        fragNormalWorld = mat3(transpose(inverse(uModel))) * inNormal;
        fragUV = inUV;
        gl_Position = uProjection * uView * vec4(fragPosWorld, 1.0);
    }
    )";

    inline const char* unlitFrag = R"(
    #version 330 core
    layout (location = 0) out vec4 gPosition;
    layout (location = 1) out vec4 gNormal;
    layout (location = 2) out vec4 gAlbedo;
    in vec3 fragPosWorld;
    in vec3 fragNormalWorld;
    in vec2 fragUV;
    uniform sampler2D uAlbedoTexture;
    uniform int uUseAlbedoMap;
    uniform vec4 uAlbedoFactor;
    void main() {
        vec4 albedo = uAlbedoFactor;
        if (uUseAlbedoMap == 1) { 
            vec4 tex = texture(uAlbedoTexture, fragUV);
            albedo *= tex; 
        }
        if (albedo.a < 0.05) {
            // cyan skycolor for alpha cutoff visualization in bake (can be removed later)
            // albedo = vec4(0.0, 1.0, 1.0, 1.0);
            discard; // Alpha Cutoff
        }

        // Output Raw Data (No Metallic/Roughness packing)
        gPosition = vec4(fragPosWorld, 1.0);
        gNormal = vec4(normalize(fragNormalWorld), 1.0);
        gAlbedo = albedo; // RAW COLOR
    }
    )";

    // --- ID Picking Shader ---
    inline const char* pickVert = R"(
        #version 330 core
        layout (location = 0) in vec3 inPos;
        uniform mat4 uModel, uView, uProjection;
        void main() { gl_Position = uProjection * uView * uModel * vec4(inPos, 1.0); }
    )";
    // Picking Fragment: Output Integer
    inline const char* pickFrag = R"(
        #version 330 core
        layout (location = 0) out int fragID;
        uniform int uID;
        void main() { fragID = uID; }
    )";
    // You also need an Animated Version (Vertex shader same as shadowMap2DAnim basically)
    inline const char* pickAnimVert = R"(
        #version 330 core
        layout (location = 0) in vec3 inPos;
        layout (location = 3) in ivec4 boneIDs;
        layout (location = 4) in vec4 weights;
        uniform mat4 uModel, uView, uProjection;
        const int MAX_BONES = 250;
        const int MAX_BONE_INFLUENCE = 4;
        uniform mat4 finalBoneMatrices[MAX_BONES];
        void main() {
            vec4 totalPosition = vec4(0.0f);
            for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
                if(boneIDs[i] == -1) continue;
                if(boneIDs[i] >= MAX_BONES) {
                    totalPosition = vec4(inPos,1.0f);
                    break;
                }
                vec4 localPosition = finalBoneMatrices[boneIDs[i]] * vec4(inPos,1.0f);
                totalPosition += localPosition * weights[i];
            }
            if (length(totalPosition) < 0.001) totalPosition = vec4(inPos, 1.0);
            gl_Position = uProjection * uView * uModel * totalPosition;
        }
    )";

    #pragma region G-Buffer Shaders
    // === G-BUFFER PASS SHADERS (Unchanged) ===
    // (Note: G-Buffer shaders are unchanged from your original file)
    inline const char* gBufferVertShader = R"(
    #version 330 core
    layout (location = 0) in vec3 inPos;
    layout (location = 1) in vec3 inNormal;
    layout (location = 2) in vec2 inUV;
    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;
    out vec3 fragPosWorld;
    out vec3 fragNormalWorld;
    out vec2 fragUV;
    out vec4 fragPosView; 
    void main() {
        fragPosWorld = (uModel * vec4(inPos, 1.0)).xyz;
        fragNormalWorld = mat3(transpose(inverse(uModel))) * inNormal;
        fragUV = inUV;
        fragPosView = uView * vec4(fragPosWorld, 1.0);
        gl_Position = uProjection * fragPosView;
    }
    )";

    // --- UPDATED G-BUFFER FRAGMENT SHADER FOR PBR ---
    inline const char* gBufferFragShader = R"(
    #version 330 core
    layout (location = 0) out vec4 gPositionMetallic; 
    layout (location = 1) out vec4 gNormalRoughness; 
    layout (location = 2) out vec4 gAlbedo;

    in vec3 fragPosWorld;
    in vec3 fragNormalWorld;
    in vec2 fragUV;
    in vec4 fragPosView;

    // --- MATERIAL UNIFORMS ---
    uniform sampler2D uAlbedoTexture;
    uniform int uUseAlbedoMap;
    uniform vec4 uAlbedoFactor;

    uniform sampler2D uNormalMap;
    uniform int uUseNormalMap;
    uniform float uNormalStrength;

    uniform sampler2D uMetallicRoughnessMap; // G=Roughness, B=Metallic
    uniform int uUseMetallicMap;
    uniform float uMetallicFactor;
    uniform float uRoughnessFactor;

    uniform mat4 uView; 

    // Helper to calculate normal from map
    vec3 getNormalFromMap()
    {
        vec3 tangentNormal = texture(uNormalMap, fragUV).xyz * 2.0 - 1.0;
        
        vec3 Q1  = dFdx(fragPosWorld);
        vec3 Q2  = dFdy(fragPosWorld);
        vec2 st1 = dFdx(fragUV);
        vec2 st2 = dFdy(fragUV);

        vec3 N   = normalize(fragNormalWorld);
        vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
        vec3 B  = -normalize(cross(N, T));
        mat3 TBN = mat3(T, B, N);

        return normalize(TBN * tangentNormal);
    }

    void main() {
        // 1. Albedo
        vec4 albedo = uAlbedoFactor;
        if (uUseAlbedoMap == 1) {
                vec4 texColor = texture(uAlbedoTexture, fragUV);
                albedo *= texColor;
        }
        
        // 2. Normal
        vec3 N = normalize(fragNormalWorld);
        if (uUseNormalMap == 1) {
            N = getNormalFromMap();
            // Simple Lerp for strength (not physically perfect but works)
            N = normalize(mix(normalize(fragNormalWorld), N, uNormalStrength));
        }
        vec3 fragNormalView = mat3(uView) * N;

        // 3. Metallic / Roughness
        float metallic = uMetallicFactor;
        float roughness = uRoughnessFactor;
        
        if (uUseMetallicMap == 1) {
            // Assumes glTF standard: G = Roughness, B = Metallic
            vec4 mrSample = texture(uMetallicRoughnessMap, fragUV);
            roughness = mrSample.g; 
            metallic = mrSample.b;
        }

        // Output to G-Buffer
        gPositionMetallic = vec4(fragPosView.xyz, metallic);
        gNormalRoughness = vec4(normalize(fragNormalView), roughness);
        gAlbedo = albedo; 
    }
    )";
    inline const char* gBufferAnimVertShader = R"(
    #version 330 core
    layout (location = 0) in vec3 inPos;
    layout (location = 1) in vec3 inNormal;
    layout (location = 2) in vec2 inUV;
    layout (location = 3) in ivec4 boneIDs;
    layout (location = 4) in vec4 weights;
    #define MAX_BONE_INFLUENCE 4
    #define MAX_BONES 250
    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;
    uniform mat4 finalBoneMatrices[MAX_BONES];
    out vec3 fragPosWorld;
    out vec3 fragNormalWorld;
    out vec2 fragUV;
    out vec4 fragPosView;
    void main() {
        vec4 totalPosition = vec4(0.0f);
        vec3 totalNormal = vec3(0.0f);
        for(int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            if(boneIDs[i] >= 0 && boneIDs[i] < MAX_BONES && weights[i] > 0.0) {
                totalPosition += (finalBoneMatrices[boneIDs[i]] * vec4(inPos, 1.0)) * weights[i];
                totalNormal += (mat3(finalBoneMatrices[boneIDs[i]]) * inNormal) * weights[i];
            }
        }
        if (length(totalPosition) < 0.0001) totalPosition = vec4(inPos, 1.0);
        if (length(totalNormal) < 0.0001) totalNormal = inNormal;
        fragPosWorld = (uModel * totalPosition).xyz;
        fragNormalWorld = mat3(transpose(inverse(uModel))) * normalize(totalNormal);
        fragUV = inUV;
        fragPosView = uView * vec4(fragPosWorld, 1.0);
        gl_Position = uProjection * fragPosView;
    }
    )";
    inline const char* gBufferAnimFragShader = gBufferFragShader;
    #pragma endregion

    // --- NEW SHADOW SHADERS (From Multi_lights.cpp) ---
    #pragma region Shadow Shaders

    // --- 2D Shadow (For Dir/Spot) ---
    inline const char* shadow2DVertShader = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 lightSpaceMatrix;
    uniform mat4 model;
    void main() {
        gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
    }
    )glsl";
    inline const char* shadow2DAnimVertShader = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 inPos;
    layout (location = 3) in ivec4 boneIDs;
    layout (location = 4) in vec4 weights;
    #define MAX_BONE_INFLUENCE 4
    #define MAX_BONES 250
    uniform mat4 lightSpaceMatrix;
    uniform mat4 model;
    uniform mat4 finalBoneMatrices[MAX_BONES];
    void main() {
        vec4 totalLocalPos = vec4(0.0f);
        for(int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if(boneIDs[i] >= 0 && boneIDs[i] < MAX_BONES && weights[i] > 0.0)
            {
                totalLocalPos += (finalBoneMatrices[boneIDs[i]] * vec4(inPos, 1.0)) * weights[i];
            }
        }
        if (length(totalLocalPos) < 0.0001) {
            totalLocalPos = vec4(inPos, 1.0);
        }
        gl_Position = lightSpaceMatrix * model * totalLocalPos;
    }
    )glsl";
    inline const char* shadow2DFragShader = R"glsl(
    #version 330 core
    void main() {
        // Hardware does the depth write
    }
    )glsl";

    // --- Cubemap Shadow (For Point) ---
    inline const char* shadowCubeVertShader = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model;
    out VS_OUT {
        vec3 FragPos;
    } vs_out;
    void main() {
        vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
        gl_Position = model * vec4(aPos, 1.0); // GS will overwrite
    }
    )glsl";
    inline const char* shadowCubeAnimVertShader = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 inPos;
    layout (location = 3) in ivec4 boneIDs;
    layout (location = 4) in vec4 weights;
    #define MAX_BONE_INFLUENCE 4
    #define MAX_BONES 250
    uniform mat4 model;
    uniform mat4 finalBoneMatrices[MAX_BONES];
    out VS_OUT {
        vec3 FragPos;
    } vs_out;
    void main() {
        vec4 totalLocalPos = vec4(0.0f);
        for(int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if(boneIDs[i] >= 0 && boneIDs[i] < MAX_BONES && weights[i] > 0.0)
            {
                totalLocalPos += (finalBoneMatrices[boneIDs[i]] * vec4(inPos, 1.0)) * weights[i];
            }
        }
        if (length(totalLocalPos) < 0.0001) {
            totalLocalPos = vec4(inPos, 1.0);
        }
        
        vs_out.FragPos = vec3(model * totalLocalPos);
        gl_Position = model * totalLocalPos; // GS will overwrite
    }
    )glsl";
    inline const char* shadowCubeGeomShader = R"glsl(
    #version 330 core
    layout (triangles) in;
    layout (triangle_strip, max_vertices=18) out; // 6 faces * 3 vertices
    in VS_OUT {
        vec3 FragPos;
    } vs_in[];
    out vec3 FragPos; // Pass FragPos to Fragment shader
    uniform mat4 shadowMatrices[6]; // 6 view*projection matrices
    void main() {
        for(int face = 0; face < 6; ++face)
        {
            gl_Layer = face; // Select which cubemap face to render to
            for(int i = 0; i < 3; ++i) // For each vertex of the triangle
            {
                FragPos = vs_in[i].FragPos;
                gl_Position = shadowMatrices[face] * vec4(FragPos, 1.0);
                EmitVertex();
            }
            EndPrimitive();
        }
    }
    )glsl";
    inline const char* shadowCubeFragShader = R"glsl(
    #version 330 core
    in vec3 FragPos;
    uniform vec3 lightPos;
    uniform float far_plane;
    void main() {
        float lightDistance = length(FragPos - lightPos);
        lightDistance = lightDistance / far_plane;
        gl_FragDepth = lightDistance;
    }
    )glsl";

    #pragma endregion

    // --- SSAO SHADERS (Unchanged) ---
    #pragma region SSAO Shaders
    inline const char* ssaoVertShader = R"(
    #version 330 core
    layout (location = 0) in vec3 inPos;
    layout (location = 1) in vec2 inUV;
    out vec2 fragUV;
    void main() {
        fragUV = inUV;
        gl_Position = vec4(inPos, 1.0);
    }
    )";

    inline const char* ssaoFragShader = R"(
    #version 330 core
    out float fragOcc;
    in vec2 fragUV;

    uniform sampler2D gPositionView; 
    uniform sampler2D gNormalView;   
    uniform sampler2D texNoise;
    uniform vec3 samples[64];
    uniform mat4 uProjection;
    uniform vec2 noiseScale; // FIX: Ensure this matches C++ (vec2)

    int kernelSize = 64;
    float radius = 0.25; // Slightly larger radius
    float bias = 0.035; // Increased bias to prevent self-occlusion acne

    void main() {
        vec3 fragPosView = texture(gPositionView, fragUV).xyz;
        
        // FIX: Background Check. 
        // If Z is very close to 0.0, it's the clear color (camera eye). 
        // We shouldn't generate AO for the background or let it occlude things.
        if (fragPosView.z > -0.0001) {
            fragOcc = 1.0; // No occlusion
            return;
        }

        vec3 normalView = normalize(texture(gNormalView, fragUV).xyz);
        vec3 randomVec = normalize(texture(texNoise, fragUV * noiseScale).xyz);
        
        vec3 tangent = normalize(randomVec - normalView * dot(randomVec, normalView));
        vec3 bitangent = cross(normalView, tangent);
        mat3 TBN = mat3(tangent, bitangent, normalView);
        
        float occlusion = 0.0;
        for(int i = 0; i < kernelSize; ++i) {
            vec3 sampleView = TBN * samples[i];
            sampleView = fragPosView + sampleView * radius; 
            
            vec4 offset = vec4(sampleView, 1.0);
            offset = uProjection * offset;
            offset.xyz /= offset.w;
            offset.xyz = offset.xyz * 0.5 + 0.5;
            
            float sampleDepth = texture(gPositionView, offset.xy).z;
            
            // FIX: Ignore samples that hit the background (Z=0)
            if (sampleDepth > -0.0001) continue;

            // Range Check: Prevent objects far behind from occluding
            float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPosView.z - sampleDepth));
            
            // Accumulate
            occlusion += (sampleDepth >= sampleView.z + bias ? 1.0 : 0.0) * rangeCheck;
        }
        
        occlusion = 1.0 - (occlusion / float(kernelSize));
        fragOcc = pow(occlusion, 2.0); // Increase contrast slightly
    }
    )";
    #pragma endregion

    // --- SSAO BLUR SHADERS (Unchanged) ---
    #pragma region SSAO Blur Shaders
    inline const char* ssaoBlurVertShader = ssaoVertShader; // Same as SSAO
    inline const char* ssaoBlurFragShader = R"(
    #version 330 core
    out float fragBlur;
    in vec2 fragUV;

    uniform sampler2D ssaoInput;
    uniform sampler2D gPositionMetallic; // Needed for Depth
    uniform sampler2D gNormalRoughness;  // Needed for Normals (optional, implies better edges)

    void main() { 
        vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
        float result = 0.0;
        float weightSum = 0.0;
        
        // Read center data
        float centerDepth = texture(gPositionMetallic, fragUV).z;
        vec3 centerNormal = texture(gNormalRoughness, fragUV).xyz;
        float centerOcc = texture(ssaoInput, fragUV).r;

        const int RADIUS = 2; // 5x5 kernel
        
        for (int x = -RADIUS; x <= RADIUS; ++x) {
            for (int y = -RADIUS; y <= RADIUS; ++y) {
                vec2 offset = vec2(float(x), float(y)) * texelSize;
                vec2 neighborUV = fragUV + offset;
                
                float neighborDepth = texture(gPositionMetallic, neighborUV).z;
                vec3 neighborNormal = texture(gNormalRoughness, neighborUV).xyz;
                float neighborOcc = texture(ssaoInput, neighborUV).r;

                // 1. Spatial Weight (Gaussian)
                // Closer pixels in 2D have higher weight
                float spatialWeight = exp(-(float(x*x + y*y)) / 4.0);

                // 2. Depth Weight (Bilateral)
                // If depth difference is large, weight drops to 0 (don't blur across edges)
                float depthDiff = abs(centerDepth - neighborDepth);
                float depthWeight = exp(-(depthDiff * depthDiff) / 0.05); // Adjust denominator for sensitivity

                // 3. Normal Weight (Optional)
                // If normals point different directions (corner), don't blur
                float normalWeight = max(dot(centerNormal, neighborNormal), 0.0);
                
                float totalWeight = spatialWeight * depthWeight * normalWeight;
                
                result += neighborOcc * totalWeight;
                weightSum += totalWeight;
            }
        }
        
        // Safety to prevent divide by zero
        if (weightSum < 0.0001) fragBlur = centerOcc;
        else fragBlur = result / weightSum;
    }
    )";
    #pragma endregion

    // --- ========================================================== ---
    // ---                 FIXED LIGHTING SHADER                      ---
    // --- ========================================================== ---
    #pragma region Lighting Shader
    inline const char* lightingVertShader = R"(
    #version 330 core
    layout (location = 0) in vec3 inPos;
    layout (location = 1) in vec2 inUV;
    out vec2 fragUV;
    void main() {
        fragUV = inUV;
        gl_Position = vec4(inPos, 1.0);
    }
    )";

    inline const char* lightingFragShader = R"(
        #version 330 core
        out vec4 outColor;
        in vec2 fragUV;

        uniform sampler2D gPositionMetallic; 
        uniform sampler2D gNormalRoughness;  
        uniform sampler2D gAlbedo;
        uniform sampler2D ssao; 
        uniform sampler2D gDepthTexture; 
        uniform sampler2D volFogTexture; 
        
        // --- NEW: DDGI Uniforms ---
        uniform sampler2D uDDGIIrradiance; // Unit 14
        uniform sampler2D uDDGIDistance;
        uniform vec3 uDDGIProbeStart;
        uniform vec3 uDDGIProbeStep;
        uniform ivec3 uDDGIProbeCounts;
        
        uniform int uDebugMode; 

        // --- SHADOWS ---
        uniform sampler2DShadow uShadowMaps2D[8];
        uniform samplerCubeShadow uShadowMapsCube[8];

        // --- LIGHT DATA ---
        struct LightData {
            vec3 dir; vec3 color; float intensity; vec3 pos;
            int type; float range; float spotAngle;
            int shadowType; mat4 lightSpaceMatrix; float shadowFarPlane; int shadowIndex;
        };
        uniform LightData uLights[64];
        uniform int uNumLights;

        uniform vec3 viewPos; 
        uniform mat4 uView;   
        uniform mat4 uProjection; 

        uniform sampler2D uTEGI;
        uniform int uUseTEGI;

        const float PI = 3.14159265359;

        // --- SMOOTH PROBE WEIGHTING ---
        float GetProbeWeight(vec3 probePos, vec3 hitPos, vec3 hitNormal, int probeIndex) {
            vec3 dirToProbe = probePos - hitPos;
            float distToProbe = length(dirToProbe);
            dirToProbe /= distToProbe;
            
            // Smooth Backface
            float backfaceWeight = dot(hitNormal, dirToProbe);
            // Sigmoid curve for smoother backface transition
            backfaceWeight = 0.5 * (dot(hitNormal, dirToProbe) + 1.0); 
            backfaceWeight = smoothstep(0.1, 0.6, backfaceWeight);

            // Chebyshev
            vec2 texSize = textureSize(uDDGIDistance, 0);
            float u = (float(probeIndex * 8) + 4.0) / texSize.x;
            
            vec2 moments = texture(uDDGIDistance, vec2(u, 0.5)).rg;
            float mean = moments.x;
            float mean2 = moments.y;
            
            // High epsilon to prevent circles
            float variance = abs(mean2 - mean * mean);
            variance = max(variance, 0.2); 
            
            float d = distToProbe - mean;
            float p_max = variance / (variance + d * d);
            
            float chebyshevWeight = (distToProbe <= mean) ? 1.0 : p_max;
            
            // SMOOTHSTEP to remove the "Edge" artifact
            // Instead of raw probability, we curve it.
            chebyshevWeight = smoothstep(0.3, 0.9, chebyshevWeight);
            
            // Distance Fade (Don't let probes influence pixels too far away)
            float distanceWeight = 1.0 - smoothstep(0.0, length(uDDGIProbeStep) * 1.5, distToProbe);

            return backfaceWeight * chebyshevWeight * distanceWeight;
        }

        vec3 SampleDDGI(vec3 P, vec3 N, vec3 viewDir) {
            // --- APPLY NORMAL BIAS ---
            // Shift the sample point slightly outward along the normal and toward the camera.
            // This prevents the surface from thinking it's inside the wall's grid cell.
            float biasMultiplier = length(uDDGIProbeStep) * 0.25;
            vec3 biasedP = P + (N * biasMultiplier) + (viewDir * (biasMultiplier * 0.5));

            vec3 gridPos = (biasedP - uDDGIProbeStart) / uDDGIProbeStep;
            ivec3 baseIdx = ivec3(floor(gridPos));
            vec3 alpha = fract(gridPos);
            
            vec3 sumIrradiance = vec3(0.0);
            float sumWeight = 0.0;
            vec2 texSize = textureSize(uDDGIIrradiance, 0);

            for(int z = 0; z <= 1; ++z) {
                for(int y = 0; y <= 1; ++y) {
                    for(int x = 0; x <= 1; ++x) {
                        ivec3 idx = baseIdx + ivec3(x, y, z);
                        
                        if(idx.x < 0 || idx.x >= uDDGIProbeCounts.x ||
                        idx.y < 0 || idx.y >= uDDGIProbeCounts.y ||
                        idx.z < 0 || idx.z >= uDDGIProbeCounts.z) continue;

                        vec3 smoothAlpha = smoothstep(0.0, 1.0, alpha);
                        float w = ((x == 1) ? smoothAlpha.x : (1.0 - smoothAlpha.x)) *
                                ((y == 1) ? smoothAlpha.y : (1.0 - smoothAlpha.y)) *
                                ((z == 1) ? smoothAlpha.z : (1.0 - smoothAlpha.z));
                        
                        if(w < 0.001) continue;

                        vec3 probePos = uDDGIProbeStart + vec3(idx) * uDDGIProbeStep;
                        int probeIndex = idx.x + idx.y * uDDGIProbeCounts.x + idx.z * (uDDGIProbeCounts.x * uDDGIProbeCounts.y);

                        // Use biased position for visibility weighting
                        float vis = GetProbeWeight(probePos, biasedP, N, probeIndex);
                        float finalWeight = w * vis;

                        float u = (float(probeIndex * 8) + 4.0) / texSize.x;
                        sumIrradiance += texture(uDDGIIrradiance, vec2(u, 0.5)).rgb * finalWeight;
                        sumWeight += finalWeight;
                    }
                }
            }

            if(sumWeight > 0.0001) sumIrradiance /= sumWeight;
            else sumIrradiance = vec3(0.0);

            return sumIrradiance;
        }

        // --- PBR FUNCTIONS (Standard) ---
        float DistributionGGX(vec3 N, vec3 H, float roughness) {
            float a = roughness * roughness;
            float a2 = a * a;
            float NdotH = max(dot(N, H), 0.0);
            float NdotH2 = NdotH * NdotH;
            float num = a2;
            float denom = (NdotH2 * (a2 - 1.0) + 1.0);
            denom = PI * denom * denom;
            return num / max(denom, 0.000001);
        }
        float GeometrySchlickGGX(float NdotV, float roughness) {
            float r = (roughness + 1.0);
            float k = (r * r) / 8.0;
            float num = NdotV;
            float denom = NdotV * (1.0 - k) + k;
            return num / denom;
        }
        float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
            float NdotV = max(dot(N, V), 0.0);
            float NdotL = max(dot(N, L), 0.0);
            float ggx1 = GeometrySchlickGGX(NdotL, roughness);
            float ggx2 = GeometrySchlickGGX(NdotV, roughness);
            return ggx1 * ggx2;
        }
        vec3 fresnelSchlick(float cosTheta, vec3 F0) {
            return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
        }

        // --- SHADOW FUNCTIONS ---
        float getShadow2D(int index, vec3 coords) {
            switch (index) {
                case 0: return texture(uShadowMaps2D[0], coords);
                case 1: return texture(uShadowMaps2D[1], coords);
                default: return 1.0;
            }
        }
        float getShadowCube(int index, vec4 coords) {
            switch (index) {
                case 0: return texture(uShadowMapsCube[0], coords);
                case 1: return texture(uShadowMapsCube[1], coords);
                default: return 1.0;
            }
        }
        float CalculateShadow2D(vec4 lightSpacePos, float dotNL, int shadowIndex) {
            vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
            projCoords = projCoords * 0.5 + 0.5;
            if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) 
                return 1.0; 
            float currentBias = max(0.002 * (1.0 - dotNL), 0.0002); 
            float shadow = 0.0;
            vec2 texelSize = 1.0 / vec2(textureSize(uShadowMaps2D[0], 0)); 
            for(int x = -1; x <= 1; ++x) {
                for(int y = -1; y <= 1; ++y) {
                    float pcfDepth = getShadow2D(shadowIndex, vec3(projCoords.xy + vec2(x, y) * texelSize, projCoords.z - currentBias)); 
                    shadow += pcfDepth; 
                }
            }
            return shadow / 9.0;
        }
        float CalculateShadowCube(vec3 fragPosWorld, vec3 lightPos, float dotNL, int shadowIndex, float far_plane) {
            vec3 fragToLight = fragPosWorld - lightPos;
            float currentBias = 0.05;
            float currentDepth = length(fragToLight);
            return getShadowCube(shadowIndex, vec4(fragToLight, (currentDepth - currentBias)/far_plane));
        }

        void main() {
            vec4 posMetallic = texture(gPositionMetallic, fragUV);
            vec3 fragPosView = posMetallic.xyz;
            float metallic = posMetallic.w;

            // In view space, Z >= 0 means the camera hasn't hit any geometry
            if (fragPosView.z >= -0.0001) {
                // vec3 skyColor = vec3(0.0, 1.0, 1.0); // Cyan Background!
                vec3 skyColor = vec3(0.05, 0.05, 0.08); // Dark Background!
                
                // Keep the Volumetric Fog blending so the horizon still looks natural
                vec4 fogData = texture(volFogTexture, fragUV);
                skyColor = skyColor * (1.0 - fogData.a) + fogData.rgb;
                
                outColor = vec4(skyColor, 1.0);
                return;
            }

            vec4 normalRoughness = texture(gNormalRoughness, fragUV);
            vec3 N = normalize(normalRoughness.xyz);
            float roughness = normalRoughness.w;

            vec3 albedo = texture(gAlbedo, fragUV).rgb;
            float ao = texture(ssao, fragUV).r;

            // Reconstruct World Position
            // vec3 fragPosWorld = vec3(inverse(uView) * vec4(fragPosView, 1.0));

            // // --- CALCULATE DDGI ---
            // vec3 indirectLight = SampleDDGI(fragPosWorld, normalize(mat3(inverse(uView)) * N));

            // Reconstruct World Position
            vec3 fragPosWorld = vec3(inverse(uView) * vec4(fragPosView, 1.0));
            vec3 worldNormal = normalize(mat3(inverse(uView)) * N);
            vec3 viewDirWorld = normalize(viewPos - fragPosWorld); // Calculate camera vector

            vec3 indirectLight = vec3(0.0);
            
            // Toggle between TEGI and DDGI
            if (uUseTEGI == 1) {
                indirectLight = texture(uTEGI, fragUV).rgb;
            } else {
                indirectLight = SampleDDGI(fragPosWorld, worldNormal, viewDirWorld);
            }

            // *** TEGI DEBUG VIEW ***
            if (uDebugMode == 12) { 
                outColor = vec4(indirectLight, 1.0); 
                return; 
            }

            // --- DEBUG VIEWS ---
            if (uDebugMode == 1) { outColor = vec4(albedo, 1.0); return; } 
            if (uDebugMode == 2) { 
                    vec3 worldNormal = normalize(mat3(inverse(uView)) * N); 
                    outColor = vec4(worldNormal * 0.5 + 0.5, 1.0); return; 
            }
            if (uDebugMode == 3) { outColor = vec4(vec3(roughness), 1.0); return; }
            if (uDebugMode == 4) { outColor = vec4(vec3(metallic), 1.0); return; }
            if (uDebugMode == 5) { outColor = vec4(vec3(ao), 1.0); return; }
            if (uDebugMode == 9) { outColor = vec4(albedo, 1.0); return; }
            
            // *** DDGI DEBUG VIEW ***
            if (uDebugMode == 10) { 
                outColor = vec4(indirectLight, 1.0); 
                return; 
            }
            
            if (uDebugMode == 11) { 
                outColor = texture(volFogTexture, fragUV); 
                return; 
            }

            vec3 V = normalize(-fragPosView); 
            vec3 F0 = vec3(0.04); 
            F0 = mix(F0, albedo, metallic);

            vec3 Lo = vec3(0.0);
            
            // Direct Lighting Loop
            for(int i = 0; i < uNumLights; ++i) {
                LightData light = uLights[i];
                vec3 L;
                float attenuation = 1.0;
                
                if (light.type == 0) { 
                    vec3 lightDirView = mat3(uView) * light.dir;
                    L = normalize(-lightDirView);
                } else { 
                    vec3 lightPosView = vec3(uView * vec4(light.pos, 1.0));
                    L = normalize(lightPosView - fragPosView);
                    float distance = length(lightPosView - fragPosView);
                    attenuation = 1.0 / (distance * distance); 
                    attenuation *= (1.0 - smoothstep(light.range * 0.8, light.range, distance));
                }

                vec3 H = normalize(V + L);
                
                float shadow = 1.0;
                vec3 lightDirWorld = normalize(light.pos - fragPosWorld); 
                if (light.type == 0) lightDirWorld = normalize(-light.dir);
                float NdotL_World = max(dot(normalize(mat3(inverse(uView)) * N), lightDirWorld), 0.0);
                
                if (light.shadowType == 1 || light.shadowType == 3) {
                    vec4 lightSpacePos = light.lightSpaceMatrix * vec4(fragPosWorld, 1.0);
                    shadow = CalculateShadow2D(lightSpacePos, NdotL_World, light.shadowIndex);
                } else if (light.shadowType == 2) {
                    shadow = CalculateShadowCube(fragPosWorld, light.pos, NdotL_World, light.shadowIndex, light.shadowFarPlane);
                }

                float NDF = DistributionGGX(N, H, roughness);   
                float G = GeometrySmith(N, V, L, roughness);      
                vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
                vec3 numerator    = NDF * G * F; 
                float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
                vec3 specular = numerator / denominator;
                
                vec3 kS = F;
                vec3 kD = vec3(1.0) - kS;
                kD *= 1.0 - metallic;	  

                float NdotL = max(dot(N, L), 0.0);        
                
                if (light.type == 2) {
                    vec3 currentLightDirWorld = normalize(light.pos - fragPosWorld);
                    float theta = dot(currentLightDirWorld, normalize(-light.dir));
                    float epsilon = 0.1; 
                    float intensity = clamp((theta - light.spotAngle) / epsilon, 0.0, 1.0);
                    attenuation *= intensity;
                }

                Lo += (kD * albedo / PI + specular) * light.color * light.intensity * attenuation * NdotL * shadow; 
            }
            // --- APPLY DDGI ---
            // Replace simple ambient with DDGI
            vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
            vec3 kD = 1.0 - kS;
            kD *= 1.0 - metallic;
            
            // Note: In full PBR, we would multiply indirectLight by AO
            vec3 ambient = (kD * indirectLight * albedo) * ao; 
            vec3 color = ambient + Lo;
            vec4 fogData = texture(volFogTexture, fragUV);
            vec3 fogColor = fogData.rgb;
            float fogOpacity = fogData.a;
            color = color * (1.0 - fogOpacity) + fogColor;
            outColor = vec4(color, 1.0);


            // outColor = vec4(vec3(1.0), 1.0);
        }
    )";
    #pragma endregion

    // --- FORWARD TRANSPARENCY SHADER (Unchanged) ---
    #pragma region Forward Transparent Shaders
    inline const char* forwardTransparentVertShader = R"(
    #version 330 core
    layout (location = 0) in vec3 inPos;
    layout (location = 1) in vec3 inNormal;
    layout (location = 2) in vec2 inUV;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;

    out vec3 fragPosWorld;
    out vec3 fragNormalWorld;
    out vec2 fragUV;

    void main() {
        fragPosWorld = (uModel * vec4(inPos, 1.0)).xyz;
        fragNormalWorld = mat3(transpose(inverse(uModel))) * inNormal;
        fragUV = inUV;
        gl_Position = uProjection * uView * uModel * vec4(inPos, 1.0);
    }
    )";

    inline const char* forwardTransparentFragShader = R"(
    #version 330 core
    out vec4 FragColor;

    in vec3 fragPosWorld;
    in vec3 fragNormalWorld;
    in vec2 fragUV;

    uniform sampler2D uAlbedoTexture;
    uniform vec3 uViewPos;

    // Simplified light (assumes one point light)
    uniform vec3 uLightPos;
    uniform vec3 uLightColor;
    uniform float uLightIntensity;

    void main() {
        vec4 albedo = texture(uAlbedoTexture, fragUV);
        
        if (albedo.a < 0.01) {
            discard;
        }
        
        // Basic Blinn-Phong lighting
        vec3 ambient = 0.05 * albedo.rgb;
        
        vec3 normal = normalize(fragNormalWorld);
        vec3 lightDir = normalize(uLightPos - fragPosWorld);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * uLightColor * uLightIntensity;
        
        vec3 viewDir = normalize(uViewPos - fragPosWorld);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        vec3 specular = spec * uLightColor * uLightIntensity;
        
        vec3 litColor = (ambient + diffuse + specular) * albedo.rgb;
        
        FragColor = vec4(litColor, albedo.a);
    }
    )";
    #pragma endregion


    #pragma region Model Preview Shaders
    #pragma region Model Preview Shaders
    inline const char* previewVert = R"(
    #version 330 core
    layout (location = 0) in vec3 inPos;
    layout (location = 1) in vec3 inNormal; // Add Normals
    layout (location = 2) in vec2 inUV;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;

    out vec2 fragUV;
    out vec3 fragNormal; // Pass to frag

    void main() {
        fragUV = inUV;
        // Transform normal to world space (simplified, assumes uniform scale)
        fragNormal = mat3(transpose(inverse(uModel))) * inNormal; 
        gl_Position = uProjection * uView * uModel * vec4(inPos, 1.0);
    }
    )";

    inline const char* previewFrag = R"(
    #version 330 core
    out vec4 FragColor;

    in vec2 fragUV;
    in vec3 fragNormal;

    uniform sampler2D uTexture;
    uniform int uUseTexture;   // 1 = Use Texture, 0 = Use Solid Color
    uniform vec3 uColor;       // Base color if no texture

    void main() {
        // 1. Base Albedo
        vec4 baseColor = vec4(uColor, 1.0);
        if (uUseTexture == 1) {
            baseColor = texture(uTexture, fragUV);
        }

        // 2. Simple "Headlamp" Lighting
        // We assume light comes roughly from the camera direction (Z+) and slightly up
        vec3 normal = normalize(fragNormal);
        vec3 lightDir = normalize(vec3(0.5, 1.0, 1.0)); 
        
        float diff = max(dot(normal, lightDir), 0.0);
        
        // Add some ambient light so backfaces aren't pitch black
        vec3 ambient = 0.3 * baseColor.rgb;
        vec3 diffuse = diff * baseColor.rgb;
        
        FragColor = vec4(ambient + diffuse, baseColor.a);
    }
    )";
    #pragma endregion

    #pragma startregion SSGI Volumetric FOG
    // --- NEW SHADERS (Declarations) ---
    inline const char* ssgiFrag = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 fragUV;

        uniform sampler2D gPositionMetallic;
        uniform sampler2D gNormalRoughness;
        uniform sampler2D gAlbedo; // Not strictly used for lighting calc but useful for rejection
        uniform sampler2D uLitScene; // Direct Lighting result (from previous pass)
        
        uniform mat4 uProjection;
        uniform mat4 uView;
        uniform vec2 texelSize;
        
        // Raymarch Settings
        const int RAYS = 8;
        const int STEPS = 12;
        const float STEP_SIZE = 0.5; // World Units
        const float THICKNESS = 0.5; // Depth thickness for hit check

        float GetRandom(vec2 uv) {
            return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
        }

        void main() {
            vec4 posMet = texture(gPositionMetallic, fragUV);
            vec3 viewPos = posMet.xyz;
            if(viewPos.z >= 0.0) { FragColor = vec4(0.0); return; } // Sky/Background

            vec3 viewNorm = texture(gNormalRoughness, fragUV).xyz;
            vec3 viewDir = normalize(viewPos);

            vec3 indirectLight = vec3(0.0);
            float totalWeight = 0.0;
            
            // Random rotation per pixel
            float rng = GetRandom(fragUV);
            const float PI = 3.14159265;

            for(int i = 0; i < RAYS; i++) {
                // Cosine Weighted Hemisphere Sample
                float angle = (float(i) + rng) * 2.0 * PI / float(RAYS);
                // Simple distribution pattern
                vec3 randomDir = vec3(cos(angle), sin(angle), (float(i)+1.0)/float(RAYS)); 
                randomDir = normalize(randomDir);

                // Orient along normal
                vec3 tangent = normalize(cross(viewNorm, vec3(0,1,0)));
                if(abs(dot(viewNorm, vec3(0,1,0))) > 0.99) tangent = vec3(1,0,0);
                vec3 bitangent = cross(viewNorm, tangent);
                mat3 TBN = mat3(tangent, bitangent, viewNorm);
                vec3 rayDir = normalize(TBN * randomDir);

                // Raymarch
                vec3 currPos = viewPos;
                bool hit = false;
                vec3 hitColor = vec3(0.0);

                for(int s = 0; s < STEPS; s++) {
                    currPos += rayDir * STEP_SIZE * (1.0 + float(s)*0.2); // Progressive stepping

                    // Project to Screen
                    vec4 offset = uProjection * vec4(currPos, 1.0);
                    offset.xy /= offset.w;
                    offset.xy = offset.xy * 0.5 + 0.5;

                    if(offset.x < 0 || offset.x > 1 || offset.y < 0 || offset.y > 1) break;

                    float depthSample = texture(gPositionMetallic, offset.xy).z;
                    
                    // Depth Check (Linear View Space Z)
                    // viewPos is negative z. larger negative means further away.
                    // If sample is closer (less negative) than current ray, we might have hit.
                    // Ray: -10, Sample: -5 -> Hit? No, ray is behind.
                    // Ray: -10, Sample: -12 -> Hit? No, sample is behind ray (air).
                    // Ray: -10, Sample: -9.8 -> Hit? Yes.
                    
                    float delta = currPos.z - depthSample;
                    if(delta < 0 && delta > -THICKNESS) {
                        // HIT!
                        // Sample the Direct Lighting from this location to simulate bounce
                        hitColor = texture(uLitScene, offset.xy).rgb;
                        hit = true;
                        break;
                    }
                }
                
                if(hit) {
                    indirectLight += hitColor * dot(viewNorm, rayDir); // Lambert
                    totalWeight += 1.0;
                }
            }

            if(totalWeight > 0.0) indirectLight /= totalWeight;
            
            // Boost intensity slightly to simulate multiple bounces or exposure
            FragColor = vec4(indirectLight * 5.0, 1.0); 
        }
    )";

    inline const char* ssgiBlurFrag = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 fragUV;
        uniform sampler2D ssgiInput;
        uniform sampler2D gNormalRoughness;
        uniform sampler2D gPositionMetallic;
        uniform vec2 texelSize;

        void main() {
            vec2 offset = texelSize;
            vec3 result = texture(ssgiInput, fragUV).rgb * 0.227027;
            vec3 centerNorm = texture(gNormalRoughness, fragUV).xyz;
            float centerDepth = texture(gPositionMetallic, fragUV).z;

            float weightSum = 0.227027;
            
            // Simple Bilateral Filter (Cross Pattern)
            vec2 offsets[4] = vec2[](vec2(1,0), vec2(-1,0), vec2(0,1), vec2(0,-1));
            
            for(int i=0; i<4; i++) {
                for(int j=1; j<3; j++) { // Radius
                    vec2 uv = fragUV + offsets[i] * float(j) * offset;
                    
                    vec3 sampleNorm = texture(gNormalRoughness, uv).xyz;
                    float sampleDepth = texture(gPositionMetallic, uv).z;
                    vec3 sampleColor = texture(ssgiInput, uv).rgb;

                    // Weights
                    float wNorm = max(0.0, dot(centerNorm, sampleNorm));
                    float wDepth = 1.0 / (1.0 + abs(centerDepth - sampleDepth) * 10.0);
                    float w = wNorm * wDepth;

                    result += sampleColor * w;
                    weightSum += w;
                }
            }

            FragColor = vec4(result / weightSum, 1.0);
        }
    )";

    inline const char* volFogFrag = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 fragUV;
        
        uniform sampler2D gPositionMetallic;
        uniform vec3 viewPos;
        
        uniform sampler2DShadow uShadowMaps2D[8];
        uniform samplerCubeShadow uShadowMapsCube[8];
        
        struct LightData {
            vec3 dir; vec3 color; float intensity; vec3 pos;
            int type; float range; float spotAngle;
            int shadowType; mat4 lightSpaceMatrix; float shadowFarPlane; int shadowIndex;
        };
        uniform LightData uLights[64];
        uniform int uNumLights;
        uniform mat4 uInverseView; 
        uniform mat4 uInverseProjection;

        uniform int uDebugMode;

        // --- SETTINGS ---
        // Increased distance to 120.0 to allow fog to fully obscure background
        const float MAX_FOG_DIST = 120.0; 
        const float DENSITY = 0.015; // Base density of the fog
        const int STEPS = 128; // Increased steps for smoother long-distance fog
        const float ANISOTROPY = 0.7;

        float getShadow2D(int index, vec3 coords) {
                switch(index) {
                case 0: return texture(uShadowMaps2D[0], coords);
                case 1: return texture(uShadowMaps2D[1], coords);
                case 2: return texture(uShadowMaps2D[2], coords);
                case 3: return texture(uShadowMaps2D[3], coords);
                default: return 1.0;
                }
        }

        float CalculateShadow2D(vec4 lightSpacePos, int shadowIndex) {
            vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
            projCoords = projCoords * 0.5 + 0.5;
            
            // [FIX] Bounds check to prevent "Dark Artifacts" outside light frustum
            if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0) 
                return 1.0; // Outside shadow map = Lit

            return getShadow2D(shadowIndex, vec3(projCoords.xy, projCoords.z - 0.005));
        }
        
        float PhaseHG(float cosTheta) {
            float g = ANISOTROPY;
            return (1.0 - g*g) / (4.0 * 3.14159 * pow(1.0 + g*g - 2.0*g*cosTheta, 1.5));
        }

        void main() {
            vec4 posMet = texture(gPositionMetallic, fragUV);
            vec3 targetPosView = posMet.xyz;
            vec3 targetPosWorld;
            
            bool isSky = false;

            // [FIX] Sky Handling: If background, force ray to Max Distance
            if(targetPosView.z >= -0.0001) {
                isSky = true;
                vec4 ndc = vec4(fragUV * 2.0 - 1.0, 1.0, 1.0);
                vec4 viewRay = uInverseProjection * ndc;
                viewRay /= viewRay.w;
                vec3 worldDir = (uInverseView * vec4(normalize(viewRay.xyz), 0.0)).xyz;
                targetPosWorld = viewPos + normalize(worldDir) * MAX_FOG_DIST; 
                
                // We hit the sky! Output 0 fog opacity so the cyan background passes through.
                FragColor = vec4(0.0, 0.0, 0.0, 0.0);
                return;
            } else {
                targetPosWorld = (uInverseView * vec4(targetPosView, 1.0)).xyz;
            }

            vec3 startPos = viewPos;
            vec3 rayDir = targetPosWorld - startPos;
            float rayLength = length(rayDir);
            
            // Clamp minimum to avoid divide-by-zero artifacts
            if(rayLength < 0.001) { FragColor = vec4(0.0); return; }

            rayDir /= rayLength;
            
            // [FIX] Clamp Max Distance: Rays stop at geometry OR Max Fog Dist
            rayLength = min(rayLength, MAX_FOG_DIST);
            
            float stepSize = rayLength / float(STEPS);
            vec3 currentPos = startPos;
            vec3 accumulatedColor = vec3(0.0);
            float transmittance = 1.0;

            // Dithering to prevent banding
            float rng = fract(sin(dot(fragUV, vec2(12.9898, 78.233))) * 43758.5453);
            currentPos += rayDir * stepSize * rng;

            for(int i = 0; i < STEPS; i++) {
                if(transmittance < 0.01) break;

                // [OPTIONAL] Height Fog: Fade density with height (e.g. Y > 10)
                // float hDensity = DENSITY * exp(-max(currentPos.y, 0.0) * 0.1);
                float density = DENSITY; 

                vec3 stepLight = vec3(0.0);
                
                for(int L = 0; L < uNumLights; L++) {
                    if(uLights[L].type == 0 || uLights[L].type == 2) {
                        float shadow = 1.0;
                        vec3 lightDir;
                        float atten = 1.0;

                        if(uLights[L].type == 0) { // Dir
                            lightDir = normalize(-uLights[L].dir);
                            vec4 lSpace = uLights[L].lightSpaceMatrix * vec4(currentPos, 1.0);
                            shadow = CalculateShadow2D(lSpace, uLights[L].shadowIndex);
                        } else { // Spot
                            lightDir = normalize(uLights[L].pos - currentPos);
                            float dist = length(uLights[L].pos - currentPos);
                            if(dist < uLights[L].range) {
                                vec4 lSpace = uLights[L].lightSpaceMatrix * vec4(currentPos, 1.0);
                                shadow = CalculateShadow2D(lSpace, uLights[L].shadowIndex);
                                
                                vec3 Ldir = normalize(uLights[L].pos - currentPos);
                                float theta = dot(Ldir, normalize(-uLights[L].dir));
                                if(theta > uLights[L].spotAngle) atten = 1.0;
                                else shadow = 0.0;
                            } else shadow = 0.0;
                        }

                        if(shadow > 0.0) {
                            float phase = PhaseHG(dot(rayDir, lightDir));
                            float lightIntensitySquared = uLights[L].intensity * 25.0; // Boost factor for visibility
                            stepLight += uLights[L].color * lightIntensitySquared * shadow * (phase * 5.0) * atten; // Boost factor for visibility
                        } else {
                            stepLight += vec3(0.02, 0.03, 0.05); // Ambient Light for Shadowed Areas
                        }
                    }
                }
                
                // Ambient Fog Light (Prevents pitch black shadows)
                // stepLight += vec3(0.02, 0.03, 0.05); 

                float scatter = density * stepSize;
                accumulatedColor += stepLight * scatter * transmittance;
                transmittance *= exp(-scatter);

                currentPos += rayDir * stepSize;
            }

            FragColor = vec4(accumulatedColor, 1.0 - transmittance);

            if (uDebugMode == 11) {
                // Debug Visualization
                vec3 debugTint = accumulatedColor * 2.0; 
                if (length(accumulatedColor) < 0.005) {
                    if (isSky) debugTint = vec3(0.0, 0.0, 0.1); // Dark Blue = Sky
                    else debugTint = vec3(0.1, 0.0, 0.0);       // Dark Red = Mesh
                }
                FragColor = vec4(debugTint, 1.0);
            }
        }
    )";
    #pragma endregion
    // --- NEW: SSGI Vertex Shader (Standard Fullscreen Quad) ---
    inline const char* ssgiVertShader = R"(
    #version 330 core
    layout (location = 0) in vec3 inPos;
    layout (location = 1) in vec2 inUV;
    out vec2 fragUV;
    void main() {
        fragUV = inUV;
        gl_Position = vec4(inPos, 1.0);
    }
    )";

    // --- 1. RAY TRACE COMPUTE SHADER (NOW PATH TRACING) ---
    inline const char* ddgiTraceCS = R"(
    #version 450 core
    layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

    uniform int uRaysPerProbe;
    uniform vec3 uProbeStart;
    uniform vec3 uProbeStep;
    uniform ivec3 uProbeCounts;
    uniform int uFrameIndex; 

    // --- TEXTURES FOR INFINITE BOUNCE ---
    layout(binding = 3) uniform sampler2D uPreviousIrradiance;
    layout(binding = 4) uniform sampler2D uPreviousDistance;

    // --- LIGHT DATA ---
    struct LightData {
        vec3 dir; vec3 color; float intensity; vec3 pos;
        int type; float range; float spotAngle;
        int shadowType; mat4 lightSpaceMatrix; float shadowFarPlane; int shadowIndex;
    };
    uniform LightData uLights[32]; 
    uniform int uNumLights;

    // --- BVH ---
    struct Node { vec3 aabbMin; float pad1; vec3 aabbMax; float pad2; uvec4 children; uint childCount; uint firstPrim; uint primCount; uint pad3; };
    struct Primitive { vec3 v0; float pad1; vec3 v1; float pad2; vec3 v2; float pad3; };
    layout(std430, binding = 0) readonly buffer NodeBuffer { Node nodes[]; };
    layout(std430, binding = 1) readonly buffer PrimBuffer { Primitive prims[]; };

    layout(rgba16f, binding = 0) writeonly uniform image2D uRayData;

    // --- RNG ---
    uint pcg_hash(uint seed) {
        uint state = seed * 747796405u + 2891336453u;
        uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }
    float rand(inout uint seed) { seed = pcg_hash(seed); return float(seed) / 4294967296.0; }

    vec3 SampleSphereUniform(inout uint seed) {
        float u = rand(seed);
        float v = rand(seed);
        float theta = 2.0 * 3.14159265 * u;
        float phi = acos(2.0 * v - 1.0);
        return vec3(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
    }

    // --- INTERSECTION ---
    float IntersectBox(vec3 origin, vec3 invDir, vec3 boxMin, vec3 boxMax) {
        vec3 tMin = (boxMin - origin) * invDir;
        vec3 tMax = (boxMax - origin) * invDir;
        vec3 t1 = min(tMin, tMax);
        vec3 t2 = max(tMin, tMax);
        float tNear = max(max(t1.x, t1.y), t1.z);
        float tFar = min(min(t2.x, t2.y), t2.z);
        return tFar >= tNear && tFar > 0.0 ? tNear : 1e30;
    }
    float IntersectTriangle(vec3 ro, vec3 rd, vec3 v0, vec3 v1, vec3 v2, out vec3 normal) {
        vec3 e1 = v1 - v0;
        vec3 e2 = v2 - v0;
        vec3 h = cross(rd, e2);
        float a = dot(e1, h);
        if (abs(a) < 0.0001) return 1e30;
        float f = 1.0 / a;
        vec3 s = ro - v0;
        float u = f * dot(s, h);
        if (u < 0.0 || u > 1.0) return 1e30;
        vec3 q = cross(s, e1);
        float v = f * dot(rd, q);
        if (v < 0.0 || u + v > 1.0) return 1e30;
        float t = f * dot(e2, q);
        if (t > 0.001) { normal = normalize(cross(e1, e2)); return t; }
        return 1e30;
    }

    // --- SAMPLE PREVIOUS BOUNCE (Simplified for Perf) ---
    vec3 SamplePreviousGI(vec3 P, vec3 N) {
        // Offset slightly to avoid self-shadowing artifacts
        vec3 samplePos = P + N * 0.2; 
        
        vec3 gridPos = (samplePos - uProbeStart) / uProbeStep;
        ivec3 baseIdx = ivec3(floor(gridPos));
        vec3 alpha = fract(gridPos);
        
        // Clamp to grid
        baseIdx = clamp(baseIdx, ivec3(0), uProbeCounts - ivec3(2));

        // Simple Trilinear Fetch (No visibility check for the bounce to keep it fast)
        vec3 sum = vec3(0.0);
        vec2 texSize = textureSize(uPreviousIrradiance, 0);

        for(int i=0; i<8; ++i) {
            ivec3 offset = ivec3(i & 1, (i >> 1) & 1, (i >> 2) & 1);
            ivec3 idx = baseIdx + offset;
            
            float w = ((offset.x == 1) ? alpha.x : (1.0 - alpha.x)) *
                    ((offset.y == 1) ? alpha.y : (1.0 - alpha.y)) *
                    ((offset.z == 1) ? alpha.z : (1.0 - alpha.z));
            
            int probeIndex = idx.x + idx.y * uProbeCounts.x + idx.z * (uProbeCounts.x * uProbeCounts.y);
            float u = (float(probeIndex * 8) + 4.0) / texSize.x;
            
            sum += texture(uPreviousIrradiance, vec2(u, 0.5)).rgb * w;
        }
        return sum;
    }

    void main() {
        int probeIdx = int(gl_WorkGroupID.x);
        int rayIdx = int(gl_LocalInvocationID.x); 
        
        // Position
        int px = probeIdx % uProbeCounts.x;
        int py = (probeIdx / uProbeCounts.x) % uProbeCounts.y;
        int pz = probeIdx / (uProbeCounts.x * uProbeCounts.y);
        vec3 probePos = uProbeStart + vec3(px, py, pz) * uProbeStep;

        // Random Direction
        uint seed = uint(probeIdx * 1923 + rayIdx * 17 + uFrameIndex * 29384);
        vec3 dir = SampleSphereUniform(seed);
        vec3 invDir = 1.0 / dir;

        // Trace
        float dist = 1e30;
        vec3 hitNormal = vec3(0,1,0);
        bool hit = false;
        
        int stack[32]; for(int i=0; i<32; i++) stack[i] = 0; 
        int stackPtr = 0; stack[0] = 0; int steps = 0;

        while(stackPtr >= 0 && steps < 500) {
            steps++;
            int nodeIdx = stack[stackPtr--];
            Node node = nodes[nodeIdx];
            if(IntersectBox(probePos, invDir, node.aabbMin, node.aabbMax) >= dist) continue;
            if(node.primCount > 0) {
                for(uint i=0; i<node.primCount; i++) {
                    Primitive p = prims[node.firstPrim + i];
                    vec3 n;
                    float t = IntersectTriangle(probePos, dir, p.v0, p.v1, p.v2, n);
                    if(t < dist) { dist = t; hitNormal = n; hit = true; }
                }
            } else {
                for(int i=0; i<node.childCount; i++) { if(stackPtr < 31) stack[++stackPtr] = int(node.children[i]); }
            }
        }

        vec3 color = vec3(0.0);
        if (hit) {
            if (dot(dir, hitNormal) > 0.0) {
                color = vec3(0.0); // Backface
            } else {
                vec3 hitPos = probePos + dir * dist;
                
                // 1. Direct Light
                vec3 direct = vec3(0.0);
                for(int i = 0; i < uNumLights; ++i) {
                    LightData light = uLights[i];
                    vec3 L; float atten = 1.0;
                    if (light.type == 0) L = -normalize(light.dir);
                    else {
                        vec3 toLight = light.pos - hitPos;
                        float d = length(toLight);
                        L = normalize(toLight);
                        atten = 1.0 / (d * d);
                        atten *= (1.0 - smoothstep(light.range * 0.8, light.range, d));
                        if (light.type == 2) {
                            float theta = dot(normalize(light.pos - hitPos), normalize(-light.dir));
                            if(theta < light.spotAngle) atten = 0.0;
                        }
                    }
                    float NdotL = max(dot(hitNormal, L), 0.0);
                    direct += light.color * light.intensity * atten * NdotL;
                }

                // 2. Indirect Light (Infinite Bounce)
                // Sample the previous frame's GI at the hit point
                vec3 indirect = SamplePreviousGI(hitPos, hitNormal);
                
                // 3. Combine
                // Assume Albedo is 0.5 grey since we don't have material data
                // Energy Conservation: (Direct + Indirect) * Albedo
                color = (direct + indirect) * 0.5; 
                
                // Clamp fireflies
                color = min(color, vec3(20.0));
            }
        } else {
            color = vec3(0.05, 0.05, 0.08); // Sky
            // Change the Sky bounce color to Cyan
            // color = vec3(0.1, 0.1, 0.1);
        }

        imageStore(uRayData, ivec2(rayIdx, probeIdx), vec4(color, dist));
    }
    )";

    // 2. PROBE UPDATE COMPUTE SHADER
    inline const char* ddgiUpdateIrradianceCS = R"(
    #version 450 core
    layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

    uniform int uRaysPerProbe;
    uniform float uHysteresis = 0.95; 
    layout(rgba16f, binding = 0) readonly uniform image2D uRayData;
    layout(rgba16f, binding = 1) uniform image2D uIrradianceTex;

    void main() {
        int probeIdx = int(gl_WorkGroupID.x);
        ivec2 localID = ivec2(gl_LocalInvocationID.xy);
        
        // Border guard (Inner 6x6)
        if (localID.x == 0 || localID.x == 7 || localID.y == 0 || localID.y == 7) return;

        vec3 result = vec3(0.0);
        float weightSum = 0.0;
        
        // Average Rays
        for(int i=0; i<uRaysPerProbe; ++i) {
            vec4 ray = imageLoad(uRayData, ivec2(i, probeIdx));
            result += ray.rgb;
            weightSum += 1.0;
        }
        if(weightSum > 0) result /= weightSum;

        ivec2 atlasPos = ivec2(probeIdx * 8 + localID.x, localID.y);
        vec3 prev = imageLoad(uIrradianceTex, atlasPos).rgb;
        
        // High Hysteresis for temporal stability
        vec3 finalColor = mix(result, prev, uHysteresis);

        imageStore(uIrradianceTex, atlasPos, vec4(finalColor, 1.0));
    }
    )";



    // --- Shader Sources (Debug Only) ---
    inline const char* bvhDebugVert = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 Color;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        Color = aColor;
        gl_Position = projection * view * vec4(aPos, 1.0);
    }
    )";

    inline const char* bvhDebugFrag = R"(
    #version 330 core
    out vec4 FragColor;
    in vec3 Color;
    void main() {
        FragColor = vec4(Color, 1.0);
    }
    )";

    // --- Compute Shader 1: Update Primitives ---
    inline const char* bvhUpdateComputeSource = R"(
    #version 430 core
    layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

    // Output: World-space triangles
    struct GPUBVHPrimitive {
        vec4 v0; 
        vec4 v1; 
        vec4 v2; 
    };

    // Input: Local-space data
    struct GPUSourcePrimitive {
        vec4 v0_local; 
        vec4 v1_local;
        vec4 v2_local;
        uvec4 v0_bones; vec4 v0_weights;
        uvec4 v1_bones; vec4 v1_weights;
        uvec4 v2_bones; vec4 v2_weights;
        uint instanceID; 
        uint pad0; uint pad1; uint pad2;
    };

    struct GPUInstanceData {
        mat4 modelMatrix;
        int boneOffset;
        int hasBones;
        int pad0; int pad1;
    };

    layout(std430, binding = 10) buffer PrimBuffer { GPUBVHPrimitive primitives[]; };
    layout(std430, binding = 11) readonly buffer SourceBuffer { GPUSourcePrimitive sourcePrims[]; };
    layout(std430, binding = 12) readonly buffer InstanceBuffer { GPUInstanceData instances[]; };
    layout(std430, binding = 13) readonly buffer BoneBuffer { mat4 globalBones[]; };

    uniform uint totalPrims;

    vec3 SkinVertex(vec3 localPos, uvec4 boneIndices, vec4 boneWeights, int boneOffset) {
        mat4 skinMat = mat4(0.0);
        
        if (boneWeights.x > 0.0) skinMat += globalBones[boneOffset + int(boneIndices.x)] * boneWeights.x;
        if (boneWeights.y > 0.0) skinMat += globalBones[boneOffset + int(boneIndices.y)] * boneWeights.y;
        if (boneWeights.z > 0.0) skinMat += globalBones[boneOffset + int(boneIndices.z)] * boneWeights.z;
        if (boneWeights.w > 0.0) skinMat += globalBones[boneOffset + int(boneIndices.w)] * boneWeights.w;
        
        if (boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w <= 0.001) return localPos;

        return (skinMat * vec4(localPos, 1.0)).xyz;
    }

    void main() {
        uint idx = gl_GlobalInvocationID.x;
        if (idx >= totalPrims) return;

        GPUSourcePrimitive src = sourcePrims[idx];
        GPUInstanceData inst = instances[src.instanceID];

        vec3 p0 = src.v0_local.xyz;
        vec3 p1 = src.v1_local.xyz;
        vec3 p2 = src.v2_local.xyz;

        if (inst.hasBones == 1) {
            p0 = SkinVertex(p0, src.v0_bones, src.v0_weights, inst.boneOffset);
            p1 = SkinVertex(p1, src.v1_bones, src.v1_weights, inst.boneOffset);
            p2 = SkinVertex(p2, src.v2_bones, src.v2_weights, inst.boneOffset);
        }

        p0 = (inst.modelMatrix * vec4(p0, 1.0)).xyz;
        p1 = (inst.modelMatrix * vec4(p1, 1.0)).xyz;
        p2 = (inst.modelMatrix * vec4(p2, 1.0)).xyz;

        primitives[idx].v0 = vec4(p0, 0.0);
        primitives[idx].v1 = vec4(p1, 0.0);
        primitives[idx].v2 = vec4(p2, 0.0);
    }
    )";

    // --- Compute Shader 2: Refit Bounds (Bottom-Up) ---
    inline const char* bvhRefitComputeSource = R"(
    #version 430 core
    layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

    struct GPUBVHNode {
        vec3 aabbMin; float pad1;   
        vec3 aabbMax; float pad2;   
        uvec4 children;             
        uint childCount;
        uint firstPrim;
        uint primCount;
        int parent; 
    }; 

    struct GPUBVHPrimitive {
        vec4 v0; 
        vec4 v1; 
        vec4 v2; 
    };

    // Binding 0 defined in cpp as NodeBuffer (usually)
    layout(std430, binding = 0) buffer NodeBuffer { GPUBVHNode nodes[]; };
    layout(std430, binding = 10) readonly buffer PrimBuffer { GPUBVHPrimitive primitives[]; };
    // Binding 14: Atomic counters for bottom-up synchronization
    layout(std430, binding = 14) buffer CounterBuffer { uint nodeCounters[]; };

    uniform uint totalNodes;

    void main() {
        uint idx = gl_GlobalInvocationID.x;
        if (idx >= totalNodes) return;

        // We only start processing from Leaf Nodes
        if (nodes[idx].primCount == 0) return;

        // 1. Refit Leaf Node
        vec3 minB = vec3(1e30);
        vec3 maxB = vec3(-1e30);
        
        uint start = nodes[idx].firstPrim;
        uint end = start + nodes[idx].primCount;
        
        for(uint i = start; i < end; ++i) {
            vec3 v0 = primitives[i].v0.xyz;
            vec3 v1 = primitives[i].v1.xyz;
            vec3 v2 = primitives[i].v2.xyz;
            
            minB = min(minB, min(v0, min(v1, v2)));
            maxB = max(maxB, max(v0, max(v1, v2)));
        }
        
        nodes[idx].aabbMin = minB;
        nodes[idx].aabbMax = maxB;

        // 2. Propagate Upwards
        int current = int(idx);
        while(true) {
            int parent = nodes[current].parent;
            if (parent == -1) break; // Reached root
            
            // Atomic increment to notify parent
            uint sync = atomicAdd(nodeCounters[parent], 1);
            
            // If I am NOT the last child to finish, I stop here.
            if (sync < nodes[parent].childCount - 1) break;
            
            // I am the last child! I must refit the parent.
            // Memory barrier implied by atomic flow in some cases, but here 
            // we rely on the fact that other threads finished their writes before atomicAdd.
            // (Strictly speaking needs memoryBarrierBuffer(), but often works on NV/AMD).
            memoryBarrierBuffer();

            vec3 pMin = vec3(1e30);
            vec3 pMax = vec3(-1e30);
            
            uint childCount = nodes[parent].childCount;
            for(uint k=0; k<childCount; ++k) {
                uint childIdx = nodes[parent].children[k];
                pMin = min(pMin, nodes[childIdx].aabbMin);
                pMax = max(pMax, nodes[childIdx].aabbMax);
            }
            
            nodes[parent].aabbMin = pMin;
            nodes[parent].aabbMax = pMax;
            
            // Continue up the tree
            current = parent;
        }
    }
    )";

    inline const char* highlightVert = R"(
    #version 330 core
    layout (location = 0) in vec3 inPos;
    layout (location = 1) in vec3 inNormal;
    layout (location = 3) in ivec4 boneIDs;
    layout (location = 4) in vec4 weights;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;
    uniform float uOutlineWidth;

    const int MAX_BONES = 250;
    uniform mat4 finalBoneMatrices[MAX_BONES];
    uniform bool uIsAnimated;

    void main() {
        vec4 localPos = vec4(inPos, 1.0);
        vec3 localNormal = inNormal;

        if (uIsAnimated) {
            mat4 boneTransform = mat4(0.0);
            for(int i = 0; i < 4; i++) {
                if(boneIDs[i] == -1) continue;
                boneTransform += finalBoneMatrices[boneIDs[i]] * weights[i];
            }
            localPos = boneTransform * localPos;
            localNormal = mat3(boneTransform) * localNormal;
        }

        // Expand vertex along the normal to create the outline shell
        vec3 worldPos = vec3(uModel * localPos);
        vec3 worldNormal = normalize(mat3(transpose(inverse(uModel))) * localNormal);
        worldPos += worldNormal * uOutlineWidth;

        gl_Position = uProjection * uView * vec4(worldPos, 1.0);
    }
    )";

    inline const char* highlightFrag = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 uColor;
    uniform float uTime;

    void main() {
        // Pulse the brightness
        float pulse = 0.8 + 0.2 * sin(uTime * 5.0);
        FragColor = vec4(uColor * pulse, 1.0);
    }
    )";

    // inline const char* selectionVert = R"(
    // #version 330 core
    // layout (location = 0) in vec3 inPos;
    // layout (location = 1) in vec3 inNormal;
    // layout (location = 3) in ivec4 boneIDs;
    // layout (location = 4) in vec4 weights;

    // uniform mat4 uModel;
    // uniform mat4 uView;
    // uniform mat4 uProjection;
    // uniform float uOutlineWidth;

    // const int MAX_BONES = 250;
    // uniform mat4 finalBoneMatrices[MAX_BONES];
    // uniform bool uIsAnimated;

    // void main() {
    //     vec4 localPos = vec4(inPos, 1.0);
    //     vec3 localNormal = inNormal;

    //     if (uIsAnimated) {
    //         mat4 boneTransform = mat4(0.0);
    //         for(int i = 0; i < 4; i++) {
    //             if(boneIDs[i] >= 0 && boneIDs[i] < MAX_BONES) {
    //                 boneTransform += finalBoneMatrices[boneIDs[i]] * weights[i];
    //             }
    //         }
    //         if (length(boneTransform[0]) > 0.001) {
    //             localPos = boneTransform * localPos;
    //             localNormal = mat3(boneTransform) * localNormal;
    //         }
    //     }

    //     vec3 worldPos = vec3(uModel * localPos);
    //     vec3 worldNormal = normalize(mat3(transpose(inverse(uModel))) * localNormal);
        
    //     // Expand the mesh along its normals to create the outline shell
    //     worldPos += worldNormal * uOutlineWidth;

    //     gl_Position = uProjection * uView * vec4(worldPos, 1.0);
    // }
    // )";

    // inline const char* selectionFrag = R"(
    // #version 330 core
    // out vec4 FragColor;
    // uniform vec3 uColor;
    // uniform float uTime;

    // void main() {
    //     // Pulse effect for the outline
    //     float pulse = 0.8 + 0.2 * sin(uTime * 6.0);
    //     FragColor = vec4(uColor * pulse, 1.0);
    // }
    // )";

    inline const char* selectionMaskVert = R"(
    #version 330 core
    layout (location = 0) in vec3 inPos;
    layout (location = 3) in ivec4 boneIDs;
    layout (location = 4) in vec4 weights;

    uniform mat4 uModel;
    uniform mat4 uView;
    uniform mat4 uProjection;

    const int MAX_BONES = 250;
    uniform mat4 finalBoneMatrices[MAX_BONES];
    uniform bool uIsAnimated;

    void main() {
        vec4 localPos = vec4(inPos, 1.0);
        if (uIsAnimated) {
            mat4 boneTransform = mat4(0.0);
            for(int i = 0; i < 4; i++) {
                if(boneIDs[i] >= 0 && boneIDs[i] < MAX_BONES) {
                    boneTransform += finalBoneMatrices[boneIDs[i]] * weights[i];
                }
            }
            if (length(boneTransform[0]) > 0.001) {
                localPos = boneTransform * localPos;
            }
        }
        gl_Position = uProjection * uView * uModel * localPos;
    }
    )";

    inline const char* selectionMaskFrag = R"(
    #version 330 core
    out float maskOut;
    void main() {
        maskOut = 1.0; // Output pure white for the mask
    }
    )";

    inline const char* outlinePostProcessFrag = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 fragUV;
    
    uniform sampler2D uMaskTexture;
    uniform sampler2D gPositionMetallic; // NEW: To read view-space depth
    
    uniform vec3 uOutlineColor;
    uniform vec2 uTexelSize;
    uniform float uOutlineWidth;
    uniform float uTime;

    void main() {
        float centerMask = texture(uMaskTexture, fragUV).r;
        
        // Inside the mask = no outline
        if (centerMask > 0.5) {
            discard; 
        }

        vec2 offset = uTexelSize * uOutlineWidth;
        
        // The 8 neighboring pixels
        vec2 offsets[8] = vec2[](
            vec2(0.0, offset.y), vec2(0.0, -offset.y),
            vec2(offset.x, 0.0), vec2(-offset.x, 0.0),
            vec2(-offset.x, offset.y), vec2(offset.x, offset.y),
            vec2(-offset.x, -offset.y), vec2(offset.x, -offset.y)
        );

        bool isEdge = false;
        vec2 maskUV = fragUV;

        // Find which neighbor triggered the edge
        for(int i = 0; i < 8; i++) {
            vec2 nuv = fragUV + offsets[i];
            if (texture(uMaskTexture, nuv).r > 0.5) {
                isEdge = true;
                maskUV = nuv; // Save the UV coordinates of the actual selected object
                break;
            }
        }

        if (isEdge) {
            // Read view-space depths
            float currentDepth = texture(gPositionMetallic, fragUV).z;
            float maskDepth = texture(gPositionMetallic, maskUV).z;
            
            // In view-space, Z is negative. Closer objects have a LARGER (less negative) Z value.
            // If currentDepth >= 0.0, it's the sky/background, so it can't be an occluder.
            // We add a tiny bias (0.1) to maskDepth to prevent self-occlusion artifacts on steep curves.
            if (currentDepth < -0.0001 && currentDepth > maskDepth + 0.1) {
                discard; // An object is physically blocking this part of the outline!
            }

            float pulse = 0.8 + 0.2 * sin(uTime * 6.0);
            FragColor = vec4(uOutlineColor * pulse, 1.0);
        } else {
            discard;
        }
    }
    )";
} // namespace Engine::Graphics::Shaders;