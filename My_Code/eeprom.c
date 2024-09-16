#include "eeprom.h"

FLASH_ProcessTypeDef p_Flash; 
u16 STMFLASH_BUF_HALFWORD[STM_SECTOR_SIZE/2];    //��������
u32 STMFLASH_BUF_WORD[STM_SECTOR_SIZE/4];    //��������

 /**********************************************************************************
  * ��������: ��ȡָ����ַ�İ���(16λ����) 
  * �������: faddr������ַ
  * �� �� ֵ: ��Ӧ����
  * ˵    ���� 
  */
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
 /**********************************************************************************
  * ��������: ��ȡָ����ַ����(32λ����) 
  * �������: faddr������ַ
  * �� �� ֵ: ��Ӧ����
  * ˵    ���� 
  */
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}
 
#if STM32_FLASH_WREN	//���ʹ����д   
 /**********************************************************************************
  * ��������:������д��(16λ)
  * �������: WriteAddr:��ʼ��ַ��pBuffer:����ָ�롢NumToWrite:����(16λ)�� 
  * �� �� ֵ: ��
  * ˵    ���� 
  */
void STMFLASH_Write_NoCheck_HalfWord(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
} 
 /**********************************************************************************
  * ��������:������д��(32λ)
  * �������: WriteAddr:��ʼ��ַ��pBuffer:����ָ�롢NumToWrite:����(16λ)�� 
  * �� �� ֵ: ��
  * ˵    ���� 
  */
void STMFLASH_Write_NoCheck_Word(u32 WriteAddr,u32 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,pBuffer[i]);
	    WriteAddr+=4;//��ַ����4
	}  
} 
 /**********************************************************************************
  * ��������:��ָ����ַ��ʼд��ָ�����ȵ�����
  * �������:WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)��pBuffer:����ָ�롢NumToWrite������(16λ)��(����Ҫд���16λ���ݵĸ���.)
  * �� �� ֵ: ��
  * ˵    ���� 
  */
void STMFLASH_Write_HalfWord(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//�Ƿ���ַ
	
	HAL_FLASH_Unlock();					    //����
	offaddr=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~64 for STM32F103C8T6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)secremain=NumToWrite;//�����ڸ�������Χ
	while(1) 
	{	
		STMFLASH_Read_HalfWord(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF_HALFWORD,STM_SECTOR_SIZE/2);//������������������
		for(i=0;i<secremain;i++)	//У������
		{
			if(STMFLASH_BUF_HALFWORD[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
		}
		if(i<secremain)				//��Ҫ����
		{
			Flash_PageErase(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);	//�����������
			FLASH_WaitForLastOperation(FLASH_WAITETIME);            	//�ȴ��ϴβ������
			CLEAR_BIT(FLASH->CR, FLASH_CR_PER);							//���CR�Ĵ�����PERλ���˲���Ӧ����FLASH_PageErase()����ɣ�
																		//����HAL�����沢û������Ӧ����HAL��bug��
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF_HALFWORD[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck_HalfWord(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF_HALFWORD,STM_SECTOR_SIZE/2);//д����������  
		}else 
		{
			FLASH_WaitForLastOperation(FLASH_WAITETIME);       	//�ȴ��ϴβ������
			STMFLASH_Write_NoCheck_HalfWord(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 
		}
		if(NumToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain*2;	//д��ַƫ��(16λ���ݵ�ַ,��Ҫ*2)	   
		   	NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//��һ����������д����
			else secremain=NumToWrite;//��һ����������д����
		}	 
	};	
	HAL_FLASH_Lock();		//����
}
/**********************************************************************************
  * ��������:��ָ����ַ��ʼд��ָ�����ȵ�����
  * �������:WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)��pBuffer:����ָ�롢NumToWrite������(16λ)��(����Ҫд���32λ���ݵĸ���.)
  * �� �� ֵ: ��
  * ˵    ���� 
  */
void STMFLASH_Write_Word(u32 WriteAddr,u32 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u32 secoff;	   //������ƫ�Ƶ�ַ(32λ�ּ���)
	u32 secremain; //������ʣ���ַ(32λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//�Ƿ���ַ
	
	HAL_FLASH_Unlock();					    //����
	offaddr=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~255 for STM32F103RCT6
	secoff=(offaddr%STM_SECTOR_SIZE)/4;		//�������ڵ�ƫ��(4���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/4-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)secremain=NumToWrite;//�����ڸ�������Χ
	while(1) 
	{	
		STMFLASH_Read_Word(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF_WORD,STM_SECTOR_SIZE/4);//������������������(����BUF��)
		for(i=0;i<secremain;i++)	//У������
		{
			if(STMFLASH_BUF_WORD[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
		}
		if(i<secremain)				//��Ҫ����
		{
			Flash_PageErase(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);	//�����������
			FLASH_WaitForLastOperation(FLASH_WAITETIME);            	//�ȴ��ϴβ������
			CLEAR_BIT(FLASH->CR, FLASH_CR_PER);							//���CR�Ĵ�����PERλ���˲���Ӧ����FLASH_PageErase()����ɣ�
																		//����HAL�����沢û������Ӧ����HAL��bug��
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF_WORD[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck_Word(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF_WORD,STM_SECTOR_SIZE/4);//д����������  
		}else 
		{
			FLASH_WaitForLastOperation(FLASH_WAITETIME);       	//�ȴ��ϴβ������
			STMFLASH_Write_NoCheck_Word(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 
		}
		if(NumToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain*4;	//д��ַƫ��(32λ���ݵ�ַ,��Ҫ*4)	   
		   	NumToWrite-=secremain;	//�ֽ�(32λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/4))secremain=STM_SECTOR_SIZE/4;//��һ����������д����
			else secremain=NumToWrite;//��һ����������д����
		}	 
	};	
	HAL_FLASH_Lock();		//����
}
#endif
 /**********************************************************************************
  * ��������:��ָ����ַ��ʼ����ָ�����ȵ�����
  * �������:ReadAddr:��ʼ��ַ��pBuffer:����ָ�롢NumToWrite:����(16λ)��
  * �� �� ֵ: ��
  * ˵    ���� 
  */
void STMFLASH_Read_HalfWord(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}
  /**********************************************************************************
  * ��������:��ָ����ַ��ʼ����ָ�����ȵ�����
  * �������:ReadAddr:��ʼ��ַ��pBuffer:����ָ�롢NumToWrite:����(16λ)��
  * �� �� ֵ: ��
  * ˵    ���� 
  */
void STMFLASH_Read_Word(u32 ReadAddr,u32 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
}
 /**********************************************************************************
  * ��������:��������
  * �������:PageAddress:����������ַ
  * �� �� ֵ: ��
  * ˵    ���� 
  */
void Flash_PageErase(uint32_t PageAddress)
{
  /* Clean the error context */
  p_Flash.ErrorCode = HAL_FLASH_ERROR_NONE;
 
#if defined(FLASH_BANK2_END)
  if(PageAddress > FLASH_BANK1_END)
  { 
    /* Proceed to erase the page */
    SET_BIT(FLASH->CR2, FLASH_CR2_PER);
    WRITE_REG(FLASH->AR2, PageAddress);
    SET_BIT(FLASH->CR2, FLASH_CR2_STRT);
  }
  else
  {
#endif /* FLASH_BANK2_END */
    /* Proceed to erase the page */
    SET_BIT(FLASH->CR, FLASH_CR_PER);
    WRITE_REG(FLASH->AR, PageAddress);
    SET_BIT(FLASH->CR, FLASH_CR_STRT);
#if defined(FLASH_BANK2_END)
 
  }
#endif /* FLASH_BANK2_END */
 }
