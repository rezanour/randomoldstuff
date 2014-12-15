#pragma once

HRESULT SaveAsWaveFrontOBJ(const wchar_t* path, uint32_t numAttributes, const GDK::GeometryContent::AttributeDesc* attributes, uint32_t stride, const byte_t* vertices, size_t numVertices, const uint32_t* indices, size_t numIndices );
