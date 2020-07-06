/*************************************************************************
> File Name: PointsRenderable.cpp
> Project Name: CubbyFlow
> This code is based on Jet Framework that was created by Doyub Kim.
> References: https://github.com/doyubkim/fluid-engine-dev
> Purpose: Abstract base rendererable for Renderer object.
> Created Time: 2020/03/29
> Copyright (c) 2020, Ji-Hong snowapril
*************************************************************************/

#include <Framework/Renderable/PointsRenderable.h>
#include <Framework/Renderable/Material.h>
#include <Framework/Buffer/VertexBuffer.h>
#include <Framework/Buffer/IndexBuffer.h>
#include <Framework/Buffer/InputLayout.h>
#include <Framework/Buffer/Vertex.h>
#include <Framework/Renderer/Renderer.h>
#include <Framework/Shader/Shader.h>
#include <Framework/Utils/Common.h>

namespace CubbyFlow {
namespace CubbyRender {
    
    PointsRenderable::PointsRenderable()
    {
        //! Do nothing.
    }

    PointsRenderable::PointsRenderable(const ConstArrayAccessor1<Vector3F>& positions, const Vector3F& color)
    {
        update(positions, color);
    }


    PointsRenderable::~PointsRenderable()
    {
        release();
    }

    void PointsRenderable::update(const ConstArrayAccessor1<Vector3F>& positions)
    {
        update(positions, _color);
    }

    void PointsRenderable::update(const ConstArrayAccessor1<Vector3F>& positions, const Vector3F& color)
    {
        _color = color;
        const size_t totalSize = positions.size() * 3;
        const size_t numVertices = positions.size();
        std::lock_guard<std::mutex> lock(_dataMutex);
        _vertices.Resize(totalSize);
        for (size_t i = 0; i < numVertices; ++i)
        {
            _vertices[i * 3 + 0] = positions[i].x;
            _vertices[i * 3 + 1] = positions[i].y;
            _vertices[i * 3 + 2] = positions[i].z;
        }
        invalidateResources();
    }

    void PointsRenderable::onInitializeResource(RendererPtr renderer)
    {
        //! allocate and initialize material instance.
        if (_material == nullptr)
        {
            _material = renderer->createMaterial();
            ShaderPtr shader = renderer->createShaderPreset("point_shader");
            _material->setShader(shader);
        }
        //! set color uniform variable of the shader.
        auto& params = _material->getShader()->getParameters();
        params.setParameter("Color", _color);
        
        //! allocate and initialize input layout instance.
        if (_inputLayout == nullptr)
        {
            _inputLayout = renderer->createInputLayout();
            //! create vertex buffer from the positions and colors.
            VertexBufferPtr buffer = renderer->createVertexBuffer(_vertices.ConstAccessor(), 
                                                                  static_cast<size_t>(_vertices.size() / size_t(3)),  
                                                                  VertexFormat::Position3);
            _inputLayout->attachVertexBuffer(renderer, _material, buffer);
        }
        else
        {
            //! without initialization, update the contents of the buffer.
            _inputLayout->updateVertexBuffer(renderer, _vertices.ConstAccessor(), VertexFormat::Position3);
        }
        //! Set the primitive type as Point.
        _primitiveType = PrimitiveType::Point;
    }

    void PointsRenderable::onRender(RendererPtr renderer)
    {
        renderer->bindMaterial(_material);
        renderer->bindInputLayout(_inputLayout);
        renderer->setPrimitiveType(_primitiveType);
        renderer->draw(_inputLayout);
        renderer->unbindInputLayout(_inputLayout);
        renderer->unbindMaterial(_material);
    }

    void PointsRenderable::onRelease()
    {
        //! Do nothing.
    }

    void PointsRenderable::setColor(const Vector3F& color)
    {
        _color = color;
    }

    Vector3F PointsRenderable::getColor() const
    {
        return _color;
    }
} 
}