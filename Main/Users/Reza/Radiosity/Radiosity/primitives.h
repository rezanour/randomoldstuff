#pragma once

struct Vertex
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT2 LightUV;
    DirectX::XMFLOAT2 DiffuseUV;

    Vertex(float x, float y, float z) : Position(x, y, z), LightUV(0, 0), DiffuseUV(0, 0) {}

    Vertex(const Vertex& other) : Position(other.Position), LightUV(other.LightUV), DiffuseUV(other.DiffuseUV) {}
};

struct Quad
{
    Vertex A;
    Vertex B;
    Vertex C;
    Vertex D;

    Quad(Vertex a, Vertex b, Vertex c, Vertex d) : A(a), B(b), C(c), D(d)
    {
        A.DiffuseUV = DirectX::XMFLOAT2(0, 1);
        B.DiffuseUV = DirectX::XMFLOAT2(0, 0);
        C.DiffuseUV = DirectX::XMFLOAT2(1, 0);
        D.DiffuseUV = DirectX::XMFLOAT2(1, 1);
    }

    Quad(const Quad& other) : A(other.A), B(other.B), C(other.C), D(other.D) {}
};

