#pragma once

struct Vertex
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT3 Normal;

    Vertex(float x, float y, float z, float nx, float ny, float nz) : Position(x, y, z), Normal(nx, ny, nz) {}

    Vertex(const Vertex& other) : Position(other.Position), Normal(other.Normal) {}
};

struct Quad
{
    Vertex A;
    Vertex B;
    Vertex C;
    Vertex D;

    Quad(Vertex a, Vertex b, Vertex c, Vertex d) : A(a), B(b), C(c), D(d) {}
    Quad(const Quad& other) : A(other.A), B(other.B), C(other.C), D(other.D) {}
};

