// This code is based on Jet framework.
// Copyright (c) 2018 Doyub Kim
// CubbyFlow is voxel-based fluid simulation engine for computer games.
// Copyright (c) 2020 CubbyFlow Team
// Core Part: Chris Ohk, Junwoo Hwang, Jihong Sin, Seungwoo Yoo
// AI Part: Dongheon Cho, Minseo Kim
// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef CUBBYFLOW_SURFACE_TO_IMPLICIT_HPP
#define CUBBYFLOW_SURFACE_TO_IMPLICIT_HPP

#include <Core/Geometry/ImplicitSurface.hpp>

#include <memory>

namespace CubbyFlow
{
//!
//! \brief N-D implicit surface wrapper for generic Surface instance.
//!
//! This class represents N-D implicit surface that converts Surface instance
//! to an ImplicitSurface object. The conversion is made by evaluating closest
//! point and normal from a given point for the given (explicit) surface. Thus,
//! this conversion won't work for every single surfaces. Use this class only
//! for the basic primitives such as Sphere or Box.
//!
template <size_t N>
class SurfaceToImplicit final : public ImplicitSurface<N>
{
 public:
    class Builder;

    using ImplicitSurface<N>::transform;
    using ImplicitSurface<N>::isNormalFlipped;

    //! Constructs an instance with generic Surface2 instance.
    SurfaceToImplicit(const std::shared_ptr<Surface<N>>& surface,
                      const Transform<N>& _transform = Transform<N>(),
                      bool _isNormalFlipped = false);

    //! Copy constructor.
    SurfaceToImplicit(const SurfaceToImplicit& other);

    //! Updates internal spatial query engine.
    void UpdateQueryEngine() override;

    //! Returns true if bounding box can be defined.
    bool IsBounded() const override;

    //! Returns true if the surface is a valid geometry.
    bool IsValidGeometry() const override;

    //! Returns the raw surface instance.
    std::shared_ptr<Surface<N>> GetSurface() const;

    //! Returns builder fox SurfaceToImplicit.
    static Builder GetBuilder();

 protected:
    Vector<double, N> ClosestPointLocal(
        const Vector<double, N>& otherPoint) const override;

    double ClosestDistanceLocal(
        const Vector<double, N>& otherPoint) const override;

    bool IntersectsLocal(const Ray<double, N>& ray) const override;

    BoundingBox<double, N> BoundingBoxLocal() const override;

    Vector<double, N> ClosestNormalLocal(
        const Vector<double, N>& otherPoint) const override;

    double SignedDistanceLocal(
        const Vector<double, N>& otherPoint) const override;

    SurfaceRayIntersection<N> ClosestIntersectionLocal(
        const Ray<double, N>& ray) const override;

 private:
    std::shared_ptr<Surface<N>> m_surface;
};

//! 2-D SurfaceToImplicit type.
using SurfaceToImplicit2 = SurfaceToImplicit<2>;

//! 3-D SurfaceToImplicit type.
using SurfaceToImplicit3 = SurfaceToImplicit<3>;

//! Shared pointer for the SurfaceToImplicit2 type.
using SurfaceToImplicit2Ptr = std::shared_ptr<SurfaceToImplicit2>;

//! Shared pointer for the SurfaceToImplicit3 type.
using SurfaceToImplicit3Ptr = std::shared_ptr<SurfaceToImplicit3>;

//!
//! \brief Front-end to create SurfaceToImplicit objects step by step.
//!
template <size_t N>
class SurfaceToImplicit<N>::Builder final
    : public SurfaceBuilderBase<N, typename SurfaceToImplicit<N>::Builder>
{
 public:
    //! Returns builder with surface.
    Builder& WithSurface(const std::shared_ptr<Surface<N>>& surface);

    //! Builds SurfaceToImplicit.
    SurfaceToImplicit Build() const;

    //! Builds shared pointer of SurfaceToImplicit instance.
    std::shared_ptr<SurfaceToImplicit> MakeShared() const;

 private:
    using Base = SurfaceBuilderBase<N, typename SurfaceToImplicit<N>::Builder>;
    using Base::m_isNormalFlipped;
    using Base::m_transform;

    std::shared_ptr<Surface<N>> m_surface;
};
}  // namespace CubbyFlow

#endif