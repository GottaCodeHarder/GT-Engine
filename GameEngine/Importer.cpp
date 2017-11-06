#include "Importer.h"

#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"
#include "cMesh.h"
#include "cTransform.h"
#include "cMaterial.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include <queue>

#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma	comment (lib, "Devil/libx86/ILU.lib")
#pragma	comment (lib, "Devil/libx86/ILUT.lib")

GameObject* Importer::LegacyLoadFbx(const char * path)
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
					mesh->vertex.reserve(scene->mMeshes[i]->mNumVertices);
					memcpy(mesh->vertex.data(), scene->mMeshes[i]->mVertices, sizeof(float3)*scene->mMeshes[i]->mNumVertices);
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

					if (mesh->vertex.empty() != false)
					{
						glGenBuffers(1, (GLuint*) &(mesh->buffVertex));
						glBindBuffer(GL_ARRAY_BUFFER, mesh->buffVertex);
						glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * scene->mMeshes[i]->mNumVertices, mesh->vertex.data(), GL_STATIC_DRAW);

						MYLOG("Importer - Loading %i vertex succesful!", scene->mMeshes[i]->mNumVertices);
					}
					else
					{
						MYLOG("WARNING, the scene has 0 vertex!");
					}
					if (mesh->vertex.size() == 0)
					{
						for (int j = 0; j < scene->mMeshes[i]->mNumVertices; j++)
						{
							mesh->vertex.push_back(float3(scene->mMeshes[i]->mVertices[j].x, scene->mMeshes[i]->mVertices[j].y, scene->mMeshes[i]->mVertices[j].z));

						}
					}

					if (scene->mMeshes[i]->HasNormals())
					{
						mesh->normals.reserve(scene->mMeshes[i]->mNumVertices);
						memcpy(mesh->normals.data(), scene->mMeshes[i]->mNormals, sizeof(float3)*scene->mMeshes[i]->mNumVertices);
						glGenBuffers(1, (GLuint*) &(mesh->buffNormals));
						glBindBuffer(GL_ARRAY_BUFFER, mesh->buffNormals);
						glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * scene->mMeshes[i]->mNumVertices, mesh->normals.data(), GL_STATIC_DRAW);
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

						glGenBuffers(1, (GLuint*) &(mesh->buffUv));
						glBindBuffer(GL_ARRAY_BUFFER, mesh->buffUv);
						glBufferData(GL_ARRAY_BUFFER, sizeof(float) * scene->mMeshes[i]->mNumVertices * 2, uv, GL_STATIC_DRAW);
					}

					mesh->index.reserve(scene->mMeshes[i]->mNumFaces * 3);


					uint* index = new uint[scene->mMeshes[i]->mNumFaces * 3];
					mesh->numIndex = scene->mMeshes[i]->mNumFaces * 3;
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
							mesh->index.push_back(*it);
							it++;
							mesh->index.push_back(*it);
							it++;
							mesh->index.push_back(*it);
						}
					}

					glGenBuffers(1, (GLuint*) &(mesh->buffIndex));
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buffIndex);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * scene->mMeshes[i]->mNumFaces * 3, index, GL_STATIC_DRAW);
					MYLOG("Importer - Loading %i index succesful!", (uint)scene->mMeshes[i]->mNumFaces * 3);

					mesh->aabbBox.SetNegativeInfinity();
					mesh->aabbBox.Enclose(mesh->vertex.data(), scene->mMeshes[i]->mNumVertices);
					mesh->numVertex = scene->mMeshes[i]->mNumVertices;
					meshesBoxes.push_back(mesh->aabbBox);

					float4x4 matrix = transform->GetGlobalMatrixTransf();
					if (mesh != nullptr)
					{
						gameObject->aabbBox.SetNegativeInfinity();
						OBB obb = mesh->aabbBox.Transform(matrix);

						gameObject->aabbBox.Enclose(obb);
						//SI TE COMPONENT CAMERA CAMBIAR FRUSTUM
					}

					if (scene->HasMaterials())
					{
						if (scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE) > 0)
						{
							aiString texturePath; // Path to the texture, from FBX directory
							scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texturePath);
							std::string texName = texturePath.C_Str();
							std::string strPath = path;
							material->path = texturePath.C_Str();

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
									material->buffTexture = LoadImageFile(strPath.c_str(),material);
								}
							}
							else
							{
								MYLOG("ERROR Image not loaded");
							}

							if (material->buffTexture == -1)
							{
								MYLOG("ERROR Creating buffer for Texture");
							}
						}
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
	App->fileSystem->Save("FileName", buffer, len);
	delete[] buffer;
	
	return ret; // tmp
}

GLuint Importer::LoadImageFile(const char* theFileName, cMaterial* material)
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

		material->imageDimensions = ImVec2(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT));
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

FileExtensions Importer::GetExtension(const char *path)
{
	char* ptr = &(char)path[strlen(path)];

	for (; *ptr != '.' && ptr != path; ptr--)
	{
		// You found a secret :D
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

	return FileExtensions::Unsupported;
}

bool Importer::FileExists(const std::string& name)
{
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}