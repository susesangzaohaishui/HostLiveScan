#ifndef DATA_QUEUE_HEAD_FILE
#define DATA_QUEUE_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////
//结构定义

//数据列头
struct tagDataHead
{
	unsigned short					wDataSize;							//数据大小
	unsigned short					wIdentifier;						//类型标识
};

//负荷信息
struct tagBurthenInfo
{
	unsigned int					dwDataSize;							//数据大小
	unsigned int					dwBufferSize;						//缓冲长度
	unsigned int					dwDataPacketCount;					//数据包数
};

//////////////////////////////////////////////////////////////////////////

//数据队列
class  data_queue_t
{
	//查询变量
protected:
	unsigned int							m_dwInsertPos;						//插入位置
	unsigned int							m_dwTerminalPos;					//结束位置
	unsigned int							m_dwDataQueryPos;					//查询位置

	//数据变量
protected:
	unsigned int							m_dwDataSize;						//数据大小
	unsigned int							m_dwDataPacketCount;				//数据包数

	//缓冲变量
protected:
	unsigned int							m_dwBufferSize;						//缓冲长度
	unsigned char*							m_pDataQueueBuffer;					//缓冲指针

	//函数定义
public:
	//构造函数
	data_queue_t();
	//析构函数
	virtual ~data_queue_t();

	//功能函数
public:
	//负荷信息
	bool get_burthen_info(tagBurthenInfo & BurthenInfo);

	//数据管理
public:
	//删除数据
	void remove_item(bool bFreeMemroy);
	//插入数据
    bool push_item(unsigned short wIdentifier, void * const pBuffer, unsigned short wDataSize);
	//提取数据
	bool pop_item(tagDataHead & DataHead, void * pBuffer, unsigned short wBufferSize);
};

//////////////////////////////////////////////////////////////////////////

#endif
