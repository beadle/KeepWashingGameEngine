#pragma once

template<class T>
class MySingleton
{
protected:
	static T* m_instance;
	MySingleton(void);
public:
	~MySingleton(void);

	void purgeInstance();
	static T* sharedInstance();
};

template<class T>
T* MySingleton<T>::m_instance=NULL;

template<class T>
MySingleton<T>::MySingleton(void)
{
}

template<class T>
MySingleton<T>::~MySingleton(void)
{
}

template<class T>
T* MySingleton<T>::sharedInstance()
{
	if(m_instance==NULL)
	{
		m_instance=new T;
	}

	return m_instance;
}

template<class T>
void MySingleton<T>::purgeInstance()
{
	if(m_instance!=NULL)
	{
		delete m_instance;
		m_instance=NULL;
	}
}

