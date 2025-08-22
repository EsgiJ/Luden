#include "Resource/ResourceSerializer.h"

#include "Audio/SoundBuffer.h"
#include "Graphics/Animation.h"
#include "Graphics/Font.h"
#include "Graphics/Texture.h"
#include "IO/FileSystem.h"
#include "Project/Project.h"
#include "Resource/ResourceManager.h"
#include "Scene/SceneSerializer.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

namespace Luden
{
//////////////////////////////////////////////////////////////////////////////////
// TextureSerializer
//////////////////////////////////////////////////////////////////////////////////
	bool TextureSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource> resource) const
	{
		auto texture = std::static_pointer_cast<Texture>(resource);
		sf::Texture sfTexture;
		if (!sfTexture.loadFromFile(metadata.FilePath))
			return false;

		texture->SetTexture(sfTexture);
		return true;
	}

	bool TextureSerializer::SerializeToResourcePack(ResourceHandle handle, FileStreamWriter& stream, ResourceSerializationInfo& outInfo) const
	{
		outInfo.Offset = stream.GetStreamPosition();

		auto metadata = Project::GetEditorResourceManager()->GetMetadata(handle);
		std::shared_ptr<Texture> texture = ResourceManager::GetResource<Texture>(handle);
		outInfo.Size = TextureRuntimeSerializer::SerializeTextureToFile(texture, stream);
		return true;
	}

	std::shared_ptr<Resource> TextureSerializer::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const
	{
		stream.SetStreamPosition(resourceInfo.PackedOffset);
		return TextureRuntimeSerializer::DeserializeTexture(stream);
	}

	//////////////////////////////////////////////////////////////////////////////////
	// FontSerializer
	//////////////////////////////////////////////////////////////////////////////////

	bool FontSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource> resource) const
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

		std::string name;
		stream.ReadString(name);
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

	void AudioFileSourceSerializer::Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource> resource) const
	{

	}

	bool AudioFileSourceSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource> resource) const
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
	void PrefabSerializer::Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource> resource) const
	{
	}

	bool PrefabSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource> resource) const
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

	void SceneResourceSerializer::Serialize(const ResourceMetadata& metadata, const std::shared_ptr<Resource> resource) const
	{
		auto scene = std::static_pointer_cast<Scene>(resource);
		SceneSerializer serializer(scene);
		serializer.Serialize(Project::GetEditorResourceManager()->GetFileSystemPath(metadata).string());
	}

	bool SceneResourceSerializer::TryLoadData(const ResourceMetadata& metadata, std::shared_ptr<Resource> resource) const
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

	std::shared_ptr<Resource> SceneResourceSerializer::DeserializeFromResourcePack(FileStreamReader& stream, const ResourcePackFile::ResourceInfo& resourceInfo) const
	{
		// Not implemented
		return nullptr;
	}

	std::shared_ptr<Scene> SceneResourceSerializer::DeserializeSceneFromResourcePack(FileStreamReader& stream, const ResourcePackFile::SceneInfo& sceneInfo) const
	{
		std::shared_ptr<Scene> scene = std::shared_ptr<Scene>();
		SceneSerializer serializer(scene);
		if (serializer.DeserializeFromResourcePack(stream, sceneInfo))
			return scene;

		return nullptr;
	}
}