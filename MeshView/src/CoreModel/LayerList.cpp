#include "pch.h"
#include "LayerList.h"
#include"Macro.h"
#include"Log.h"
namespace MV 
{

	LayerList::LayerList()
	{
		m_Head = nullptr;
		m_Tail = m_Head;
	}
	
	LayerList::~LayerList()
	{
		LayerNode* iter =m_Head;
		while (iter!= nullptr)
		{
			LayerNode* p = iter;
			iter = iter->Next;
			delete p;
		}
	}
	
	void LayerList::PushLayer(Layer* layer)
	{
		if (m_Head == nullptr)
		{
			m_Head = new LayerNode();
			m_Head->Layer = layer;
			m_Head->Next = nullptr;
			m_Tail = m_Head;
			m_Size++;
		}
		else
		{
			LayerNode* newNode = new LayerNode();
			newNode->Layer = layer;
			newNode->Next = nullptr;
			m_Tail->Next = newNode;
			m_Tail = newNode;
			m_Size++;
		}
	}
	
	void LayerList::PopLayer(const std::string& layerName)
	{
		IGAME_ASSERT(m_Head->Layer->GetName() != layerName, "if you want to pop overlayer,try PopOverlay()");
		LayerNode* iter = m_Head;
		
		while (iter->Next!=nullptr)
		{
			LayerNode* p = iter->Next;
			if (p->Layer->GetName() == layerName)
			{
				iter->Next = p->Next;
				iter = p;
				delete p;
				break;
			}
			iter = p;
			
		}
	}
	
	void LayerList::PushOverlay(Layer* overlay)
	{
		LayerNode* newHead=new LayerNode();

		newHead->Layer = overlay;
		newHead->Next = m_Head;
		m_Head = newHead;
		m_Size++;
		if (m_Size == 1)
			m_Tail = m_Head;
	}
	
	void LayerList::PopOverlay()
	{
		IGAME_ASSERT(m_Size > 0, "Layer is already empty!!");
		LayerNode* deleteHead = m_Head;
		m_Head = m_Head->Next;
		delete deleteHead;
		m_Size--;
		if (m_Size == 0)
			m_Tail = nullptr;
	}

}
