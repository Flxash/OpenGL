#pragma once

class IndexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	//distinction between size and count naming
	//when using size it is exclusivly bytes
	//count is used for indices
	//this isnt standard or a rule but that is the cherno's standard
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
};
//note for future performance implementation
//it might be worth considereing 16 bit integars or "shorts"
//this is because for smaller renders when you dont have large mashes with many polygons
//16 bit integars are faster to process which will increase performance