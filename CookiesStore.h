#ifndef COOKIES_STORE_H
#define COOKIES_STORE_H


template <int N> class CookiesStore
{
private:
	int m_store[N]; //lista dei cookies validi

public:
	CookiesStore()
	{
		for (int i = 0; i < N ; i++)
		{
			m_store[i] = 0;
		}
	}

	int Get(int index) const { return m_store[index]; }

	bool CheckCookie(int cookieValue) 
	{		
		for (int i = 0; i < N; i++)
		{
			if (m_store[i] == cookieValue)
			{
				for (int y = i-1; y >= 0; y--)
				{
					m_store[y+1] = m_store[y];
				}
				m_store[0] = cookieValue;
				return true;


			}
		}


		return false;
	}
	void AddNewCookie(int cookieValue)
	{

		if (CheckCookie(cookieValue) == true)
		{
			return;
		}

			for (int i = N-1; i >= 1; i--)
			{
				m_store[i] = m_store[i - 1];
				
			}
			m_store[0] = cookieValue;
		

	}

};



#endif // !COOKIES_STORE_H
