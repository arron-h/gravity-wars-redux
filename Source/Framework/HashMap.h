#ifndef HASHMAP_H
#define HASHMAP_H

template <class TType>
class HashPair
	{
	public:
		Uint32 uiHash;
		TType Val;
	};

template <class TType>
class HashMap
	{
	private:
		HashPair<TType>*		m_pPairs;
		Uint32					m_uiCurrent;
		Uint32					m_uiGrowBy;
		Uint32					m_uiMax;

	public:
		HashMap() : m_pPairs((HashPair<TType>*)NULL), m_uiCurrent(0), m_uiMax(0), m_uiGrowBy(128)
			{
			}

		~HashMap()
			{
			if(m_pPairs != NULL)
				delete [] m_pPairs;
			}

		void SetSize(Uint32 uiNewSize)
			{
			// Free
			if(uiNewSize == 0)
				{
				if(m_pPairs != NULL)
					delete [] m_pPairs;

				m_pPairs		= NULL;
				m_uiCurrent		= 0;
				m_uiMax			= 0;
				return;
				}

			// Fits already
			if(uiNewSize <= m_uiMax)
				{
				m_uiCurrent = uiNewSize;
				return;
				}

			// Resize
			if(uiNewSize > m_uiMax)
				{
				m_uiMax = m_uiMax + m_uiGrowBy;

				// --- Allocate new size array
				HashPair<TType> *pNewData = new HashPair<TType>[m_uiMax];

				// --- Copy source data into new array;
				for(Uint32 uiIndex = 0; uiIndex < m_uiCurrent; uiIndex++)
					pNewData[uiIndex] = m_pPairs[uiIndex];

				// --- Free old array
				if(m_pPairs)
					delete [] m_pPairs;

				// --- Update array stats
				m_pPairs		= pNewData;
				m_uiCurrent		= uiNewSize;
				return;
				}
			}

		void Add(HashPair<TType> Pair)
			{
			Uint32 uiIdx = m_uiCurrent;
			SetSize(m_uiCurrent + 1);

			m_pPairs[uiIdx] = Pair;
			}

		TType Get(Uint32 uiHash) const
			{
			for(Uint32 uiIndex = 0; uiIndex < m_uiCurrent; ++uiIndex)
				{
				if(uiHash == m_pPairs[uiIndex].uiHash)
					return m_pPairs[uiIndex].Val;
				}

			ASSERT(!"Unabled to find Hash value!");
			return 0;
			}
	};

Uint32 HashMap_Hash(const char* c_pszString);

#endif
