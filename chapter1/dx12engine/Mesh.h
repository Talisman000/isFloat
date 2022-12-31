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

	TriangleMesh(XMFLOAT4 vertexColor = { 0,0,0,0 })
	{
		Vertices = std::vector<Vertex>(3);
		Vertices[0].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
		Vertices[1].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
		Vertices[2].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
		for (auto& vertex : Vertices) {
			vertex.Color = vertexColor;
		}

		Indices = { 0, 1, 2 }; // これに書かれている順序で描画する
	}
};

struct SquareMesh : Mesh {

	SquareMesh(XMFLOAT4 vertexColor = { 0,0,0,0 })
	{
		Vertices = std::vector<Vertex>(4);
		Vertices[0].Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
		Vertices[1].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
		Vertices[2].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
		Vertices[3].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
		for (auto& vertex : Vertices) {
			vertex.Color = vertexColor;
		}


		Indices = { 0, 1, 2, 0, 2, 3 }; // これに書かれている順序で描画する
	}
};

struct CubeMesh : Mesh
{
	CubeMesh(XMFLOAT4 vertexColor = { 0,0,0,1 })
	{
		const float k = 1.0f;

		XMFLOAT3 cubeVertices[] = {
			{-k, -k, -k},
			{-k, k, -k},
			{k, k, -k}, 
			{k, -k, -k},
			{k, -k, -k},
			{k, k, -k}, 
			{k, k, k}, 
			{k, -k, k}, 
			{-k, -k, k},
			{-k, k, k}, 
			{-k, k, -k},
			{-k, -k, -k},
			{k, -k, k}, 
			{k, k, k},
			{-k, k, k}, 
			{-k, -k, k},
			{-k, k, -k},
			{-k, k, k}, 
			{k, k, k},
			{k, k, -k}, 
			{-k, -k, k},
			{-k, -k, -k},
			{k, -k, -k},
			{k, -k, k}, 
		};

		for(auto vertex : cubeVertices)
		{
			Vertex v;
			v.Position = vertex;
			v.Color = vertexColor;
			Vertices.emplace_back(v);
		}
		Indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4,
			8, 9, 10, 10, 11, 8,
			12, 13, 14, 14, 15, 12,
			16, 17, 18, 18, 19, 16,
			20, 21, 22, 22, 23, 20,
		};
	}
};

