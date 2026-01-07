#include "Resource/ResourceSerializer.h"

#include "Audio/SoundBuffer.h"
#include "Graphics/Animation.h"
#include "Graphics/Font.h"
#include "Graphics/Texture.h"
#include "IO/FileSystem.h"
#include "Project/Project.h"
#include "Resource/ResourceManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneSerializer.h"
#include "Scene/Prefab.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

namespace Luden
{

	//////////////////////////////////////////////////////////////////////////////////
	// NativeScriptSerializer
	//////////////////////////////////////////////////////////////////////////////////

	void NativeScriptResourceSerializer::Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const
	{
		auto script = std::static_pointer_cast<NativeScript>(resource);

		nlohmann::json j;

		j["ClassName"] = script->GetClassName();
		j["HeaderPath"] = script->GetHeaderPath().string();
		j["SourcePath"] = script->GetSourcePath().string();
		j["Handle"] = static_cast<uint64_t>(script->Handle);

		std::ofstream out(Project::GetEditorResourceManager()->GetFileSystemPath(metadata));
		out << j.dump(4);
	}

	bool NativeScriptResourceSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const
	{
		auto path = Project::GetEditorResourceManager()->GetFileSystemPath(metadata);
		std::ifstream in(path);
		if (!in.is_open())
			return false;

		nlohmann::json j;
		in >> j;

		auto script = std::make_shared<NativeScript>();
		script->SetClassName(j["ClassName"].get<std::string>());
		script->SetHeaderPath(j["HeaderPath"].get<std::string>());
		script->SetSourcePath(j["SourcePath"].get<std::string>());
		script->Handle = j["Handle"].get<uint64_t>();
		resource = script;
		return true;
	}

	bool NativeScriptResourceSerializer::SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const
	{
		outInfo.Offset = stream.GetStreamPosition();

		auto script = ResourceManager::GetResource<NativeScript>(handle);

		nlohmann::json j;
		j["ClassName"] = script->GetClassName();
		j["Handle"] = static_cast<uint64_t>(script->Handle);

		std::string jsonStr = j.dump();
		stream.WriteString(jsonStr);

		outInfo.Size = stream.GetStreamPosition() - outInfo.Offset;
		return true;
	}

	std::shared_ptr<Resource> NativeScriptResourceSerializer::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const
	{
		return std::shared_ptr<Resource>();
	}

	//////////////////////////////////////////////////////////////////////////////////
	// SpriteSerializer
	//////////////////////////////////////////////////////////////////////////////////
	void SpriteSerializer::Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const
	{
		auto sprite = std::static_pointer_cast<Sprite>(resource);

		nlohmann::json j;
		j["Name"] = sprite->GetName();
		j["TextureHandle"] = static_cast<uint64_t>(sprite->GetTextureHandle());

		const auto& rect = sprite->GetTextureRect();
		j["TextureRect"] = {
			{"x", rect.position.x},
			{"y", rect.position.y},
			{"width", rect.size.x},
			{"height", rect.size.y}
		};

		j["Pivot"] = {
			{"x", sprite->GetPivot().x},
			{"y", sprite->GetPivot().y}
		};

		j["Handle"] = static_cast<uint64_t>(sprite->Handle);

		std::ofstream out(Project::GetEditorResourceManager()->GetFileSystemPath(metadata));
		if (out.is_open())
		{
			out << j.dump(4);
		}
	}

	bool SpriteSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const
	{
		auto path = Project::GetEditorResourceManager()->GetFileSystemPath(metadata);

		std::ifstream in(path);
		if (!in.is_open())
			return false;

		nlohmann::json j;
		in >> j;

		auto sprite = std::make_shared<Sprite>();

		if (j.contains("Name"))
			sprite->SetName(j["Name"].get<std::string>());

		if (j.contains("TextureHandle"))
			sprite->SetTextureHandle(j["TextureHandle"].get<uint64_t>());

		if (j.contains("TextureRect"))
		{
			const auto& jRect = j["TextureRect"];
			sf::IntRect rect(
				{ jRect["x"].get<int>(), jRect["y"].get<int>() },
				{ jRect["width"].get<int>(), jRect["height"].get<int>() }
			);
			sprite->SetTextureRect(rect);
		}

		if (j.contains("Pivot"))
		{
			glm::vec2 pivot(
				j["Pivot"]["x"].get<float>(),
				j["Pivot"]["y"].get<float>()
			);
			sprite->SetPivot(pivot);
		}

		sprite->Handle = j["Handle"].get<uint64_t>();

		resource = sprite;
		return true;
	}

	bool SpriteSerializer::SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const
	{
		outInfo.Offset = stream.GetStreamPosition();

		auto sprite = ResourceManager::GetResource<Sprite>(handle);
		if (!sprite)
			return false;

		nlohmann::json j;

		j["Name"] = sprite->GetName();
		j["TextureHandle"] = static_cast<uint64_t>(sprite->GetTextureHandle());

		const auto& rect = sprite->GetTextureRect();
		j["TextureRect"] = {
			{"x", rect.position.x},
			{"y", rect.position.y},
			{"width", rect.size.x},
			{"height", rect.size.y}
		};

		j["Pivot"] = {
			{"x", sprite->GetPivot().x},
			{"y", sprite->GetPivot().y}
		};

		j["Handle"] = static_cast<uint64_t>(sprite->Handle);

		std::string jsonStr = j.dump();
		stream.WriteString(jsonStr);

		outInfo.Size = stream.GetStreamPosition() - outInfo.Offset;
		return true;
	}

	std::shared_ptr<Resource> SpriteSerializer::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const
	{
		stream.SetStreamPosition(resourceInfo.PackedOffset);

		std::string jsonStr;
		stream.ReadString(jsonStr);

		nlohmann::json j = nlohmann::json::parse(jsonStr);

		auto sprite = std::make_shared<Sprite>();

		if (j.contains("Name"))
			sprite->SetName(j["Name"].get<std::string>());

		if (j.contains("TextureHandle"))
			sprite->SetTextureHandle(j["TextureHandle"].get<uint64_t>());

		if (j.contains("TextureRect"))
		{
			const auto& jRect = j["TextureRect"];
			sf::IntRect rect(
				{ jRect["x"].get<int>(), jRect["y"].get<int>() },
				{ jRect["width"].get<int>(), jRect["height"].get<int>() }
			);
			sprite->SetTextureRect(rect);
		}

		if (j.contains("Pivot"))
		{
			glm::vec2 pivot(
				j["Pivot"]["x"].get<float>(),
				j["Pivot"]["y"].get<float>()
			);
			sprite->SetPivot(pivot);
		}

		return sprite;
	}

	//////////////////////////////////////////////////////////////////////////////////
	// TextureSerializer
	//////////////////////////////////////////////////////////////////////////////////

	bool TextureSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const
	{
		auto path = Project::GetEditorResourceManager()->GetFileSystemPath(metadata);
		resource = std::make_shared<Texture>();

		auto texture = std::static_pointer_cast<Texture>(resource);

		if (!texture->GetTexture().loadFromFile(path))
		{
			resource->SetFlag(ResourceFlag::Invalid);
			return false;
		}

		return true;
	}

	bool TextureSerializer::SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const
	{
		outInfo.Offset = stream.GetStreamPosition();

		auto path = Project::GetEditorResourceManager()->GetFileSystemPath(handle);
		Buffer textureData = FileSystem::ReadBytes(path);
		stream.WriteBuffer(textureData);

		outInfo.Size = stream.GetStreamPosition() - outInfo.Offset;
		return true;
	}

	std::shared_ptr<Resource> TextureSerializer::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const
	{
		stream.SetStreamPosition(resourceInfo.PackedOffset);

		Buffer textureData;
		stream.ReadBuffer(textureData);

		auto texture = std::make_shared<Texture>();
		sf::Texture sfTexture;
		if (!sfTexture.loadFromMemory(textureData.Data, textureData.GetSize()))
			return nullptr;

		texture->SetTexture(sfTexture);
		return texture;
	}

	//////////////////////////////////////////////////////////////////////////////////
	// FontSerializer
	//////////////////////////////////////////////////////////////////////////////////

	bool FontSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const
	{
		auto fontResource = std::static_pointer_cast<Font>(resource);
		sf::Font font;
		if (!font.openFromFile(metadata.FilePath))
			return false;

		fontResource->SetFont(font);
		return true;
	}

	bool FontSerializer::SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const
	{
		outInfo.Offset = stream.GetStreamPosition();

		std::shared_ptr<Font> font = ResourceManager::GetResource<Font>(handle);
		auto path = Project::GetEditorResourceManager()->GetFileSystemPath(handle);
		Buffer fontData = FileSystem::ReadBytes(path);
		stream.WriteBuffer(fontData);

		outInfo.Size = stream.GetStreamPosition() - outInfo.Offset;
		return true;
	}

	std::shared_ptr<Resource> FontSerializer::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const
	{
		stream.SetStreamPosition(resourceInfo.PackedOffset);

		Buffer fontData;
		stream.ReadBuffer(fontData);

		auto font = std::make_shared <Font>();
		sf::Font sfFont;
		if (!sfFont.openFromMemory(fontData.Data, fontData.GetSize()))
			return nullptr;

		font->SetFont(sfFont);
		return font;
	}

//////////////////////////////////////////////////////////////////////////////////
// AudioFileSourceSerializer
//////////////////////////////////////////////////////////////////////////////////

	void AudioFileSourceSerializer::Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const
	{

	}

	bool AudioFileSourceSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const
	{
		auto soundBufferResource = std::static_pointer_cast<SoundBuffer>(resource);
		sf::SoundBuffer sfSoundBuffer;
		if (!sfSoundBuffer.loadFromFile(metadata.FilePath))
			return false;

		soundBufferResource->SetBuffer(sfSoundBuffer);
		soundBufferResource->SetFilePath(metadata.FilePath);
		return true;
	}

	bool AudioFileSourceSerializer::SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const
	{
		outInfo.Offset = stream.GetStreamPosition();

		std::shared_ptr<SoundBuffer> sounfBuffer = ResourceManager::GetResource<SoundBuffer>(handle);
		auto path = Project::GetEditorResourceManager()->GetFileSystemPath(handle);
		auto relativePath = std::filesystem::relative(path, Project::GetActiveResourceDirectory());

		std::string filePath;
		if (relativePath.empty())
			filePath = path.string();
		else
			filePath = relativePath.string();

		stream.WriteString(filePath);

		outInfo.Size = stream.GetStreamPosition() - outInfo.Offset;

		return true;
	}

	std::shared_ptr<Resource> AudioFileSourceSerializer::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const
	{
		stream.SetStreamPosition(resourceInfo.PackedOffset);
		std::shared_ptr<SoundBuffer> audioFile = std::shared_ptr<SoundBuffer>();

		std::string pathString= audioFile->GetFilePath().string();
		stream.ReadString(pathString);

		return audioFile;
	}


	//////////////////////////////////////////////////////////////////////////////////
	// PrefabSerializer
	//////////////////////////////////////////////////////////////////////////////////

	void PrefabSerializer::Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const
	{
		auto prefab = std::static_pointer_cast<Prefab>(resource);

		auto scene = prefab->GetScene(); 

		if (!scene)
			return;

		SceneSerializer serializer(scene);

		nlohmann::json j;
		if (!serializer.SerializeToJSON(j))
			return;

		j["Name"] = prefab->GetName();
		j["PrefabHandle"] = static_cast<uint64_t>(prefab->Handle);
		j["RootEntityUUID"] = static_cast<uint64_t>(prefab->GetRootEntity().UUID());

		std::ofstream out(Project::GetEditorResourceManager()->GetFileSystemPath(metadata));
		if (out.is_open())
		{
			out << j.dump(4);
		}
	}

	bool PrefabSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const
	{
		auto path = Project::GetEditorResourceManager()->GetFileSystemPath(metadata);

		std::ifstream in(path);
		if (!in.is_open())
			return false;

		nlohmann::json j;
		in >> j;

		auto prefab = std::make_shared<Prefab>();

		auto scene = Scene::CreateEmpty();
		SceneSerializer serializer(scene);

		if (!serializer.DeserializeFromJSON(j))
			return false;

		if (j.contains("Name"))
			prefab->SetName(j["Name"].get<std::string>());

		prefab->SetScene(scene);
		prefab->Handle = j["PrefabHandle"].get<uint64_t>();

		UUID rootEntityUUID = j["RootEntityUUID"].get<uint64_t>();

		prefab->GetScene()->GetEntityManager().Update(0.0f);
		resource = prefab;
		return true;
	}

	bool PrefabSerializer::SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const
	{
		outInfo.Offset = stream.GetStreamPosition();

		auto prefab = ResourceManager::GetResource<Prefab>(handle);
		if (!prefab)
			return false;

		auto scene = prefab->GetScene();
		if (!scene)
			return false;

		SceneSerializer serializer(scene);

		nlohmann::json j;
		if (!serializer.SerializeToJSON(j))
			return false;

		j["Name"] = prefab->GetName();
		j["PrefabHandle"] = static_cast<uint64_t>(prefab->Handle);
		j["RootEntityUUID"] = static_cast<uint64_t>(prefab->GetRootEntity().UUID());

		std::string jsonStr = j.dump();
		stream.WriteString(jsonStr);

		outInfo.Size = stream.GetStreamPosition() - outInfo.Offset;
		return true;
	}

	std::shared_ptr<Resource> PrefabSerializer::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const
	{
		stream.SetStreamPosition(resourceInfo.PackedOffset);

		std::string jsonStr;
		stream.ReadString(jsonStr);

		nlohmann::json j = nlohmann::json::parse(jsonStr);

		auto prefab = std::make_shared<Prefab>();

		auto scene = Scene::CreateEmpty();
		SceneSerializer serializer(scene);

		if (!serializer.DeserializeFromJSON(j))
			return nullptr;

		if (j.contains("Name"))
			prefab->SetName(j["Name"].get<std::string>());
		prefab->SetScene(scene);
		prefab->Handle = j["PrefabHandle"].get<uint64_t>();

		UUID rootEntityUUID = j["RootEntityUUID"].get<uint64_t>();
		Entity rootEntity = scene->TryGetEntityWithUUID(rootEntityUUID);

		if (rootEntity.IsValid())
		{
			prefab->SetRootEntity(rootEntity);
		}
		prefab->GetScene()->GetEntityManager().Update(0.0f);

		return prefab;
	}

	//////////////////////////////////////////////////////////////////////////////////
	// SceneResourceSerializer
	//////////////////////////////////////////////////////////////////////////////////

	void SceneResourceSerializer::Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const
	{
		auto scene = std::static_pointer_cast<Scene>(resource);
		SceneSerializer serializer(scene);
		serializer.Serialize(Project::GetEditorResourceManager()->GetFileSystemPath(metadata).string());
	}

	bool SceneResourceSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const
	{
		auto scene = std::make_shared<Scene>();

		std::filesystem::path fullPath = Project::GetEditorResourceManager()->GetFileSystemPath(metadata);

		SceneSerializer serializer(scene);
		if (!serializer.Deserialize(fullPath.string()))
		{
			std::cerr << "[SceneResourceSerializer] Failed to deserialize scene from: " << fullPath << "\n";
			return false;
		}

		scene->Handle = metadata.Handle;

		resource = scene;
		return true;
	}

	bool SceneResourceSerializer::SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const
	{
		std::shared_ptr<Scene> scene = std::shared_ptr<Scene>();
		const auto& metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
		SceneSerializer serializer(scene);
		if (serializer.Deserialize(Project::GetActiveResourceDirectory() / metadata.FilePath))
		{
			return serializer.SerializeToResourcePack(stream, outInfo);
		}
		return false;
	}

	std::shared_ptr<Luden::Resource> SceneResourceSerializer::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const
	{
		return nullptr;
	}

	std::shared_ptr<Scene> SceneResourceSerializer::DeserializeSceneFromResourcePack(FileStreamReader& stream, const ResourcePackFile::SceneInfo& sceneInfo) const
	{
		std::shared_ptr<Scene> scene = std::make_shared<Scene>();
		SceneSerializer serializer(scene);
		if (serializer.DeserializeFromResourcePack(stream, sceneInfo))
			return scene;

		return nullptr;
	}

//////////////////////////////////////////////////////////////////////////////////
// AnimationResourceSerializer
//////////////////////////////////////////////////////////////////////////////////

	void AnimationResourceSerializer::Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const
	{
		auto anim = std::static_pointer_cast<Animation>(resource);

		nlohmann::json j;
		j["Name"] = anim->GetName();
		j["Loop"] = anim->IsLooping();
		j["FrameCount"] = anim->GetFrameCount();

		j["Frames"] = nlohmann::json::array();

		for (size_t i = 0; i < anim->GetFrameCount(); i++)
		{
			const auto& frame = anim->GetFrame(i);

			nlohmann::json jFrame;
			jFrame["SpriteHandle"] = static_cast<uint64_t>(frame.spriteHandle); 
			jFrame["Duration"] = frame.duration;
			jFrame["Offset"] = { frame.offset.x, frame.offset.y };

			j["Frames"].push_back(jFrame);
		}

		j["Handle"] = static_cast<uint64_t>(anim->Handle);

		std::ofstream out(Project::GetEditorResourceManager()->GetFileSystemPath(metadata));
		if (out.is_open())
		{
			out << j.dump(4);
		}
	}

	bool AnimationResourceSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const
	{
		auto path = Project::GetEditorResourceManager()->GetFileSystemPath(metadata);

		std::ifstream in(path);
		if (!in.is_open())
			return false;

		nlohmann::json j;
		in >> j;

		auto anim = std::make_shared<Animation>();

		if (j.contains("Name"))
			anim->SetName(j["Name"].get<std::string>());

		if (j.contains("Loop"))
			anim->SetLooping(j["Loop"].get<bool>());

		if (j.contains("Frames") && j["Frames"].is_array())
		{
			for (const auto& jFrame : j["Frames"])
			{
				ResourceHandle spriteHandle = jFrame["SpriteHandle"].get<uint64_t>();
				float duration = jFrame.value("Duration", 0.1f);

				anim->AddFrame(spriteHandle, duration);

				if (jFrame.contains("Offset"))
				{
					auto& frame = anim->GetFrame(anim->GetFrameCount() - 1);
					frame.offset.x = jFrame["Offset"][0].get<float>();
					frame.offset.y = jFrame["Offset"][1].get<float>();
				}
			}
		}

		anim->Handle = j["Handle"].get<uint64_t>();

		resource = anim;
		return true;
	}

	bool AnimationResourceSerializer::SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const
	{
		outInfo.Offset = stream.GetStreamPosition();

		auto anim = ResourceManager::GetResource<Animation>(handle);
		if (!anim)
			return false;

		nlohmann::json j;
		j["Name"] = anim->GetName();
		j["Loop"] = anim->IsLooping();
		j["FrameCount"] = anim->GetFrameCount();

		j["Frames"] = nlohmann::json::array();
		for (size_t i = 0; i < anim->GetFrameCount(); i++)
		{
			const auto& frame = anim->GetFrame(i);

			nlohmann::json jFrame;
			jFrame["SpriteHandle"] = static_cast<uint64_t>(frame.spriteHandle);
			jFrame["Duration"] = frame.duration;
			jFrame["Offset"] = { frame.offset.x, frame.offset.y };

			j["Frames"].push_back(jFrame);
		}

		j["Handle"] = static_cast<uint64_t>(anim->Handle);

		std::string jsonStr = j.dump();
		stream.WriteString(jsonStr);

		outInfo.Size = stream.GetStreamPosition() - outInfo.Offset;
		return true;
	}

	std::shared_ptr<Resource> AnimationResourceSerializer::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const
	{
		stream.SetStreamPosition(resourceInfo.PackedOffset);

		std::string jsonStr;
		stream.ReadString(jsonStr);

		nlohmann::json j = nlohmann::json::parse(jsonStr);

		auto anim = std::make_shared<Animation>();

		if (j.contains("Name"))
			anim->SetName(j["Name"].get<std::string>());

		if (j.contains("Loop"))
			anim->SetLooping(j["Loop"].get<bool>());

		if (j.contains("Frames") && j["Frames"].is_array())
		{
			for (const auto& jFrame : j["Frames"])
			{
				ResourceHandle spriteHandle = jFrame["SpriteHandle"].get<uint64_t>(); 
				float duration = jFrame.value("Duration", 0.1f);

				anim->AddFrame(spriteHandle, duration);

				if (jFrame.contains("Offset"))
				{
					auto& frame = anim->GetFrame(anim->GetFrameCount() - 1);
					frame.offset.x = jFrame["Offset"][0].get<float>();
					frame.offset.y = jFrame["Offset"][1].get<float>();
				}
			}
		}

		return anim;
	}

}