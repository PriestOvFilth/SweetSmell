#include "mesh.h"
#include "resource.h"
#include "texture.h"

using namespace std;

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

bool Mesh::InitMaterials(const aiScene* Scene, const char* FileName)
{
	bool bRet = false;

	std::string FilePath = FileName;
	std::string::size_type SlashIndex = FilePath.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos)
	{
		Dir = ".";
	}
	else if (SlashIndex == 0)
	{
		Dir = "/";
	}
	else
	{
		Dir = FilePath.substr(0, SlashIndex);
	}

	for (uint32 i = 0; i < Scene->mNumMaterials; i++)
	{
		const aiMaterial* Mat = Scene->mMaterials[i];
		Textures[i] = NULL;

		if (Mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString Path;

			if (Mat->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string FullPath = Dir + "/" + Path.data;
				Textures[i] = GResourceMan->GetTexture(FullPath.c_str());

				if (!Textures[i] || !Textures[i]->bIsLoaded)
				{
					WriteLog("Error loading texture %s\n", FullPath.c_str());
					bRet = false;
				}
				else
				{
					bRet = true;
				}
			}
		}

		if (!Textures[i])
		{
			Textures[i] = GResourceMan->GetTexture("data/textures/test.png");
			bRet = (Textures[i]->bIsLoaded);
		}
	}

	return bRet;
}

void Mesh::DrawMesh()
{
	glBindVertexArray(VAO);

	for (uint32 i = 0; i < SubMeshes.size(); i++)
	{
		const uint32 MatIdx = SubMeshes[i].MatIndex;

		FILTH_ASSERT(MatIdx < Textures.size());

		if (Textures[MatIdx])
		{
			Textures[MatIdx]->Bind(GL_TEXTURE0);
		}

		glDrawElementsBaseVertex(GL_TRIANGLES, SubMeshes[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(uint32) * SubMeshes[i].BaseIndex), SubMeshes[i].BaseVertex);
	}

	glBindVertexArray(0);
}

bool StaticMesh::LoadModel(const char* FileName)
{
	Filename = FileName;
	Clear();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(Buffers), Buffers);

	bIsLoaded = false;

	AssimpScene = Importer.ReadFile(FileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	if (AssimpScene)
	{
		bIsLoaded = InitFromScene(AssimpScene, FileName);
	}
	else
	{
		WriteLog("Error loading model '%s': %s\n", FileName, Importer.GetErrorString());
	}

	glBindVertexArray(0);

	return bIsLoaded;
}

bool StaticMesh::InitFromScene(const aiScene* Scene, const char* FileName)
{
	SubMeshes.resize(Scene->mNumMeshes);
	Textures.resize(Scene->mNumMaterials);

	std::vector<Vector3> Positions;
	std::vector<Vector3> Normals;
	std::vector<Vector2> TexCoords;
	std::vector<uint32> Indices;

	uint32 NumVertices = 0;
	uint32 NumIndices = 0;

	for (uint32 i = 0; i < SubMeshes.size(); i++)
	{
		SubMeshes[i].MatIndex = Scene->mMeshes[i]->mMaterialIndex;
		SubMeshes[i].NumIndices = Scene->mMeshes[i]->mNumFaces * 3;
		SubMeshes[i].BaseVertex = NumVertices;
		SubMeshes[i].BaseIndex = NumIndices;

		NumVertices += Scene->mMeshes[i]->mNumVertices;
		NumIndices += SubMeshes[i].NumIndices;
	}

	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Indices.reserve(NumIndices);

	for (uint32 i = 0; i < SubMeshes.size(); i++)
	{
		const aiMesh* Mesh = Scene->mMeshes[i];
		InitModel(Mesh, Positions, Normals, TexCoords, Indices);
	}

	BoundingBox.ComputeFromVertices(Positions);

	if (!InitMaterials(Scene, FileName))
	{
		return false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	return GLCheckError();
}

void StaticMesh::InitModel(const aiMesh* Mesh, std::vector<Vector3>& Positions, std::vector<Vector3>& Normals, std::vector<Vector2>& TexCoords, std::vector<uint32>& Indices)
{
	const aiVector3D Zero(0.0f, 0.0f, 0.0f);

	for (uint32 i = 0; i < Mesh->mNumVertices; i++)
	{
		const aiVector3D* Pos = &Mesh->mVertices[i];
		const aiVector3D* Normal = &Mesh->mNormals[i];
		const aiVector3D* TexCoord = Mesh->HasTextureCoords(0) ? &Mesh->mTextureCoords[0][i] : &Zero;

		Positions.push_back(Vector3(Pos->x, Pos->y, Pos->z));
		Normals.push_back(Vector3(Normal->x, Normal->y, Normal->z));
		TexCoords.push_back(Vector2(TexCoord->x, TexCoord->y));
	}

	for (uint32 i = 0; i < Mesh->mNumFaces; i++)
	{
		const aiFace& Face = Mesh->mFaces[i];
		FILTH_ASSERT(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}
}

void StaticMesh::Clear()
{
	SubMeshes.clear();
	SubMeshes.resize(0);

	Importer.FreeScene();

	if (Buffers[0] != 0)
	{
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(Buffers), Buffers);
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
}

void SkeletalMesh::VertexBoneData::AddBoneData(uint32 BoneID, float Weight)
{
	for (uint32 i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++)
	{
		if (Weights[i] == 0.0)
		{
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	FILTH_ASSERT(0);
}

SkeletalMesh::SkeletalMesh()
{
	VAO = 0;
	ZERO_MEM(Buffers);
	NumBones = 0;
	AssimpScene = NULL;
}


SkeletalMesh::~SkeletalMesh()
{
	Clear();
}


void SkeletalMesh::Clear()
{
	SubMeshes.clear();
	SubMeshes.resize(0);

	Importer.FreeScene();

	if (Buffers[0] != 0)
	{
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(Buffers), Buffers);
	}

	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
}


bool SkeletalMesh::LoadModel(const char* FileName)
{
	Filename = FileName;
	Clear();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(Buffers), Buffers);

	bIsLoaded = false;

	AssimpScene = Importer.ReadFile(FileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (AssimpScene)
	{
		GlobalInverseTransform = AssimpScene->mRootNode->mTransformation;
		GlobalInverseTransform.Inverse();
		bIsLoaded = InitFromScene(AssimpScene, FileName);
	}
	else
	{
		WriteLog("Error parsing '%s': '%s'\n", FileName, Importer.GetErrorString());
	}

	glBindVertexArray(0);

	return bIsLoaded;
}


bool SkeletalMesh::InitFromScene(const aiScene* Scene, const char* FileName)
{
	SubMeshes.resize(AssimpScene->mNumMeshes);
	Textures.resize(AssimpScene->mNumMaterials);

	vector<Vector3> Positions;
	vector<Vector3> Normals;
	vector<Vector2> TexCoords;
	vector<VertexBoneData> Bones;
	vector<uint32> Indices;

	uint32 NumVertices = 0;
	uint32 NumIndices = 0;

	for (uint32 i = 0; i < SubMeshes.size(); i++)
	{
		SubMeshes[i].MatIndex = AssimpScene->mMeshes[i]->mMaterialIndex;
		SubMeshes[i].NumIndices = AssimpScene->mMeshes[i]->mNumFaces * 3;
		SubMeshes[i].BaseVertex = NumVertices;
		SubMeshes[i].BaseIndex = NumIndices;

		NumVertices += AssimpScene->mMeshes[i]->mNumVertices;
		NumIndices += SubMeshes[i].NumIndices;
	}

	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Bones.resize(NumVertices);
	Indices.reserve(NumIndices);

	for (uint32 i = 0; i < SubMeshes.size(); i++)
	{
		const aiMesh* Mesh = AssimpScene->mMeshes[i];
		InitModel(i, Mesh, Positions, Normals, TexCoords, Indices, Bones);
	}

	// FIXME: this generates really weird bounding boxes for skeletal meshes
	BoundingBox.ComputeFromVertices(Positions);

	if (!InitMaterials(AssimpScene, FileName))
	{
		return false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	return GLCheckError();
}


void SkeletalMesh::InitModel(uint32 MeshIndex,
	const aiMesh* Mesh,
	vector<Vector3>& Positions,
	vector<Vector3>& Normals,
	vector<Vector2>& TexCoords,
	vector<uint32>& Indices,
	vector<VertexBoneData>& Bones)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for (uint32 i = 0; i < Mesh->mNumVertices; i++)
	{
		const aiVector3D* pPos = &Mesh->mVertices[i];
		const aiVector3D* pNormal = &Mesh->mNormals[i];
		const aiVector3D* pTexCoord = Mesh->HasTextureCoords(0) ? &Mesh->mTextureCoords[0][i] : &Zero3D;

		Positions.push_back(Vector3(pPos->x, pPos->y, pPos->z));
		Normals.push_back(Vector3(pNormal->x, pNormal->y, pNormal->z));
		TexCoords.push_back(Vector2(pTexCoord->x, pTexCoord->y));
	}

	LoadBones(MeshIndex, Mesh, Bones);

	for (uint32 i = 0; i < Mesh->mNumFaces; i++)
	{
		const aiFace& Face = Mesh->mFaces[i];
		FILTH_ASSERT(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}
}


void SkeletalMesh::LoadBones(uint32 MeshIndex, const aiMesh* Mesh, vector<VertexBoneData>& Bones)
{
	for (uint32 i = 0; i < Mesh->mNumBones; i++)
	{
		uint32 BoneIndex = 0;
		string BoneName(Mesh->mBones[i]->mName.data);

		if (BoneMapping.find(BoneName) == BoneMapping.end())
		{
			BoneIndex = NumBones;
			NumBones++;
			BoneInfo bi;
			BoneInfos.push_back(bi);
			BoneInfos[BoneIndex].BoneOffset = Mesh->mBones[i]->mOffsetMatrix;
			BoneMapping[BoneName] = BoneIndex;
		}
		else
		{
			BoneIndex = BoneMapping[BoneName];
		}

		for (uint32 j = 0; j < Mesh->mBones[i]->mNumWeights; j++)
		{
			uint32 VertexID = SubMeshes[MeshIndex].BaseVertex + Mesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = Mesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}

uint32 SkeletalMesh::FindPosition(float AnimationTime, const aiNodeAnim* NodeAnim)
{
	for (uint32 i = 0; i < NodeAnim->mNumPositionKeys - 1; i++)
	{
		if (AnimationTime < (float)NodeAnim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}

	FILTH_ASSERT(0);

	return 0;
}


uint32 SkeletalMesh::FindRotation(float AnimationTime, const aiNodeAnim* NodeAnim)
{
	FILTH_ASSERT(NodeAnim->mNumRotationKeys > 0);

	for (uint32 i = 0; i < NodeAnim->mNumRotationKeys - 1; i++)
	{
		if (AnimationTime < (float)NodeAnim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}

	FILTH_ASSERT(0);

	return 0;
}


uint32 SkeletalMesh::FindScaling(float AnimationTime, const aiNodeAnim* NodeAnim)
{
	FILTH_ASSERT(NodeAnim->mNumScalingKeys > 0);

	for (uint32 i = 0; i < NodeAnim->mNumScalingKeys - 1; i++)
	{
		if (AnimationTime < (float)NodeAnim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}

	FILTH_ASSERT(0);

	return 0;
}


void SkeletalMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* NodeAnim)
{
	if (NodeAnim->mNumPositionKeys == 1)
	{
		Out = NodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint32 PositionIndex = FindPosition(AnimationTime, NodeAnim);
	uint32 NextPositionIndex = (PositionIndex + 1);
	FILTH_ASSERT(NextPositionIndex < NodeAnim->mNumPositionKeys);

	float DeltaTime = (float)(NodeAnim->mPositionKeys[NextPositionIndex].mTime - NodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)NodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	FILTH_ASSERT(Factor >= 0.0f && Factor <= 1.0f);

	const aiVector3D& Start = NodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = NodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void SkeletalMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* NodeAnim)
{
	if (NodeAnim->mNumRotationKeys == 1)
	{
		Out = NodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint32 RotationIndex = FindRotation(AnimationTime, NodeAnim);
	uint32 NextRotationIndex = (RotationIndex + 1);
	FILTH_ASSERT(NextRotationIndex < NodeAnim->mNumRotationKeys);

	float DeltaTime = (float)(NodeAnim->mRotationKeys[NextRotationIndex].mTime - NodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)NodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	FILTH_ASSERT(Factor >= 0.0f && Factor <= 1.0f);

	const aiQuaternion& Start = NodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& End = NodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, Start, End, Factor);
	Out = Out.Normalize();
}


void SkeletalMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* NodeAnim)
{
	if (NodeAnim->mNumScalingKeys == 1)
	{
		Out = NodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint32 ScalingIndex = FindScaling(AnimationTime, NodeAnim);
	uint32 NextScalingIndex = (ScalingIndex + 1);
	FILTH_ASSERT(NextScalingIndex < NodeAnim->mNumScalingKeys);

	float DeltaTime = (float)(NodeAnim->mScalingKeys[NextScalingIndex].mTime - NodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)NodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	FILTH_ASSERT(Factor >= 0.0f && Factor <= 1.0f);

	const aiVector3D& Start = NodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = NodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D DeltaVector = End - Start;
	Out = Start + Factor * DeltaVector;
}


void SkeletalMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* Node, const Matrix4& ParentTransform)
{
	string NodeName = Node->mName.data;

	const aiAnimation* Animation = AssimpScene->mAnimations[0];

	Matrix4 NodeTransformation(Node->mTransformation);

	const aiNodeAnim* NodeAnim = FindNodeAnim(Animation, NodeName);

	if (NodeAnim)
	{
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, NodeAnim);
		Matrix4 ScalingM;
		ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, NodeAnim);
		Matrix4 RotationM = Matrix4(RotationQ.GetMatrix());

		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, NodeAnim);
		Matrix4 TranslationM;
		TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	Matrix4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (BoneMapping.find(NodeName) != BoneMapping.end())
	{
		uint32 BoneIndex = BoneMapping[NodeName];
		BoneInfos[BoneIndex].FinalTransformation = GlobalInverseTransform * GlobalTransformation * BoneInfos[BoneIndex].BoneOffset;
	}

	for (uint32 i = 0; i < Node->mNumChildren; i++)
	{
		ReadNodeHeirarchy(AnimationTime, Node->mChildren[i], GlobalTransformation);
	}
}


void SkeletalMesh::BoneTransform(float TimeInSeconds, vector<Matrix4>& Transforms)
{
	Matrix4 Identity;
	Identity.InitIdentity();

	float TicksPerSecond = (float)(AssimpScene->mAnimations[0]->mTicksPerSecond != 0 ? AssimpScene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)AssimpScene->mAnimations[0]->mDuration);

	ReadNodeHeirarchy(AnimationTime, AssimpScene->mRootNode, Identity);

	Transforms.resize(NumBones);

	for (uint32 i = 0; i < NumBones; i++)
	{
		Transforms[i] = BoneInfos[i].FinalTransformation;
	}
}


const aiNodeAnim* SkeletalMesh::FindNodeAnim(const aiAnimation* Animation, const string NodeName)
{
	for (uint32 i = 0; i < Animation->mNumChannels; i++)
	{
		const aiNodeAnim* NodeAnim = Animation->mChannels[i];

		if (string(NodeAnim->mNodeName.data) == NodeName)
		{
			return NodeAnim;
		}
	}

	return NULL;
}