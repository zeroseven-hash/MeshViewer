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
		
		//��������
		void PushLayer(Layer* layer);
		//�����ض���
		void PopLayer(const std::string& layerName);
		//���붥��
		void PushOverlay(Layer* overlay);
		//��������
		void PopOverlay();

		LayerNode* const GetHeadPointer()const { return m_Head; }
		LayerNode* const GetTailPointer()const { return m_Tail; }
	private:
		LayerNode* m_Head;
		LayerNode* m_Tail;
		uint32_t m_Size = 0;
	};


}