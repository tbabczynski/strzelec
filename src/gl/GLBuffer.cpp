#include "GLBuffer.h"
#include <glad/glad.h>

GLBuffer::GLBuffer()
    : m_BufferID(0)
    , m_Size(0)
{
}

GLBuffer::~GLBuffer()
{
    Destroy();
}

void GLBuffer::Create(size_t size, const void* data, unsigned int target, unsigned int usage)
{
    m_Size = size;
    glGenBuffers(1, &m_BufferID);
    glBindBuffer(target, m_BufferID);
    glBufferData(target, size, data, usage);
    glBindBuffer(target, 0);
}

void GLBuffer::Destroy()
{
    if (m_BufferID)
    {
        glDeleteBuffers(1, &m_BufferID);
        m_BufferID = 0;
        m_Size = 0;
    }
}

void GLBuffer::Bind(unsigned int target) const
{
    glBindBuffer(target, m_BufferID);
}

void GLBuffer::BindBase(unsigned int target, unsigned int index) const
{
    glBindBufferBase(target, index, m_BufferID);
}

void* GLBuffer::Map(unsigned int target, unsigned int access)
{
    glBindBuffer(target, m_BufferID);
    return glMapBuffer(target, access);
}

void GLBuffer::Unmap(unsigned int target)
{
    glUnmapBuffer(target);
    glBindBuffer(target, 0);
}

void GLBuffer::UpdateSubData(unsigned int target, size_t offset, size_t size, const void* data)
{
    glBindBuffer(target, m_BufferID);
    glBufferSubData(target, offset, size, data);
    glBindBuffer(target, 0);
}
