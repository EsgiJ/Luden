#pragma once

#include <memory>

#include <SFML/Graphics/Font.hpp>

#include "EngineAPI.h"
#include "Resource/IResource.h"

namespace Luden
{
	class ENGINE_API FontResource : public IResource
	{
	public:
		virtual bool Load(std::shared_ptr<IFile> file) override;
		virtual void Unload() override;
		virtual void Init() override;

		const sf::Font& GetFont() const;
	private:
		sf::Font m_Font;
	};
}
