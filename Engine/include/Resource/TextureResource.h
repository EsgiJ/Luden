#pragma once

#include <memory>

#include <SFML/Graphics/Texture.hpp>

#include "EngineAPI.h"
#include "Resource/IResource.h"

namespace Luden 
{

	class ENGINE_API TextureResource : public IResource 
	{
	public:
		virtual bool Load(std::shared_ptr<IFile> file) override;
		virtual void Unload() override;
		virtual void Init() override ;

		const sf::Texture& GetTexture() const;
	private:
		sf::Texture m_Texture;
	};
}
