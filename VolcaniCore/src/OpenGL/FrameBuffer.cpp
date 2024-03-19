#include "FrameBuffer.h"

#include <glad/glad.h>

namespace VolcaniCore
{

FrameBuffer::FrameBuffer(const AttachmentSpecification& specs)
{
    glGenFramebuffers(1, &m_BufferID);
    glGenTextures(1, &m_TextureID);
    glGenRenderbuffers(1, &m_RenderbufferID);

    switch(specs.Color)
    {
        case AttachmentType::Texture:
            glBindTexture(GL_TEXTURE_2D, m_TextureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, specs.Width, specs.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0);
            break;

        case AttachmentType::RenderBuffer:
            break;
    }

    if(specs.Depth == AttachmentType::RenderBuffer && specs.Stencil == AttachmentType::RenderBuffer)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, specs.Width, specs.Height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID);
    }

    switch(specs.Depth)
    {
        case AttachmentType::Texture:
            glBindTexture(GL_TEXTURE_2D, m_TextureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, specs.Width, specs.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        
        case AttachmentType::RenderBuffer:
            break;
    }

    switch(specs.Stencil)
    {

    }

    if(specs.Color == AttachmentType::None)
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    VOLCANICORE_ASSERT((glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteTextures(1, &m_TextureID);
    glDeleteRenderbuffers(1, &m_RenderbufferID);
    glDeleteBuffers(1, &m_BufferID);
}

void FrameBuffer::Bind() { glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID); }
void FrameBuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void FrameBuffer::BindTexture() { glBindTexture(GL_TEXTURE_2D, m_TextureID); }
void FrameBuffer::BindRenderbuffer() { glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID); }

}