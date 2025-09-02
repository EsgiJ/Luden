#pragma once

#include "Core/Layer.h"
#include "EngineAPI.h"

#include <vector>

namespace Luden
{
	class ENGINE_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		Layer* operator[](size_t index)
		{
			//TODO: Assert index >= 0 AND index < m_Layers.size()
			return m_Layers[index];
		}

		const Layer* operator[](size_t index) const
		{
			//TODO: Assert index >= 0 AND index < m_Layers.size()
			return m_Layers[index];
		}

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};
}