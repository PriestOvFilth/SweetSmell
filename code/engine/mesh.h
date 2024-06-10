#pragma once

#include <map>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "common/common.h"
#include "common/math3d.h"

#define INDEX_BUFFER 0    
#define POS_VB       1
#define NORMAL_VB    2
#define TEXCOORD_VB  3
#define BONE_VB		 4

#define NUM_BONES_PER_VERTEX 4

class Texture;

class Mesh
{
public:

	virtual bool LoadModel(const char* FileName) { return false; }
	virtual bool InitFromScene(const aiScene* Scene, const char* FileName) { return false; }
	virtual bool InitMaterials(const aiScene* Scene, const char* FileName);
	virtual void Clear() {}
	virtual void DrawMesh();

	struct SubMesh
	{
		SubMesh()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MatIndex = INVALID;
		}

		uint32 NumIndices;
		uint32 BaseVertex;
		uint32 BaseIndex;
		uint32 MatIndex;
	};

	std::vector<SubMesh> SubMeshes;
	std::vector<Texture*> Textures;
	GLuint VAO;

	AABB BoundingBox;

	std::string Filename;
	bool bIsLoaded;

	const aiScene* AssimpScene;
	Assimp::Importer Importer;
};

class StaticMesh : public Mesh
{
public:

	StaticMesh() { }

	~StaticMesh()
	{
		Clear();
	}

	virtual bool LoadModel(const char* FileName) override;

	virtual bool InitFromScene(const aiScene* Scene, const char* FileName) override;

	void InitModel(const aiMesh* Mesh, std::vector<Vector3>& Positions, std::vector<Vector3>& Normals, std::vector<Vector2>& TexCoords, std::vector<uint32>& Indices);

	virtual void Clear() override;

	GLuint Buffers[4];
};

class SkeletalMesh : public Mesh
{
public:

	SkeletalMesh();
	~SkeletalMesh();

	struct BoneInfo
	{
		Matrix4 BoneOffset;
		Matrix4 FinalTransformation;

		BoneInfo()
		{
			BoneOffset.SetZero();
			FinalTransformation.SetZero();
		}
	};

	struct VertexBoneData
	{
		uint32 IDs[NUM_BONES_PER_VERTEX];
		float Weights[NUM_BONES_PER_VERTEX];

		VertexBoneData()
		{
			Reset();
		}

		void Reset()
		{
			ZERO_MEM(IDs);
			ZERO_MEM(Weights);
		}

		void AddBoneData(uint32 BoneID, float Weight);
	};

	virtual bool LoadModel(const char* FileName) override;
	virtual bool InitFromScene(const aiScene* Scene, const char* FileName) override;
	void InitModel(uint32 MeshIndex, const aiMesh* Mesh, std::vector<Vector3>& Positions, std::vector<Vector3>& Normals, std::vector<Vector2>& TexCoords, std::vector<uint32>& Indices, std::vector<VertexBoneData>& Bones);
	virtual void Clear() override;

	void BoneTransform(float TimeInSeconds, std::vector<Matrix4>& Transforms);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* NodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* NodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* NodeAnim);
	uint32 FindScaling(float AnimationTime, const aiNodeAnim* NodeAnim);
	uint32 FindRotation(float AnimationTime, const aiNodeAnim* NodeAnim);
	uint32 FindPosition(float AnimationTime, const aiNodeAnim* NodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* Animation, const std::string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* Node, const Matrix4& ParentTransform);
	void LoadBones(uint32 MeshIndex, const aiMesh* Mesh, std::vector<VertexBoneData>& Bones);

	GLuint Buffers[5];

	std::map<std::string, uint32> BoneMapping;
	uint32 NumBones;
	std::vector<BoneInfo> BoneInfos;
	Matrix4 GlobalInverseTransform;
};