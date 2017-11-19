#include "Importer.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"
#include "cMesh.h"
#include "cTransform.h"
#include "cMaterial.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include <queue>

#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma	comment (lib, "Devil/libx86/ILU.lib")
#pragma	comment (lib, "Devil/libx86/ILUT.lib")

GameObject* Importer::ImportFbx(const char * path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	GameObject* ret = new GameObject("fakeRoot", true, nullptr);
	bCalcRet = true;

	if (scene != nullptr && scene->HasMeshes())
	{
		std::queue<std::pair<aiNode*, GameObject*>> toLoad;
		toLoad.push(std::pair<aiNode*, GameObject*>(scene->mRootNode, nullptr));
		int numMeshes = 0;

		while (toLoad.empty() == false)
		{

			GameObject* parent = toLoad.front().second;
			aiNode* loading = toLoad.front().first;
			toLoad.pop();

			GameObject* me = nullptr;

			//
			if (loading->mNumMeshes > 0)
			{
				for (int j = 0; j < loading->mNumMeshes; j++)
				{
					int i = loading->mMeshes[j];
					// Use scene->mNumMeshes to iterate on scene->mMeshes array
					GameObject* gameObject = new GameObject(scene->mMeshes[i]->mName.data, true, parent);
					if (gameObject->name == "")
					{
						std::string number = std::to_string(numMeshes);
						gameObject->name.assign("default");
						gameObject->name += number;
						numMeshes++;
					}
					me = gameObject;

					if (bCalcRet)
					{
						ret = me;
						bCalcRet = false;
					}
					
					if (parent != nullptr)
					{
						parent->sons.push_back(gameObject);
					}

					cMesh* mesh = new cMesh(gameObject);
					cTransform* transform = new cTransform(gameObject);
					cMaterial* material = new cMaterial(gameObject);

					//LOAD MESH
					mesh->resource = (ResourceMesh*)App->resourceManager->LoadResourceMesh(scene, i , path + gameObject->name);

					float4x4 matrix = transform->GetGlobalMatrixTransf();
					gameObject->AddComponent(mesh);
					gameObject->AddComponent(transform);
					gameObject->AddComponent(material);

					aiVector3D vectorScale;
					aiQuaternion quaternionTransform;
					aiVector3D vectorPosition;

					loading->mTransformation.Decompose(vectorScale, quaternionTransform, vectorPosition);
					transform->scaleLocal = { vectorScale.x , vectorScale.y , vectorScale.z };
					transform->positionLocal = { vectorPosition.x , vectorPosition.y , vectorPosition.z };
					transform->rotationLocal = { quaternionTransform.x,quaternionTransform.y, quaternionTransform.z, quaternionTransform.w };

					//LOAD TEXTURE/MATERIAL
					material->resource = (ResourceTexture*)App->resourceManager->LoadResourceTexture(scene, i, path, gameObject->name + path , material);

					if (mesh != nullptr)
					{
						gameObject->aabbBox.SetNegativeInfinity();
						OBB obb = mesh->resource->aabbBox.Transform(matrix);
					
						gameObject->aabbBox.Enclose(obb);
					}

					if (gameObject->statiC)
					{
						//QUADTREE
						App->scene->quad.AddGameObject(me);
					}
				}
			}
			else
			{
				GameObject* gameObject = new GameObject(loading->mName.data, true, parent);
				me = gameObject;
				cTransform* transform = new cTransform(gameObject);
				aiVector3D vectorScale;
				aiQuaternion quaternionTransform;
				aiVector3D vectorPosition;
				gameObject->AddComponent(transform);
				loading->mTransformation.Decompose(vectorScale, quaternionTransform, vectorPosition);
				transform->scaleLocal = { vectorScale.x , vectorScale.y , vectorScale.z };
				transform->positionLocal = { vectorPosition.x , vectorPosition.y , vectorPosition.z };
				transform->rotationLocal = { quaternionTransform.x,quaternionTransform.y, quaternionTransform.z, quaternionTransform.w };

				if (bCalcRet)
				{
					ret = me;
					bCalcRet = false;
				}

				if (parent != nullptr)
				{
					parent->sons.push_back(gameObject);
				}
			}

			for (int i = 0; i < loading->mNumChildren; i++)
			{
				toLoad.push(std::pair<aiNode*,GameObject*>(loading->mChildren[i], me));
			}
		}

		maxBox.SetNegativeInfinity();
		std::vector<AABB>::iterator itBoxes = meshesBoxes.begin();
		for (; itBoxes != meshesBoxes.end(); itBoxes++)
		{
			maxBox.Enclose((*itBoxes));
		}

		aiReleaseImport(scene);
	}
	else
		MYLOG("Error loading scene %s", path);

	char* buffer;
	uint len = ret->Serialize(buffer);

	std::string filename = "Library/";
	std::string tmp = path;

	// If files are from Assets
	int pos = tmp.find("Assets");
	if (pos != std::string::npos)
	{
		tmp = tmp.substr(pos + 7); // 7 being "Assets/"
	}

	while (true)
	{
		pos = tmp.find("/");
		if (pos != std::string::npos)
		{
			tmp[pos] = '_';
		}
		else
			break;
	}

	std::string::iterator it = tmp.end() - 1;
	for (; *it != '.' && it != tmp.begin(); it--) {}
	tmp.erase(it, tmp.end());
	tmp += ".GTE";

	filename += tmp;
	
	App->fileSystem->Save(filename.c_str(), buffer, len);
	delete[] buffer;
	
	return ret; // tmp
}

ResourceMesh * Importer::LoadMesh(const aiScene* scene , int meshIndex)
{
	ResourceMesh* ret = new ResourceMesh();
	int i = meshIndex;
	ret->vertex.reserve(scene->mMeshes[i]->mNumVertices);
	memcpy(ret->vertex.data(), scene->mMeshes[i]->mVertices, sizeof(float3)*scene->mMeshes[i]->mNumVertices);

	if (ret->vertex.empty() != false)
	{
		glGenBuffers(1, (GLuint*) &(ret->buffVertex));
		glBindBuffer(GL_ARRAY_BUFFER, ret->buffVertex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * scene->mMeshes[i]->mNumVertices, ret->vertex.data(), GL_STATIC_DRAW);

		MYLOG("Importer - Loading %i vertex succesful!", scene->mMeshes[i]->mNumVertices);
	}
	else
	{
		MYLOG("WARNING, the scene has 0 vertex!");
	}
	if (ret->vertex.size() == 0)
	{
		for (int j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
		{
			ret->vertex.push_back(float3(scene->mMeshes[i]->mVertices[j].x, scene->mMeshes[i]->mVertices[j].y, scene->mMeshes[i]->mVertices[j].z));

		}
	}

	if (scene->mMeshes[i]->HasNormals())
	{
		ret->normals.reserve(scene->mMeshes[i]->mNumVertices);
		memcpy(ret->normals.data(), scene->mMeshes[i]->mNormals, sizeof(float3)*scene->mMeshes[i]->mNumVertices);
		glGenBuffers(1, (GLuint*) &(ret->buffNormals));
		glBindBuffer(GL_ARRAY_BUFFER, ret->buffNormals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * scene->mMeshes[i]->mNumVertices, ret->normals.data(), GL_STATIC_DRAW);
	}

	if (scene->mMeshes[i]->HasTextureCoords(0))
	{
		float* uv = new float[scene->mMeshes[i]->mNumVertices * 2]; //BE
																	//memcpy(uv, scene->mMeshes[i]->mTextureCoords, sizeof(float)*scene->mMeshes[i]->mNumVertices);
		for (int j = 0; j < scene->mMeshes[i]->mNumVertices; ++j)
		{
			uv[j * 2] = scene->mMeshes[i]->mTextureCoords[0][j].x;
			uv[j * 2 + 1] = scene->mMeshes[i]->mTextureCoords[0][j].y;
		}

		glGenBuffers(1, (GLuint*) &(ret->buffUv));
		glBindBuffer(GL_ARRAY_BUFFER, ret->buffUv);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * scene->mMeshes[i]->mNumVertices * 2, uv, GL_STATIC_DRAW);
	}

	ret->index.reserve(scene->mMeshes[i]->mNumFaces * 3);


	uint* index = new uint[scene->mMeshes[i]->mNumFaces * 3];
	ret->numIndex = scene->mMeshes[i]->mNumFaces * 3;
	for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
	{
		if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
		{
			MYLOG("WARNING, geometry face with != 3 indices!");
		}
		else
		{
			memcpy(&index[j * 3], scene->mMeshes[i]->mFaces[j].mIndices, sizeof(uint) * 3);
			unsigned int * it = scene->mMeshes[i]->mFaces[j].mIndices;
			ret->index.push_back(*it);
			it++;
			ret->index.push_back(*it);
			it++;
			ret->index.push_back(*it);
		}
	}

	glGenBuffers(1, (GLuint*) &(ret->buffIndex));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret->buffIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * scene->mMeshes[i]->mNumFaces * 3, index, GL_STATIC_DRAW);
	MYLOG("Importer - Loading %i index succesful!", (uint)scene->mMeshes[i]->mNumFaces * 3);

	ret->aabbBox.SetNegativeInfinity();
	ret->aabbBox.Enclose(ret->vertex.data(), scene->mMeshes[i]->mNumVertices);
	ret->numVertex = scene->mMeshes[i]->mNumVertices;
	meshesBoxes.push_back(ret->aabbBox);

	return ret;
}

ResourceTexture * Importer::LoadTexture(const aiScene* scene , int textIndex , const char* path , cMaterial* material)
{
	ResourceTexture* ret = new ResourceTexture();

	if (scene->HasMaterials())
	{
		if (scene->mMaterials[scene->mMeshes[textIndex]->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString texturePath; // Path to the texture, from FBX directory
			scene->mMaterials[scene->mMeshes[textIndex]->mMaterialIndex]->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texturePath);
			std::string texName = texturePath.C_Str();
			std::string strPath = path;
			ret->path = texturePath.C_Str();

			for (uint i = strlen(path); i >= 0; i--)
			{
				if (path[i] == '\\')
				{
					strPath.resize(i + 1);
					strPath += texName;
					break;
				}
			}
			if (strPath.c_str() != NULL)
			{
				if (FileExists(strPath.c_str()))
				{
					ret->buffTexture = LoadImageFile(strPath.c_str());
				}
				else
				{
					MYLOG("ERROR Image not loaded");
				}
			}


			if (ret->buffTexture == -1)
			{
				MYLOG("ERROR Creating buffer for Texture");
			}
			material->resource->imageDimensions = ImVec2(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
		}
	}
	return ret;
}

GameObject * Importer::ImportGTE(const char * path)
{
	GameObject* ret;
	std::string tmp = path;
	
	// Check that it isn't in Assets
	int pos = tmp.find("Assets");
	if (pos != std::string::npos)
	{
		tmp = tmp.substr(pos + 7); // 7 being "Assets/"
	}

	while (true)
	{
		pos = tmp.find("/");
		if (pos != std::string::npos)
		{
			tmp[pos] = '_';
		}
		else
			break;
	}

	std::string filename = tmp;

	SDL_RWops* rwops = App->fileSystem->Load(filename.c_str());
	
	if (rwops != nullptr)
	{
		uint size = rwops->size(rwops);

		if (size >= NULL)
		{
			char* content = new char[size];
			rwops->read(rwops, content, size, 1);


			ret = new GameObject("", true, App->scene->root);

			ret->DeSerialize(content, App->scene->root);
		}
		rwops->close(rwops);
	}

	return ret;
}

GLuint Importer::LoadImageFile(const char* theFileName)
{
	if (!bDevilInit)
	{
		ilInit();

		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			MYLOG("GLEW initialisation error: %s", glewGetErrorString(err));
			exit(-1);
		}
		MYLOG("GLEW intialised successfully. Current GLEW version: %s", glewGetString(GLEW_VERSION));

		bDevilInit = true;
	}

	ILuint imageID = 0;
	GLuint textureID;

	// Safe
	ILboolean success;
	ILenum error;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	success = ilLoadImage(theFileName);
	if (success)
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		if (!success)
		{
			error = ilGetError();
			MYLOG("Image conversion failed! IL error: %i - %s", error, iluErrorString(error));
			return -1;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

		//material->resource->imageDimensions = ImVec2(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
	}
	else
	{
		error = ilGetError();
		MYLOG("Image loading failed! IL error: %i - %s", error, iluErrorString(error));
		return -1;
	}

	//ilDeleteImages(1, &imageID);

	MYLOG("Texture for mesh applied: %s", theFileName);

	return textureID;
}

GLuint Importer::LoadImageFile(const char * theFileName, cMaterial * material)
{
	if (!bDevilInit)
	{
		ilInit();

		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			MYLOG("GLEW initialisation error: %s", glewGetErrorString(err));
			exit(-1);
		}
		MYLOG("GLEW intialised successfully. Current GLEW version: %s", glewGetString(GLEW_VERSION));

		bDevilInit = true;
	}

	ILuint imageID = 0;
	GLuint textureID;

	// Safe
	ILboolean success;
	ILenum error;

	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	success = ilLoadImage(theFileName);
	if (success)
	{
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		if (!success)
		{
			error = ilGetError();
			MYLOG("Image conversion failed! IL error: %i - %s", error, iluErrorString(error));
			return -1;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, ilGetData());

		material->resource->imageDimensions = ImVec2(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
	}
	else
	{
		error = ilGetError();
		MYLOG("Image loading failed! IL error: %i - %s", error, iluErrorString(error));
		return -1;
	}

	//ilDeleteImages(1, &imageID);

	MYLOG("Texture for mesh applied: %s", theFileName);

	return textureID;
}

void Importer::FindAndReplace(std::string & source, std::string const & toFind, std::string const & replace)
{
	for (std::string::size_type i = 0; (i = source.find(toFind, i)) != std::string::npos;)
	{
		source.replace(i, toFind.length(), replace);
		i += replace.length();
	}
}
FileExtensions Importer::GetExtension(const char *path)
{
	char* ptr = &(char)path[strlen(path)];

	for (; *ptr != '.'; ptr--)
	{
		// You found a secret :D

		if (ptr == path)
		{
			return FileExtensions::Folder;
		}
	}
	
	ptr++;

	std::string supportedImageFormats("bmp dcx dds hdr icns ico cur iff gif jpg jpe jpeg jp2 lbm png PNG raw tif tga");

	if (supportedImageFormats.find(ptr) != std::string::npos)
	{
		return FileExtensions::Image;
	}

	std::string supported3DScenesFormats("FBX fbx OBJ obj");

	if (supported3DScenesFormats.find(ptr) != std::string::npos)
	{
		return FileExtensions::Scene3D;
	}

	std::string supportedGTSceneFormats("GTScene");

	if (supportedGTSceneFormats.find(ptr) != std::string::npos)
	{
		return FileExtensions::GTScene;
	}

	std::string supportedGTImportedFormats("GTE");

	if (supportedGTImportedFormats.find(ptr) != std::string::npos)
	{
		return FileExtensions::GTImported;
	}

	return FileExtensions::Unsupported;
}

bool Importer::FileExists(const std::string& name) //Move to ModuleFileSystem
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}