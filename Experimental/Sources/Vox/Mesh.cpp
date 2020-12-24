/*************************************************************************
> File Name: Mesh.cpp
> Project Name: CubbyFlow
> This code is based on Jet Framework that was created by Doyub Kim.
> References: https://github.com/doyubkim/fluid-engine-dev
> Purpose: OpenGL Mesh class for sharing resourches which have same vertices data.
> Created Time: 2020/08/18
> Copyright (c) 2020, Ji-Hong snowapril
*************************************************************************/
#include <Vox/Mesh.hpp>
#include <Vox/FrameContext.hpp>
#include <glad/glad.h>

namespace Vox {

    Mesh::Mesh(const MeshShape& shape, VertexFormat format, bool bInterleaved)
    {
        GenerateMeshObject(shape, format, bInterleaved);
    }

    Mesh::~Mesh()
    {
        ClearMeshObject();
    }

    void Mesh::GenerateMeshObject(const MeshShape& shape, VertexFormat format, bool bInterleaved)
    {
        //! Genetry opengl resources.
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ebo);
        //! Bind input layout vertex array.
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        GLuint index = 0;
        size_t offset = 0;
        //! Pass Buffer data and set attributes according to whether if it is interleaved or not. 
        if (bInterleaved)
        {
            glBufferData(GL_ARRAY_BUFFER, shape.interleaved.size() * sizeof(float), shape.interleaved.data(), GL_STATIC_DRAW);
            if (static_cast<int>(format & VertexFormat::Position3))
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, VertexHelper::GetSizeInBytes(format), (void*)0);
                offset += 3; index += 1;
            }
            if (static_cast<int>(format & VertexFormat::Normal3))
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, VertexHelper::GetSizeInBytes(format), (void*)(offset * sizeof(float)));
                offset += 3; index += 1;
            }
            if (static_cast<int>(format & VertexFormat::TexCoord2))
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, VertexHelper::GetSizeInBytes(format), (void*)(offset * sizeof(float)));
            }
        }
        else
        {
            const size_t totalNum = shape.positions.size() * 3 + shape.normals.size() * 3 + shape.texCoords.size() * 2;
            glBufferData(GL_ARRAY_BUFFER, totalNum * sizeof(float), nullptr, GL_STATIC_DRAW);
            if (static_cast<int>(format & VertexFormat::Position3))
            {
                glBufferSubData(GL_ARRAY_BUFFER, 0, shape.positions.size() * sizeof(CubbyFlow::Vector3F), shape.positions.data());
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, VertexHelper::GetSizeInBytes(VertexFormat::Position3), (void*)0);
                offset += shape.positions.size() * sizeof(CubbyFlow::Vector3F); index += 1;
            }
            if (static_cast<int>(format & VertexFormat::Normal3))
            {
                glBufferSubData(GL_ARRAY_BUFFER, offset, shape.normals.size() * sizeof(CubbyFlow::Vector3F), shape.normals.data());
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, VertexHelper::GetSizeInBytes(VertexFormat::Normal3), (void*)(offset));
                offset += shape.positions.size() * sizeof(CubbyFlow::Vector3F); index += 1;
            }
            if (static_cast<int>(format & VertexFormat::TexCoord2))
            {
                glBufferSubData(GL_ARRAY_BUFFER, offset, shape.texCoords.size() * sizeof(CubbyFlow::Vector3F), shape.texCoords.data());
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, VertexHelper::GetSizeInBytes(VertexFormat::TexCoord2), (void*)(offset));
            }
        }
        //! Bind index buffer and pass data.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indices.size() * sizeof(unsigned int), shape.indices.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        //! Set the number of indices of given geometry shape.
        _numVertices = shape.indices.size();
    }

    void Mesh::ClearMeshObject()
    {
        if (_vao) glDeleteVertexArrays(1, &_vao);
        if (_vbo) glDeleteBuffers(1, &_vbo);
        if (_ebo) glDeleteBuffers(1, &_ebo);
    }

    void Mesh::DrawMesh(const std::shared_ptr<FrameContext>& ctx)
    {
        glBindVertexArray(_vao);
        glDrawElements(ctx->GetRenderStatus().primitive, _numVertices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

}