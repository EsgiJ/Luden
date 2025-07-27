#pragma once

#include <memory>

#include <SFML/Audio/SoundBuffer.hpp>

#include "EngineAPI.h"
#include "Resource/IResource.h"

namespace Luden
{
	class ENGINE_API SoundResource : public IResource
	{
	public:
		virtual bool Load(std::shared_ptr<IFile> file) override;
		virtual void Unload() override;
		virtual void Init() override;

		const sf::SoundBuffer& GetSoundBuffer() const;
	private:
		sf::SoundBuffer m_SoundBuffer;
	};
}