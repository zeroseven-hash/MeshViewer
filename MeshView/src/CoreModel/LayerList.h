#pragma once

#include"Layer.h"

namespace MV
{
	struct LayerNode
	{
		Layer* Layer;
		LayerNode* Next;

		LayerNode()
		{
			Layer = nullptr;
			Next = nullptr;
		}
		~LayerNode()
		{
			delete Layer;
		}
	};

	class LayerList
	{
	public:

		LayerList();
		~LayerList();
		
		//在最后插入
		void PushLayer(Layer* layer);
		//弹出特定层
		void PopLayer(const std::string& layerName);
		//插入顶层
		void PushOverlay(Layer* overlay);
		//弹出顶层
		void PopOverlay();

		LayerNode* const GetHeadPointer()const { return m_Head; }
		LayerNode* const GetTailPointer()const { return m_Tail; }
	private:
		LayerNode* m_Head;
		LayerNode* m_Tail;
		uint32_t m_Size = 0;
	};


}