#pragma once
#include <vector>

#include "SharedStruct.h"
using namespace DirectX;
struct Mesh
{
	std::vector<Vertex> m_vertices; // 頂点データの配列
	std::vector<uint32_t> m_indices; // インデックスの配列
};

struct TriangleMesh : Mesh {

	TriangleMesh(XMFLOAT4 vertexColor = { 0,0,0,0 })
	{
		m_vertices = std::vector<Vertex>(3);
		m_vertices[0].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
		m_vertices[1].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
		m_vertices[2].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
		for (auto& vertex : m_vertices) {
			vertex.Color = vertexColor;
		}

		m_indices = { 0, 1, 2 }; // これに書かれている順序で描画する
	}
};

struct SquareMesh : Mesh {

	SquareMesh(XMFLOAT4 vertexColor = { 0,0,0,0 })
	{
		m_vertices = std::vector<Vertex>(4);
		m_vertices[0].Position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
		m_vertices[1].Position = XMFLOAT3(1.0f, 1.0f, 0.0f);
		m_vertices[2].Position = XMFLOAT3(1.0f, -1.0f, 0.0f);
		m_vertices[3].Position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
		for (auto& vertex : m_vertices) {
			vertex.Color = vertexColor;
		}


		m_indices = { 0, 1, 2, 0, 2, 3 }; // これに書かれている順序で描画する
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
			m_vertices.emplace_back(v);
		}
		m_indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4,
			8, 9, 10, 10, 11, 8,
			12, 13, 14, 14, 15, 12,
			16, 17, 18, 18, 19, 16,
			20, 21, 22, 22, 23, 20,
		};
	}
};

