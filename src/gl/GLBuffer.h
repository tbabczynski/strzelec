#pragma once

#include <cstddef>

class GLBuffer
{
public:
    GLBuffer();
    ~GLBuffer();

    void Create(size_t size, const void* data, unsigned int target, unsigned int usage);
    void Destroy();
    void Bind(unsigned int target) const;
    void BindBase(unsigned int target, unsigned int index) const;
    void* Map(unsigned int target, unsigned int access);
    void Unmap(unsigned int target);
    void UpdateSubData(unsigned int target, size_t offset, size_t size, const void* data);

    unsigned int GetID() const { return m_BufferID; }
    size_t GetSize() const { return m_Size; }

private:
    unsigned int m_BufferID;
    size_t m_Size;
};
