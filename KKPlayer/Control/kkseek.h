#ifndef KK_SEEK_H_
#define KK_SEEK_H_
#include "../stdafx.h"
namespace SOUI
{
/** 
 * @class     SSliderBar
 * @brief     滑块工具条
 *
 * Describe   滑块工具条
 */
class  SAVSeekBar : public SProgress
{
    SOUI_CLASS_NAME(SSliderBar, L"avseekbar")

    struct RANGE
    {
        int value1;
        int value2;
    };
public:
    
   
    SAVSeekBar();
    ~SAVSeekBar();

	BOOL SetCacheValue(int nValue);
    enum
    {
        SC_RAIL,
        SC_SELECT,
        SC_THUMB,
        SC_RAILBACK,
    };

public:    
    int    HitTest(CPoint pt);
protected:
    BOOL      m_bDrag;          /**< 是否允许拖动 */
    CPoint    m_ptDrag;         /**< 拖动位置     */
    int       m_nDragValue;     /**< 拖动距离 */
    int       m_uHtPrev;        /**< 上次的鼠标位置 */

    ISkinObj *m_pSkinThumb;     /**< 皮肤对象 */
	ISkinObj *m_pSkinCachePos;  /**< 前景资源 */
    BOOL      m_bThumbInRail;   /**< 滑块包含在轨道中 */
	int m_nCacheValue;
protected:
    
    /**
     * SSliderBar::NotifyPos
     * @brief    通知 
     * @param    UINT uCode  -- 消息码
     * @param    int nPos  -- 坐标
     *
     * Describe  自定义消息响应函数 
     */
    LRESULT    NotifyPos(UINT uCode, int nPos);

    virtual CSize    GetDesiredSize(LPCRECT pRcContainer);    
    virtual void OnColorize(COLORREF cr);
    CRect   GetPartRect(UINT uSBCode);
    
    RANGE _GetPartRange(int nLength,int nThumbSize, BOOL bThumbInRail, int nMin,int nMax,int nValue, UINT uSBCode);

    void    OnPaint(IRenderTarget * pRT);
    void    OnLButtonUp(UINT nFlags, CPoint point);
    void    OnLButtonDown(UINT nFlags, CPoint point);    
    void    OnMouseMove(UINT nFlags, CPoint point);
    void    OnMouseLeave();
    SOUI_MSG_MAP_BEGIN()
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSELEAVE(OnMouseLeave)
        MSG_WM_PAINT_EX(OnPaint)
    SOUI_MSG_MAP_END()
    SOUI_ATTRS_BEGIN()
        ATTR_SKIN(L"thumbSkin", m_pSkinThumb, TRUE)
        ATTR_INT(L"thumbInRail",m_bThumbInRail,TRUE)
		ATTR_SKIN(L"cachePosSkin", m_pSkinCachePos, TRUE)
    SOUI_ATTRS_END()
};

}

#endif