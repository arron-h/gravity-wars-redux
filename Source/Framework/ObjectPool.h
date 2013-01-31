#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

template <class TType>
class ObjectPool
	{
	private:
		std::pair<TType, Uint8>*		m_pPool;
		Uint32							m_uiPoolSize;
	
	public:
		ObjectPool(Uint32 uiPoolSize) : m_uiPoolSize(uiPoolSize)
			{
			m_pPool = new std::pair<TType, Uint8>[m_uiPoolSize];

			// Initialise all pairs so that it's all free
			for(Uint32 i = 0; i < m_uiPoolSize; ++i)
				m_pPool[i].second = 0;
			}

		TType* Allocate()
			{
			// Find a free slot
			Uint32 uiIdx = 0xFFFFFFFF;
			for(Uint32 i = 0; i < m_uiPoolSize; ++i)
				{	
				if(m_pPool[i].second == 0)
					{
					uiIdx = i;
					break;
					}
				}

			if(uiIdx == 0xFFFFFFFF)		// No valid slot found :(
				return NULL;

			// Set this slot to 'used'
			m_pPool[uiIdx].second = 1;

			// Return the new element
			return &m_pPool[uiIdx].first;
			}

		void Free(TType* pObjElem)
			{
			// Find this object
			Uint32 uiIdx = 0xFFFFFFFF;
			TType* pToFind;
			for(Uint32 i = 0; i < m_uiPoolSize; ++i)
				{
				pToFind = &m_pPool[i].first;
				if(pToFind == pObjElem)
					{
					uiIdx = i;
					break;
					}
				}

			// Free, if found
			if(uiIdx != 0xFFFFFFFF)
				m_pPool[uiIdx].second = 0;
			}
	};

#endif
