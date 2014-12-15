#pragma once

class Bsp : public BaseObject<Bsp>, public IBsp
{
public:
    static std::shared_ptr<Bsp> Create(_Inout_ BspData& bsp);

    //
    // IBsp
    //

    const BspData& GetData() const override;

    _Success_(return)
    bool ClipLineSegment(_In_ const XMVECTOR& start, _In_ const XMVECTOR& end, _Out_ ClipResult& result) const override;

    void CheckEllipsoid(_Inout_ CollisionQuery& query) const override;

private:
    Bsp(_Inout_ BspData& bsp);

    _Success_(return)
    bool ClipLineSegment(_In_ const XMVECTOR& start, _In_ const XMVECTOR& end, _In_ int32_t nodeIndex, _Out_ ClipResult& result) const;

    bool CheckEllipsoid(_Inout_ CollisionQuery& query, _In_ int32_t nodeIndex) const;

    static bool TestLineSegmentTriangle(_In_ const XMVECTOR& start, _In_ const XMVECTOR& end, _In_ const BspTriangle& triangle, _Out_ ClipResult& result);

private:
    BspData _data;
};
