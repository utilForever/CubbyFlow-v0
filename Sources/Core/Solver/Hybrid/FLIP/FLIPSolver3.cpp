// This code is based on Jet framework.
// Copyright (c) 2018 Doyub Kim
// CubbyFlow is voxel-based fluid simulation engine for computer games.
// Copyright (c) 2020 CubbyFlow Team
// Core Part: Chris Ohk, Junwoo Hwang, Jihong Sin, Seungwoo Yoo
// AI Part: Dongheon Cho, Minseo Kim
// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Core/Solver/Hybrid/FLIP/FLIPSolver3.hpp>

namespace CubbyFlow
{
FLIPSolver3::FLIPSolver3()
    : FLIPSolver3{ { 1, 1, 1 }, { 1, 1, 1 }, { 0, 0, 0 } }
{
    // Do nothing
}

FLIPSolver3::FLIPSolver3(const Vector3UZ& resolution,
                         const Vector3D& gridSpacing,
                         const Vector3D& gridOrigin)
    : PICSolver3{ resolution, gridSpacing, gridOrigin }
{
    // Do nothing
}

double FLIPSolver3::GetPICBlendingFactor() const
{
    return m_picBlendingFactor;
}

void FLIPSolver3::SetPICBlendingFactor(double factor)
{
    m_picBlendingFactor = std::clamp(factor, 0.0, 1.0);
}

void FLIPSolver3::TransferFromParticlesToGrids()
{
    PICSolver3::TransferFromParticlesToGrids();

    // Store snapshot
    const FaceCenteredGrid3Ptr vel = GetGridSystemData()->GetVelocity();
    ConstArrayView3<double> u = GetGridSystemData()->GetVelocity()->UView();
    ConstArrayView3<double> v = GetGridSystemData()->GetVelocity()->VView();
    ConstArrayView3<double> w = GetGridSystemData()->GetVelocity()->WView();
    m_uDelta.Resize(u.Size());
    m_vDelta.Resize(v.Size());
    m_wDelta.Resize(w.Size());

    vel->ParallelForEachUIndex(
        [&](size_t i, size_t j, size_t k) { m_uDelta(i, j, k) = u(i, j, k); });
    vel->ParallelForEachVIndex(
        [&](size_t i, size_t j, size_t k) { m_vDelta(i, j, k) = v(i, j, k); });
    vel->ParallelForEachWIndex(
        [&](size_t i, size_t j, size_t k) { m_wDelta(i, j, k) = w(i, j, k); });
}

void FLIPSolver3::TransferFromGridsToParticles()
{
    FaceCenteredGrid3Ptr flow = GetGridSystemData()->GetVelocity();
    ArrayView1<Vector3<double>> positions =
        GetParticleSystemData()->Positions();
    ArrayView1<Vector3<double>> velocities =
        GetParticleSystemData()->Velocities();
    const size_t numberOfParticles =
        GetParticleSystemData()->NumberOfParticles();

    // Compute delta
    flow->ParallelForEachUIndex([&](size_t i, size_t j, size_t k) {
        m_uDelta(i, j, k) = flow->GetU(i, j, k) - m_uDelta(i, j, k);
    });

    flow->ParallelForEachVIndex([&](size_t i, size_t j, size_t k) {
        m_vDelta(i, j, k) = flow->GetV(i, j, k) - m_vDelta(i, j, k);
    });

    flow->ParallelForEachWIndex([&](size_t i, size_t j, size_t k) {
        m_wDelta(i, j, k) = flow->GetW(i, j, k) - m_wDelta(i, j, k);
    });

    LinearArraySampler3<double> uSampler{ m_uDelta.View(),
                                          flow->GridSpacing().CastTo<double>(),
                                          flow->UOrigin().CastTo<double>() };
    LinearArraySampler3<double> vSampler{ m_vDelta.View(),
                                          flow->GridSpacing().CastTo<double>(),
                                          flow->VOrigin().CastTo<double>() };
    LinearArraySampler3<double> wSampler{ m_wDelta.View(),
                                          flow->GridSpacing().CastTo<double>(),
                                          flow->WOrigin().CastTo<double>() };

    auto sampler = [uSampler, vSampler, wSampler](const Vector3D& x) {
        const Vector3<double> xf = x.CastTo<double>();
        const double u = uSampler(xf);
        const double v = vSampler(xf);
        const double w = wSampler(xf);
        return Vector3D{ u, v, w };
    };

    // Transfer delta to the particles
    ParallelFor(ZERO_SIZE, numberOfParticles, [&](size_t i) {
        Vector3D flipVel = velocities[i] + sampler(positions[i]);

        if (m_picBlendingFactor > 0.0)
        {
            const Vector3D picVel = flow->Sample(positions[i]);
            flipVel = Lerp(flipVel, picVel, m_picBlendingFactor);
        }

        velocities[i] = flipVel;
    });
}

FLIPSolver3::Builder FLIPSolver3::GetBuilder()
{
    return Builder{};
}

FLIPSolver3 FLIPSolver3::Builder::Build() const
{
    return FLIPSolver3{ m_resolution, GetGridSpacing(), m_gridOrigin };
}

FLIPSolver3Ptr FLIPSolver3::Builder::MakeShared() const
{
    return std::shared_ptr<FLIPSolver3>(
        new FLIPSolver3{ m_resolution, GetGridSpacing(), m_gridOrigin },
        [](FLIPSolver3* obj) { delete obj; });
}
}  // namespace CubbyFlow