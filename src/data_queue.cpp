#include "err.hpp"
#include "data_queue.hpp"
#include "debug.h"

#define  __max_(a,b)  (((a) > (b)) ? (a) : (b))
#define  __min_(a,b)  (((a) < (b)) ? (a) : (b))
#define  SafeDeleteArray(pData) { try { delete [] pData; } catch (...) { net_assert(false); } pData=0; } 
//////////////////////////////////////////////////////////////////////////

//构造函数
data_queue_t::data_queue_t()
{
	//查询变量
	m_dwInsertPos=0L;
	m_dwTerminalPos=0L;
	m_dwDataQueryPos=0L;

	//数据变量
	m_dwDataSize=0L;
	m_dwDataPacketCount=0L;

	//缓冲变量
	m_dwBufferSize=0L;
	m_pDataQueueBuffer=0;

	return;
}

//析构函数
data_queue_t::~data_queue_t()
{
	SafeDeleteArray(m_pDataQueueBuffer);
	return;
}

//负荷信息
bool data_queue_t::get_burthen_info(tagBurthenInfo & BurthenInfo)
{
	//设置变量
	BurthenInfo.dwDataSize=m_dwDataSize;
	BurthenInfo.dwBufferSize=m_dwBufferSize;
	BurthenInfo.dwDataPacketCount=m_dwDataPacketCount;

	return true;
}

//插入数据
bool data_queue_t::push_item(unsigned short wIdentifier, void * const pBuffer, unsigned short wDataSize)
{
	//初始化数据
	tagDataHead DataHead;
	DataHead.wDataSize=wDataSize;
	DataHead.wIdentifier= wIdentifier;
	unsigned int dwCopySize=sizeof(DataHead)+wDataSize;

	try
	{
		//效验缓冲
		if ((m_dwDataSize+dwCopySize)>m_dwBufferSize) throw 0;
		else if ((m_dwInsertPos==m_dwTerminalPos)&&((m_dwInsertPos+dwCopySize)>m_dwBufferSize)) 
		{
			if (m_dwDataQueryPos>=dwCopySize) m_dwInsertPos=0;
			else throw 0;
		}
		else if ((m_dwInsertPos<m_dwTerminalPos)&&((m_dwInsertPos+dwCopySize)>m_dwDataQueryPos)) throw 0;
	}
	catch (...)
	{
		try
		{
			//申请内存
			unsigned int dwNewBufferSize=m_dwBufferSize+__max_(m_dwBufferSize/2L,(wDataSize+sizeof(tagDataHead))*10L);
			unsigned char * pNewQueueServiceBuffer=new unsigned char [dwNewBufferSize];
            char strLog[120];
            sprintf(strLog,"!!!!!!!!!!!!!!!!!!!memory extern %d\n", dwNewBufferSize);
           // debug_str(strLog);
			if (pNewQueueServiceBuffer==0) return false;

			//拷贝数据
			if (m_pDataQueueBuffer!=0) 
			{
				net_assert(m_dwTerminalPos>=m_dwDataSize);
				net_assert(m_dwTerminalPos>=m_dwDataQueryPos);
				unsigned int dwPartOneSize=m_dwTerminalPos-m_dwDataQueryPos;
				if (dwPartOneSize>0L) memcpy(pNewQueueServiceBuffer,m_pDataQueueBuffer+m_dwDataQueryPos,dwPartOneSize);
				if (m_dwDataSize>dwPartOneSize) 
				{
					net_assert((m_dwInsertPos+dwPartOneSize)==m_dwDataSize);
					memcpy(pNewQueueServiceBuffer+dwPartOneSize,m_pDataQueueBuffer,m_dwInsertPos);
				}
			}

			//调整变量
			m_dwDataQueryPos=0L;
			m_dwInsertPos=m_dwDataSize;
			m_dwTerminalPos=m_dwDataSize;
			m_dwBufferSize=dwNewBufferSize;
			SafeDeleteArray(m_pDataQueueBuffer);
			m_pDataQueueBuffer=pNewQueueServiceBuffer;
		}
		catch (...) { return false; }
	}

	try
	{
		//拷贝数据
		memcpy(m_pDataQueueBuffer+m_dwInsertPos,&DataHead,sizeof(DataHead));
		if (wDataSize>0) 
		{
			net_assert(pBuffer!=0);
			memcpy(m_pDataQueueBuffer+m_dwInsertPos+sizeof(DataHead),pBuffer,wDataSize);
		}

		//调整数据
		m_dwDataPacketCount++;
		m_dwDataSize+=dwCopySize;
		m_dwInsertPos+=dwCopySize;
		m_dwTerminalPos=__max_(m_dwTerminalPos,m_dwInsertPos);

		return true;
	}
	catch (...) { }

	return false;
}

//获取数据
bool data_queue_t::pop_item(tagDataHead & DataHead, void * pBuffer, unsigned short wBufferSize)
{
	//效验变量
	net_assert(m_dwDataSize>0L);
	net_assert(m_dwDataPacketCount>0);
	net_assert(m_pDataQueueBuffer!=NULL);
	if (m_dwDataSize==0L) return false;
	if (m_dwDataPacketCount==0L) return false;

	//调整参数
	if (m_dwDataQueryPos==m_dwTerminalPos)
	{
		m_dwDataQueryPos=0L;
		m_dwTerminalPos=m_dwInsertPos;
	}

	//获取指针
	net_assert(m_dwBufferSize>=(m_dwDataQueryPos+sizeof(tagDataHead)));
	tagDataHead * pDataHead=(tagDataHead *)(m_pDataQueueBuffer+m_dwDataQueryPos);
	net_assert(wBufferSize>=pDataHead->wDataSize);

	//获取大小
	unsigned short wPacketSize=sizeof(DataHead)+pDataHead->wDataSize;
	net_assert(m_dwBufferSize>=(m_dwDataQueryPos+wPacketSize));

	//判断缓冲
	unsigned short wCopySize=0;
	net_assert(wBufferSize>=pDataHead->wDataSize);
	if (wBufferSize>=pDataHead->wDataSize) wCopySize=pDataHead->wDataSize;

	//拷贝数据
	DataHead=*pDataHead;
	if (DataHead.wDataSize>0)
	{
		if (wBufferSize<pDataHead->wDataSize) DataHead.wDataSize=0;
		else memcpy(pBuffer,pDataHead+1,DataHead.wDataSize);
	}

	//效验参数
	net_assert(wPacketSize<=m_dwDataSize);
	net_assert(m_dwBufferSize>=(m_dwDataQueryPos+wPacketSize));

	//设置变量
	m_dwDataPacketCount--;
	m_dwDataSize-=wPacketSize;
	m_dwDataQueryPos+=wPacketSize;

	return true;
}

//删除数据
void data_queue_t::remove_item(bool bFreeMemroy)
{
	//设置变量
	m_dwDataSize=0L;
	m_dwInsertPos=0L;
	m_dwTerminalPos=0L;
	m_dwDataQueryPos=0L;
	m_dwDataPacketCount=0L;

	//删除内存
	if (bFreeMemroy==true) 
	{
		m_dwBufferSize=0;
		SafeDeleteArray(m_pDataQueueBuffer);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
