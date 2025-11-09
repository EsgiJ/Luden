#pragma once

#include "EngineAPI.h"
#include "Resource/Resource.h"

#include <functional>
#include <filesystem>

namespace Luden
{
	class ScriptableEntity;

	using ScriptInstantiateFunc = ScriptableEntity* (*)();
	using ScriptDestroyFunc = void(*)(ScriptableEntity*);

	class ENGINE_API NativeScript : public Resource
	{
	public:
		NativeScript() = default; 

		NativeScript(const std::string& name, ScriptInstantiateFunc instantiate, ScriptDestroyFunc destroy)
			: m_ClassName(name)
			, m_InstantiateFunc(instantiate)
			, m_DestroyFunc(destroy)
		{}

		~NativeScript() override = default;

		static ResourceType GetStaticType() { return ResourceType::NativeScript; }
		virtual ResourceType GetResourceType() const override { return GetStaticType(); }

		const std::string& GetClassName() { return m_ClassName; }
		const std::filesystem::path& GetHeaderPath() { return m_HeaderPath; }
		const std::filesystem::path& GetSourcePath() { return m_SourcePath; }

		void SetClassName(const std::string& className) { m_ClassName = className; }
		void SetHeaderPath(const std::filesystem::path& headerPath) { m_HeaderPath = headerPath; }
		void SetSourcePath(const std::filesystem::path& sourcePath) { m_SourcePath = sourcePath; }

		ScriptInstantiateFunc GetInstantiateFunc() const { return m_InstantiateFunc; }
		ScriptDestroyFunc GetDestroyFunc() const { return m_DestroyFunc; }
	private:
		std::string m_ClassName;
		std::filesystem::path m_HeaderPath;
		std::filesystem::path m_SourcePath;

		ScriptInstantiateFunc m_InstantiateFunc = nullptr;
		ScriptDestroyFunc m_DestroyFunc = nullptr;
	};
}