// This code is based on Jet framework.
// Copyright (c) 2018 Doyub Kim
// CubbyFlow is voxel-based fluid simulation engine for computer games.
// Copyright (c) 2020 CubbyFlow Team
// Core Part: Chris Ohk, Junwoo Hwang, Jihong Sin, Seungwoo Yoo
// AI Part: Dongheon Cho, Minseo Kim
// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Core/Surface/SurfaceSet3.hpp>

namespace CubbyFlow
{
SurfaceSet3::SurfaceSet3()
{
    // Do nothing
}

SurfaceSet3::SurfaceSet3(const std::vector<Surface3Ptr>& others,
                         const Transform3& transform, bool isNormalFlipped)
    : Surface3(transform, isNormalFlipped), m_surfaces(others)
{
    for (const auto& surface : m_surfaces)
    {
        if (!surface->IsBounded())
        {
            m_unboundedSurfaces.push_back(surface);
        }
    }

    InvalidateBVH();
}

SurfaceSet3::SurfaceSet3(const SurfaceSet3& other)
    : Surface3(other),
      m_surfaces(other.m_surfaces),
      m_unboundedSurfaces(other.m_unboundedSurfaces)
{
    InvalidateBVH();
}

void SurfaceSet3::UpdateQueryEngine()
{
    BuildBVH();
}

bool SurfaceSet3::IsValidGeometry() const
{
    // All surfaces should be valid.
    for (const auto& surface : m_surfaces)
    {
        if (!surface->IsValidGeometry())
        {
            return false;
        }
    }

    // Empty set is not valid.
    return !m_surfaces.empty();
}

size_t SurfaceSet3::NumberOfSurfaces() const
{
    return m_surfaces.size();
}

const Surface3Ptr& SurfaceSet3::SurfaceAt(size_t i) const
{
    return m_surfaces[i];
}

void SurfaceSet3::AddSurface(const Surface3Ptr& surface)
{
    m_surfaces.push_back(surface);

    if (!surface->IsBounded())
    {
        m_unboundedSurfaces.push_back(surface);
    }

    InvalidateBVH();
}

SurfaceSet3::Builder SurfaceSet3::GetBuilder()
{
    return Builder();
}

Vector3D SurfaceSet3::ClosestPointLocal(const Vector3D& otherPoint) const
{
    BuildBVH();

    const auto distanceFunc = [](const Surface3Ptr& surface,
                                 const Vector3D& pt) {
        return surface->ClosestDistance(pt);
    };

    Vector3D result{ std::numeric_limits<double>::max(),
                     std::numeric_limits<double>::max(),
                     std::numeric_limits<double>::max() };

    const auto queryResult = m_bvh.GetNearestNeighbor(otherPoint, distanceFunc);
    if (queryResult.item != nullptr)
    {
        result = (*queryResult.item)->ClosestPoint(otherPoint);
    }

    double minDist = queryResult.distance;
    for (const auto& surface : m_unboundedSurfaces)
    {
        auto pt = surface->ClosestPoint(otherPoint);
        const double dist = pt.DistanceTo(otherPoint);

        if (dist < minDist)
        {
            minDist = dist;
            result = surface->ClosestPoint(otherPoint);
        }
    }

    return result;
}

Vector3D SurfaceSet3::ClosestNormalLocal(const Vector3D& otherPoint) const
{
    BuildBVH();

    const auto distanceFunc = [](const Surface3Ptr& surface,
                                 const Vector3D& pt) {
        return surface->ClosestDistance(pt);
    };

    Vector3D result{ 1.0, 0.0, 0.0 };

    const auto queryResult = m_bvh.GetNearestNeighbor(otherPoint, distanceFunc);
    if (queryResult.item != nullptr)
    {
        result = (*queryResult.item)->ClosestNormal(otherPoint);
    }

    double minDist = queryResult.distance;
    for (const auto& surface : m_unboundedSurfaces)
    {
        auto pt = surface->ClosestPoint(otherPoint);
        const double dist = pt.DistanceTo(otherPoint);

        if (dist < minDist)
        {
            minDist = dist;
            result = surface->ClosestNormal(otherPoint);
        }
    }

    return result;
}

double SurfaceSet3::ClosestDistanceLocal(const Vector3D& otherPoint) const
{
    BuildBVH();

    const auto distanceFunc = [](const Surface3Ptr& surface,
                                 const Vector3D& pt) {
        return surface->ClosestDistance(pt);
    };

    const auto queryResult = m_bvh.GetNearestNeighbor(otherPoint, distanceFunc);

    double minDist = queryResult.distance;
    for (const auto& surface : m_unboundedSurfaces)
    {
        auto pt = surface->ClosestPoint(otherPoint);
        const double dist = pt.DistanceTo(otherPoint);

        if (dist < minDist)
        {
            minDist = dist;
        }
    }

    return minDist;
}

bool SurfaceSet3::IntersectsLocal(const Ray3D& ray) const
{
    BuildBVH();

    const auto testFunc = [](const Surface3Ptr& surface, const Ray3D& ray) {
        return surface->Intersects(ray);
    };

    bool result = m_bvh.IsIntersects(ray, testFunc);
    for (const auto& surface : m_unboundedSurfaces)
    {
        result |= surface->Intersects(ray);
    }

    return result;
}

SurfaceRayIntersection3 SurfaceSet3::ClosestIntersectionLocal(
    const Ray3D& ray) const
{
    BuildBVH();

    const auto testFunc = [](const Surface3Ptr& surface, const Ray3D& ray) {
        SurfaceRayIntersection3 result = surface->ClosestIntersection(ray);
        return result.distance;
    };

    const auto queryResult = m_bvh.GetClosestIntersection(ray, testFunc);

    SurfaceRayIntersection3 result;
    result.distance = queryResult.distance;
    result.isIntersecting = queryResult.item != nullptr;

    if (queryResult.item != nullptr)
    {
        result.point = ray.PointAt(queryResult.distance);
        result.normal = (*queryResult.item)->ClosestNormal(result.point);
    }

    for (const auto& surface : m_unboundedSurfaces)
    {
        const SurfaceRayIntersection3 localResult =
            surface->ClosestIntersection(ray);
        if (localResult.distance < result.distance)
        {
            result = localResult;
        }
    }

    return result;
}

BoundingBox3D SurfaceSet3::BoundingBoxLocal() const
{
    BuildBVH();

    return m_bvh.GetBoundingBox();
}

bool SurfaceSet3::IsInsideLocal(const Vector3D& otherPoint) const
{
    for (const auto& surface : m_surfaces)
    {
        if (surface->IsInside(otherPoint))
        {
            return true;
        }
    }

    return false;
}

void SurfaceSet3::InvalidateBVH() const
{
    m_bvhInvalidated = true;
}

void SurfaceSet3::BuildBVH() const
{
    if (m_bvhInvalidated)
    {
        std::vector<BoundingBox3D> bounds;

        for (const auto& surface : m_surfaces)
        {
            if (surface->IsBounded())
            {
                bounds.push_back(surface->BoundingBox());
            }
        }

        m_bvh.Build(m_surfaces, bounds);
        m_bvhInvalidated = false;
    }
}

SurfaceSet3::Builder& SurfaceSet3::Builder::WithSurfaces(
    const std::vector<Surface3Ptr>& others)
{
    m_surfaces = others;
    return *this;
}

SurfaceSet3 SurfaceSet3::Builder::Build() const
{
    return SurfaceSet3(m_surfaces, m_transform, m_isNormalFlipped);
}

SurfaceSet3Ptr SurfaceSet3::Builder::MakeShared() const
{
    return std::shared_ptr<SurfaceSet3>(
        new SurfaceSet3(m_surfaces, m_transform, m_isNormalFlipped),
        [](SurfaceSet3* obj) { delete obj; });
}
}  // namespace CubbyFlow