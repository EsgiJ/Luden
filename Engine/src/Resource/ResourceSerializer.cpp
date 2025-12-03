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

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

namespace Luden
{
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
/*
	void PrefabSerializer::Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource>& resource) const
	{
	}

	bool PrefabSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource>& resource) const
	{
	}

	bool PrefabSerializer::SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const
	{
	}

	std::shared_ptr<Resource> PrefabSerializer::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const
	{
	}

	std::string PrefabSerializer::SerializeToYAML(std::shared_ptr<Prefab> prefab) const
	{
	}

	bool PrefabSerializer::DeserializeFromYAML(const std::string& yamlString, std::shared_ptr<Prefab> prefab) const
	{
	}
*/
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
		resource = std::shared_ptr<Scene>();
		resource->Handle = metadata.Handle;
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

	void AnimationResourceSerializer::Serialize(const ResourceMetadata & metadata, const std::shared_ptr<Resource>&resource) const
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
			jFrame["TextureHandle"] = static_cast<uint64_t>(frame.textureHandle);
			jFrame["Duration"] = frame.duration;
			jFrame["Offset"] = { frame.offset.x, frame.offset.y };

			j["Frames"].push_back(jFrame);
		}

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
				ResourceHandle textureHandle = jFrame["TextureHandle"].get<uint64_t>();
				float duration = jFrame.value("Duration", 0.1f);

				anim->AddFrame(textureHandle, duration);

				if (jFrame.contains("Offset"))
				{
					auto& frame = anim->GetFrame(anim->GetFrameCount() - 1);
					frame.offset.x = jFrame["Offset"][0].get<float>();
					frame.offset.y = jFrame["Offset"][1].get<float>();
				}
			}
		}

		resource = anim;
		return true;
	}

	bool AnimationResourceSerializer::SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const
	{
		outInfo.Offset = stream.GetStreamPosition();

		std::shared_ptr<Animation> anim = ResourceManager::GetResource<Animation>(handle);

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
			jFrame["TextureHandle"] = static_cast<uint64_t>(frame.textureHandle);
			jFrame["Duration"] = frame.duration;
			jFrame["Offset"] = { frame.offset.x, frame.offset.y };

			j["Frames"].push_back(jFrame);
		}

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

		nlohmann::json j;
		j = nlohmann::json::parse(jsonStr);

		auto anim = std::make_shared<Animation>();

		if (j.contains("Name"))
			anim->SetName(j["Name"].get<std::string>());

		if (j.contains("Loop"))
			anim->SetLooping(j["Loop"].get<bool>());

		if (j.contains("Frames") && j["Frames"].is_array())
		{
			for (const auto& jFrame : j["Frames"])
			{
				ResourceHandle textureHandle = jFrame["TextureHandle"].get<uint64_t>();
				float duration = jFrame.value("Duration", 0.1f);

				anim->AddFrame(textureHandle, duration);

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