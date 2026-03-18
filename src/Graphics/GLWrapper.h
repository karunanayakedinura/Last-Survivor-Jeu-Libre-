#pragma once

// Required includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <cstdint>
#include <cstddef>
#include <cassert>

// Exception
class GLException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Helper: deleter function types
inline void deleteBuffers(GLsizei n, const GLuint* ids) { glDeleteBuffers(n, ids); }
inline void deleteTextures(GLsizei n, const GLuint* ids) { glDeleteTextures(n, ids); }
inline void deleteShader(GLsizei n, const GLuint* ids) { glDeleteShader(*ids); }
inline void deleteProgram(GLsizei n, const GLuint* ids) { glDeleteProgram(*ids); }
inline void deleteVertexArrays(GLsizei n, const GLuint* ids) { glDeleteVertexArrays(n, ids); }

// RAII base for OpenGL objects
template<void(*Deleter)(GLsizei, const GLuint*)>
class GLObject {
    GLuint id_ = 0;

public:
    // Destroys current object (if any) and takes ownership of new_id
    void reset(GLuint new_id = 0) {
        if (id_) {
            Deleter(1, &id_);
        }
        id_ = new_id;
    }

    GLObject() = default;
    explicit GLObject(GLuint id) : id_(id) {}

    // No Copying (Unique Ownership)
    GLObject(const GLObject&) = delete;
    GLObject& operator=(const GLObject&) = delete;

    // Move Constructor
    GLObject(GLObject&& other) noexcept : id_(other.id_) {
        other.id_ = 0;
    }

    // Move Assignment
    GLObject& operator=(GLObject&& other) noexcept {
        if (this != &other) {
            reset(other.id_);
            other.id_ = 0;
        }
        return *this;
    }

    ~GLObject() { reset(); }

    GLuint id() const { return id_; }
    explicit operator bool() const { return id_ != 0; }
};

// Define resource types
using Buffer = GLObject<deleteBuffers>;
using Texture = GLObject<deleteTextures>;
using Shader = GLObject<deleteShader>;
using Program = GLObject<deleteProgram>;
using VertexArray = GLObject<deleteVertexArrays>;

// Buffer data container
class BufferData {
    std::vector<uint8_t> data_;
    GLenum usage_;
public:
    BufferData(size_t size, GLenum usage = GL_STATIC_DRAW)
        : data_(size), usage_(usage) {}

    template<typename T>
    BufferData(const std::vector<T>& vec, GLenum usage = GL_STATIC_DRAW)
        : data_(reinterpret_cast<const uint8_t*>(vec.data()),
                reinterpret_cast<const uint8_t*>(vec.data()) + vec.size() * sizeof(T)),
          usage_(usage) {}

    void* data() { return data_.data(); }
    const void* data() const { return data_.data(); }
    size_t size() const { return data_.size(); }
    GLenum usage() const { return usage_; }
};

// ShaderProgram
class ShaderProgram {
    Program program_;
    mutable std::unordered_map<std::string, GLint> uniforms_;

    static GLuint compileShader(const std::string& source, GLenum type) {
        GLuint shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            glDeleteShader(shader);
            throw GLException("Shader compilation failed: " + std::string(infoLog));
        }
        return shader;
    }

public:
    ShaderProgram(const std::string& vertSource, const std::string& fragSource) {
        GLuint vert = compileShader(vertSource, GL_VERTEX_SHADER);
        GLuint frag = compileShader(fragSource, GL_FRAGMENT_SHADER);

        program_.reset(glCreateProgram());
        glAttachShader(program_.id(), vert);
        glAttachShader(program_.id(), frag);
        glLinkProgram(program_.id());

        glDeleteShader(vert);
        glDeleteShader(frag);

        GLint success;
        glGetProgramiv(program_.id(), GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(program_.id(), 512, nullptr, infoLog);
            throw GLException("Shader linking failed: " + std::string(infoLog));
        }
    }

    void use() const { glUseProgram(program_.id()); }

    GLint getUniformLocation(const std::string& name) const {
        auto it = uniforms_.find(name);
        if (it != uniforms_.end()) return it->second;

        GLint loc = glGetUniformLocation(program_.id(), name.c_str());
        if (loc == -1) {
            // Warning only, as compilers optimize out unused uniforms
            std::cerr << "Warning: Uniform not found or unused: " << name << std::endl;
        }
        uniforms_[name] = loc;
        return loc;
    }

    void setUniform(const std::string& name, int value) {
        glUniform1i(getUniformLocation(name), value);
    }
    void setUniform(const std::string& name, float value) {
        glUniform1f(getUniformLocation(name), value);
    }
    void setUniform(const std::string& name, const glm::vec2& value) {
        glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }
    void setUniform(const std::string& name, const glm::vec3& value) {
        glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }
    void setUniform(const std::string& name, const glm::vec4& value) {
        glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }
    void setUniform(const std::string& name, const glm::mat4& value) {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }
};

// ShaderStorageBuffer
class ShaderStorageBuffer {
    Buffer buffer_; // Implicitly initialized to 0
    size_t size_ = 0;
public:
    void setData(const void* data, size_t size, GLenum usage = GL_DYNAMIC_DRAW) {
        size_ = size;
        if (!buffer_) {
            GLuint id;
            glGenBuffers(1, &id);
            buffer_.reset(id);
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_.id());
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
    }

    void bindBase(GLuint index) const {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, buffer_.id());
    }
    
    GLuint id() const { return buffer_.id(); }
    size_t size() const { return size_; }
    void reset() { buffer_.reset(); size_ = 0; }
};

// VertexBuffer - Fixed: removed manual move/destructor, relied on Buffer (GLObject)
class VertexBuffer {
    Buffer buffer_; // Removed "= 0" which is invalid for explicit constructor
public:
    // Uses default destructor (calls Buffer destructor -> resets GL object)
    // Uses default move constructor/assignment (transfers Buffer ownership)

    void setData(const BufferData& data) {
        if (!buffer_) {
            GLuint id;
            glGenBuffers(1, &id);
            buffer_.reset(id);
        }
        glBindBuffer(GL_ARRAY_BUFFER, buffer_.id());
        glBufferData(GL_ARRAY_BUFFER, data.size(), data.data(), data.usage());
    }

    void updateData(const void* data, size_t size, size_t offset = 0) {
        if (!buffer_) {
            throw GLException("Buffer not initialized. Call setData first.");
        }
        glBindBuffer(GL_ARRAY_BUFFER, buffer_.id());
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    }

    void bind() const { glBindBuffer(GL_ARRAY_BUFFER, buffer_.id()); }
    void reset() { buffer_.reset(); }
};

// IndexBuffer
class IndexBuffer {
    Buffer buffer_;
    GLsizei count_ = 0;
public:
    void setData(const std::vector<uint32_t>& indices) {
        count_ = static_cast<GLsizei>(indices.size());
        if (!buffer_) {
            GLuint id;
            glGenBuffers(1, &id);
            buffer_.reset(id);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_.id());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(uint32_t),
                     indices.data(),
                     GL_STATIC_DRAW);
    }

    void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_.id()); }
    GLsizei count() const { return count_; }
    void reset() { buffer_.reset(); count_ = 0; }
};

// GLVertexArray - Fixed: Lazy initialization logic
class GLVertexArray {
    VertexArray vao_;
public:
    // Default constructor is now empty (creates invalid/null object)
    // We lazily create the GL handle when needed, OR explicitly create it.
    GLVertexArray() = default; 

    // Explicitly create a valid VAO (useful for Mesh initialization)
    static GLVertexArray Create() {
        GLVertexArray v;
        GLuint id;
        glGenVertexArrays(1, &id);
        v.vao_.reset(id);
        return v;
    }

    // Ensures the VAO exists before binding
    void bind() { 
        if (!vao_) {
            GLuint id;
            glGenVertexArrays(1, &id);
            vao_.reset(id);
        }
        glBindVertexArray(vao_.id()); 
    }

    // Const bind assumes it exists (assert or check)
    void bind() const { 
        if (vao_) glBindVertexArray(vao_.id()); 
    }

    GLuint id() const { return vao_.id(); }
    void reset() { vao_.reset(); }

    void setAttribute(GLuint index, GLint size, GLenum type,
                      GLsizei stride, const void* offset,
                      bool normalized = false) {
        bind(); 
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, type, normalized, stride, offset);
    }

    void setAttributeI(GLuint index, GLint size, GLenum type,
                      GLsizei stride, const void* offset) {
        bind(); 
        glEnableVertexAttribArray(index);
        glVertexAttribIPointer(index, size, type, stride, offset);
    }
};

// Texture2D
class Texture2D {
    Texture texture_;
    GLenum format_;
    int width_, height_;

public:
    Texture2D(int width, int height, const void* data = nullptr,
              GLenum format = GL_RGB, GLenum internalFormat = GL_RGB)
        : format_(format), width_(width), height_(height) {
        GLuint id;
        glGenTextures(1, &id);
        texture_.reset(id);
        
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height,
                     0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void bind(GLuint unit = 0) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture_.id());
    }

    GLuint id() const { return texture_.id(); }
    int width() const { return width_; }
    int height() const { return height_; }
    void reset() { texture_.reset(); }
};

// RenderContext
class RenderContext {
    GLFWwindow* window_ = nullptr;

public:
    RenderContext(int width, int height, const char* title) {
        if (!glfwInit()) throw GLException("Failed to initialize GLFW");

        // UPGRADED TO OPENGL 4.5 FOR DDGI (Compute Shaders), previously 3.3
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window_) {
            glfwTerminate();
            throw GLException("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window_);

        GLenum err = glewInit();
        if (err != GLEW_OK) {
            glfwTerminate();
            throw GLException("Failed to initialize GLEW: " + 
                            std::string((const char*)glewGetErrorString(err)));
        }
        
        glEnable(GL_DEPTH_TEST);
    }

    ~RenderContext() {
        glfwTerminate();
    }

    void swapBuffers() { glfwSwapBuffers(window_); }
    bool shouldClose() { return glfwWindowShouldClose(window_); }
    void pollEvents() { glfwPollEvents(); }
    void setClearColor(float r, float g, float b, float a = 1.0f) { glClearColor(r, g, b, a); }
    void clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
    GLFWwindow* nativeHandle() { return window_; }
};

// --------------------------------------------------------
// MESH STRUCT (Moved from mesh.h and fixed)
// --------------------------------------------------------

struct Mesh {
    public:
        // CPU-side data
        std::vector<float> vertex_data; // (x, y, z, nx, ny, nz, u, v) (8)
        std::vector<uint32_t> indices;
        int vertexCount = 0;
        
        // Bone data
        std::vector<int> bone_ids;         // 4 bone IDs per vertex
        std::vector<float> bone_weights;   // 4 weights per vertex

        // GPU DATA (Wrappers)
        std::shared_ptr<Texture2D> texture; 
        int materialIndex = 0;

        glm::vec3 minBound = glm::vec3(FLT_MAX); 
        glm::vec3 maxBound = glm::vec3(-FLT_MAX);
        glm::vec3 centroid = glm::vec3(0.0f, 0.0f, 0.0f);

        GLVertexArray vao;
        VertexBuffer vbo;
        IndexBuffer ibo;
        VertexBuffer bone_vbo;
        VertexBuffer weight_vbo;

        bool Processed = false;

        Mesh() = default;

        Mesh(std::vector<float> data) {
            vertex_data = data;
        }

        // --- Copy Constructor ---
        // Deep copies CPU data, creates NEW GPU resources
        Mesh(const Mesh& other) {
            vertex_data = other.vertex_data;
            indices = other.indices;
            vertexCount = other.vertexCount;
            bone_ids = other.bone_ids;
            bone_weights = other.bone_weights;
            texture = other.texture; // Shared ptr copy
            
            if (vertexCount > 0) {
                upload(); 
            }
        }

        // --- Copy Assignment ---
        Mesh& operator=(const Mesh& other) {
            if (this != &other) {
                vertex_data = other.vertex_data;
                indices = other.indices;
                vertexCount = other.vertexCount;
                bone_ids = other.bone_ids;
                bone_weights = other.bone_weights;
                texture = other.texture;
                
                if (vertexCount > 0) {
                    upload();
                } else {
                    Processed = false;
                }
            }
            return *this;
        }

        // --- Move Operations ---
        // Default is fine because wrappers (GLVertexArray, etc.) handle move correctly
        Mesh(Mesh&&) = default;
        Mesh& operator=(Mesh&&) = default;

        // Destructor: Must clean up GPU resources
        ~Mesh() {
            Destroy();
        }

        void AddBoneData(unsigned int vertexID, int boneID, float weight) {
            for (int i = 0; i < 4; ++i) { 
                if (bone_ids[vertexID * 4 + i] == -1) { 
                    bone_ids[vertexID * 4 + i] = boneID;
                    bone_weights[vertexID * 4 + i] = weight;
                    return; 
                }
            }
        }
        
        void upload() {
            vertexCount = (int) vertex_data.size() / 8;
            if (vertexCount == 0) {
                Processed = false;
                return;
            }

            ComputeBoundsAndCentroid();

            // RAII: Assigning new wrappers resets old ones automatically.
            // Explicitly creating a new VAO here to ensure fresh state.
            vao = GLVertexArray::Create(); 
            // VBOs are reset automatically when setData is called internally or lazily init
            vbo.reset(); 
            ibo.reset();
            bone_vbo.reset();
            weight_vbo.reset();

            vao.bind();

            // 1. Main VBO
            vbo.setData(BufferData(vertex_data, GL_DYNAMIC_DRAW)); 
            vbo.bind();

            // Attributes
            vao.setAttribute(0, 3, GL_FLOAT, 8 * sizeof(float), (void*)(0)); // pos
            vao.setAttribute(1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float))); // norm
            vao.setAttribute(2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float))); // uv

            // 2. Bone VBOs
            if (!bone_ids.empty() && !bone_weights.empty()) {
                assert(bone_ids.size() == vertexCount * 4);
                assert(bone_weights.size() == vertexCount * 4);

                bone_vbo.setData(BufferData(bone_ids, GL_STATIC_DRAW));
                bone_vbo.bind();
                vao.setAttributeI(3, 4, GL_INT, 0, (void*)0); 

                weight_vbo.setData(BufferData(bone_weights, GL_STATIC_DRAW));
                weight_vbo.bind();
                vao.setAttribute(4, 4, GL_FLOAT, 0, (void*)0);
            }

            // 3. Index Buffer
            if (!indices.empty()) {
                ibo.setData(indices);
                ibo.bind();
            }

            glBindVertexArray(0);
            Processed = true;
        }

        void draw() {
            if (!Processed) return;
            vao.bind();
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
        }

        void ComputeBoundsAndCentroid() {
            // Reset bounds
            minBound = glm::vec3(FLT_MAX);
            maxBound = glm::vec3(-FLT_MAX);

            if (vertex_data.empty()) {
                centroid = glm::vec3(0.0f);
                return;
            }

            // vertex_data layout: (x, y, z, nx, ny, nz, u, v)
            constexpr int stride = 8;

            for (size_t i = 0; i < vertex_data.size(); i += stride) {
                glm::vec3 p(
                    vertex_data[i + 0],
                    vertex_data[i + 1],
                    vertex_data[i + 2]
                );

                minBound = glm::min(minBound, p);
                maxBound = glm::max(maxBound, p);
            }

            // Centroid = center of AABB
            centroid = (minBound + maxBound) * 0.5f;
        }
        
        void Destroy() {
            // EFFICIENT RESET: Instead of allocating NEW objects to replace old ones,
            // we just reset the IDs to 0, freeing the GPU memory via GLObject logic.
            vao.reset();
            vbo.reset();
            ibo.reset();
            bone_vbo.reset();
            weight_vbo.reset();
            
            texture.reset(); 

            // Clear CPU data
            vertex_data.clear();
            indices.clear();
            bone_ids.clear();
            bone_weights.clear();
            
            vertexCount = 0;
            Processed = false;
        }
};