#pragma once
#include <vector>

#include "SharedStruct.h"
using namespace DirectX;
struct Mesh
{
	std::vector<Vertex> Vertices; // 頂点データの配列
	std::vector<uint32_t> Indices; // インデックスの配列
	std::wstring DiffuseMap; // テクスチャのファイルパス
};

struct TriangleMesh : Mesh {

	TriangleMesh(XMFLOAT4 vertexColor = { 0,0,0,0 }, std::wstring diffuseMap = L"")
	{
		Vertices = std::vector<Vertex>(3);
		Vertices[0].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
		Vertices[1].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
		Vertices[2].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
		for (auto& vertex : Vertices) {
			vertex.Color = vertexColor;
		}

		Indices = { 0, 1, 2 }; // これに書かれている順序で描画する
		DiffuseMap = diffuseMap;
	}
private:
	void CreateMesh()
	{

	}
};

struct SquareMesh : Mesh {

	SquareMesh(XMFLOAT4 vertexColor = { 0,0,0,0 }, std::wstring diffuseMap = L"")
	{
		Vertices = std::vector<Vertex>(4);
		Vertices[0].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
		Vertices[0].UV = { 0.0, 1.0 };
		Vertices[1].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
		Vertices[1].UV = { 1.0, 1.0 };
		Vertices[2].Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
		Vertices[2].UV = { 0.0, 0.0 };
		Vertices[3].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
		Vertices[3].UV = { 1.0, 0.0 };
		for (auto& vertex : Vertices) {
			vertex.Color = vertexColor;
			vertex.Normal = {0,0,1};
		}


		Indices = { 0, 1, 2, 1, 3, 2 }; // これに書かれている順序で描画する
		DiffuseMap = diffuseMap;
	}
};

struct CubeMesh : Mesh
{
	CubeMesh(XMFLOAT4 vertexColor = { 0,0,0,1 }, std::wstring diffuseMap = L"")
	{
		const float k = 1.0f;


		Vertex vertexList[]{
		{ { -k,  k, -k }, {  0.0f,  0.0f, -1.0f } },
		{ {  k,  k, -k }, {  0.0f,  0.0f, -1.0f } },
		{ { -k, -k, -k }, {  0.0f,  0.0f, -1.0f } },
		{ {  k, -k, -k }, {  0.0f,  0.0f, -1.0f } },

		{ { -k,  k,  k }, {  0.0f,  0.0f,  1.0f } },
		{ { -k, -k,  k }, {  0.0f,  0.0f,  1.0f } },
		{ {  k,  k,  k }, {  0.0f,  0.0f,  1.0f } },
		{ {  k, -k,  k }, {  0.0f,  0.0f,  1.0f } },

		{ { -k,  k,  k }, { -1.0f,  0.0f,  0.0f } },
		{ { -k,  k, -k }, { -1.0f,  0.0f,  0.0f } },
		{ { -k, -k,  k }, { -1.0f,  0.0f,  0.0f } },
		{ { -k, -k, -k }, { -1.0f,  0.0f,  0.0f } },

		{ {  k,  k,  k }, {  1.0f,  0.0f,  0.0f } },
		{ {  k, -k,  k }, {  1.0f,  0.0f,  0.0f } },
		{ {  k,  k, -k }, {  1.0f,  0.0f,  0.0f } },
		{ {  k, -k, -k }, {  1.0f,  0.0f,  0.0f } },

		{ { -k,  k,  k }, {  0.0f,  1.0f,  0.0f } },
		{ {  k,  k,  k }, {  0.0f,  1.0f,  0.0f } },
		{ { -k,  k, -k }, {  0.0f,  1.0f,  0.0f } },
		{ {  k,  k, -k }, {  0.0f,  1.0f,  0.0f } },

		{ { -k, -k,  k }, {  0.0f, -1.0f,  0.0f } },
		{ { -k, -k, -k }, {  0.0f, -1.0f,  0.0f } },
		{ {  k, -k,  k }, {  0.0f, -1.0f,  0.0f } },
		{ {  k, -k, -k }, {  0.0f, -1.0f,  0.0f } },
		};



		for (const auto& vertex : vertexList)
		{
			Vertex v;
			XMFLOAT2 uv = { 1,1 };
			if (vertex.Position.x < 0) uv.x = 0;
			if (vertex.Position.y < 0) uv.y = 0;
			v.Position = vertex.Position;
			v.Normal = vertex.Normal;
			v.Color = vertexColor;
			v.UV = uv;
			Vertices.emplace_back(v);
		}
		Indices = {
			 0,  1,  2,     3,  2,  1,
			 4,  5,  6,     7,  6,  5,
			 8,  9, 10,    11, 10,  9,
			12, 13, 14,    15, 14, 13,
			16, 17, 18,    19, 18, 17,
			20, 21, 22,    23, 22, 21,
		};
		DiffuseMap = diffuseMap;
	}
};

