#pragma once

#include <string>
#include <memory>

#include "EngineAPI.h"
#include "IO/IFileSystem.h"

namespace Luden 
{

	class ENGINE_API IResource 
	{
	public:
		virtual ~IResource() = default;
		virtual bool Load(std::shared_ptr<IFile> file) = 0;
		virtual void Unload() = 0;
		virtual void Init();
	};

	using ResourceID = std::string;
}
