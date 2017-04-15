/**************************kkplayer*********************************************/
/*******************Copyright (c) Saint ******************/
/******************Author: Saint *********************/
/*******************Author e-mail: lssaint@163.com ******************/
/*******************Author qq: 784200679 ******************/
/*******************KKPlayer  WWW: http://www.70ic.com/KKplayer ********************************/
/*************************date：2015-6-25**********************************************/
#ifndef KKPlayUI_H_
#define KKPlayUI_H_

enum EKKPlayerErr
{
     KKOpenUrlOk=0,          /***播发器打开成功**/
	 KKOpenUrlOkFailure=1,  /**播发器打开失败***/
	 KKAVNotStream=2,
	 KKAVReady=3,          ///缓冲已经准备就绪
	 KKAVWait=4,           ///需要缓冲
};
/*******UI接口********/
class IKKPlayUI
{
   public:
	         virtual unsigned char* GetWaitImage(int &length,int curtime)=0;
			 virtual unsigned char* GetErrImage(int &length,int ErrType)=0;
		     virtual unsigned char* GetBkImage(int &length)=0;
			 virtual unsigned char* GetCenterLogoImage(int &length)=0;
			 //打开失败
			 virtual void OpenMediaFailure(char* strURL,EKKPlayerErr err)=0;
			 
			 /*******视频流结束调用*******/
			 virtual void  AutoMediaCose(void *playerIns,int Stata,int quesize)=0;

			 /***视频读取线程结束调用****/
			 virtual void  AVReadOverThNotify(void *playerIns)=0;
			 virtual void  AVRender()=0;
   protected:
	         unsigned char* m_pBkImage;
			 int m_pBkImageLen;
			 int m_pCenterLogoImageLen;

};
#endif
