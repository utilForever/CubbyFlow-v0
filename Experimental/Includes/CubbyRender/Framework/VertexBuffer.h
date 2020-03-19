/*************************************************************************
> File Name: VertexBuffer.h
> Project Name: CubbyFlow
> This code is based on Jet Framework that was created by Doyub Kim.
> References: https://github.com/doyubkim/fluid-engine-dev
> Purpose: Abstract base class for VertexBuffer object.
> Created Time: 2020/02/19
> Copyright (c) 2020, Ji-Hong snowapril
*************************************************************************/
#ifndef CUBBYFLOW_VERTEXBUFFER_H
#define CUBBYFLOW_VERTEXBUFFER_H

#include <Framework/Vertex.h>
#include <Framework/Buffer.h>
#include <Framework/Prerequisites.h>
#include <memory>

namespace CubbyFlow {
namespace CubbyRender {

    //!
    //! \brief Abstract base class for Shader object.
    //!
    class VertexBuffer : public Buffer
    {
    public:
        //! Default constructor.
        VertexBuffer();

        //! Constructor with parameters
        VertexBuffer(size_t numberOfElements, VertexFormat format = VertexFormat::Position3);
        
        //! Default destructor.
        virtual ~VertexBuffer();

    protected:
        //! implementation of bind method
        virtual void onBind(RendererPtr renderer) = 0;
        
        //! implementation of unbind method
        virtual void onUnbind(RendererPtr renderer) = 0;

        //! implementation of destry method
        virtual void onDestroy(RendererPtr renderer) = 0;

        //! Allocate gpu 
        virtual void onAllocateResource(RendererPtr renderer, MaterialPtr material, const float* data, bool storeData) = 0;

        //! Update 
        virtual void onUpdateResource(RendererPtr renderer, MaterialPtr material, const float* data, bool storeData) = 0;
    private:
    };

    using VertexBufferPtr = std::shared_ptr<VertexBuffer>;
} 
}

#endif