#ifndef DATA_QUEUE_HEAD_FILE
#define DATA_QUEUE_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//������ͷ
struct tagDataHead
{
	unsigned short					wDataSize;							//���ݴ�С
	unsigned short					wIdentifier;						//���ͱ�ʶ
};

//������Ϣ
struct tagBurthenInfo
{
	unsigned int					dwDataSize;							//���ݴ�С
	unsigned int					dwBufferSize;						//���峤��
	unsigned int					dwDataPacketCount;					//���ݰ���
};

//////////////////////////////////////////////////////////////////////////

//���ݶ���
class  data_queue_t
{
	//��ѯ����
protected:
	unsigned int							m_dwInsertPos;						//����λ��
	unsigned int							m_dwTerminalPos;					//����λ��
	unsigned int							m_dwDataQueryPos;					//��ѯλ��

	//���ݱ���
protected:
	unsigned int							m_dwDataSize;						//���ݴ�С
	unsigned int							m_dwDataPacketCount;				//���ݰ���

	//�������
protected:
	unsigned int							m_dwBufferSize;						//���峤��
	unsigned char*							m_pDataQueueBuffer;					//����ָ��

	//��������
public:
	//���캯��
	data_queue_t();
	//��������
	virtual ~data_queue_t();

	//���ܺ���
public:
	//������Ϣ
	bool get_burthen_info(tagBurthenInfo & BurthenInfo);

	//���ݹ���
public:
	//ɾ������
	void remove_item(bool bFreeMemroy);
	//��������
    bool push_item(unsigned short wIdentifier, void * const pBuffer, unsigned short wDataSize);
	//��ȡ����
	bool pop_item(tagDataHead & DataHead, void * pBuffer, unsigned short wBufferSize);
};

//////////////////////////////////////////////////////////////////////////

#endif
