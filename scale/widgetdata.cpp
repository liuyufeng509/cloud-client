#include "widgetdata.h"
#include "frameless_helper.h"
#include <QMouseEvent>
#include <QtDebug>

const qreal WIDTH_TO_HEIGHT = 1.4;  //宽高比 700/500
/***** WidgetData *****/
WidgetData::WidgetData(FramelessHelperPrivate *_d, QWidget *pTopLevelWidget)
{
    d = _d;
    m_pWidget = pTopLevelWidget;
    m_bLeftButtonPressed = false;
    m_bCursorShapeChanged = false;
    m_bLeftButtonTitlePressed = false;
    m_pRubberBand = NULL;

    m_windowFlags = m_pWidget->windowFlags();
    m_pWidget->setMouseTracking(false);
    m_pWidget->setAttribute(Qt::WA_Hover, true);

    updateRubberBandStatus();
}

WidgetData::~WidgetData()
{
    m_pWidget->setMouseTracking(false);
    m_pWidget->setWindowFlags(m_windowFlags);
    m_pWidget->setAttribute(Qt::WA_Hover, false);

    delete m_pRubberBand;
    m_pRubberBand = NULL;
}

QWidget* WidgetData::widget()
{
    return m_pWidget;
}

void WidgetData::handleWidgetEvent(QEvent *event)
{
    switch (event->type())
    {
    default:
        break;
    case QEvent::MouseButtonPress:
        handleMousePressEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseButtonRelease:
        handleMouseReleaseEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseMove:
        handleMouseMoveEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::Leave:
        handleLeaveEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::HoverMove:
        handleHoverMoveEvent(static_cast<QHoverEvent*>(event));
        break;
    }
}

void WidgetData::updateRubberBandStatus()
{
    if (d->m_bRubberBandOnMove || d->m_bRubberBandOnResize)
    {
        if (NULL == m_pRubberBand)
            m_pRubberBand = new QRubberBand(QRubberBand::Rectangle);
    }
    else
    {
        delete m_pRubberBand;
        m_pRubberBand = NULL;
    }
}

void WidgetData::updateCursorShape(const QPoint &gMousePos)
{
    if (m_pWidget->isFullScreen() || m_pWidget->isMaximized())
    {
        if (m_bCursorShapeChanged)
        {
            m_pWidget->unsetCursor();
        }
        return;
    }

    m_moveMousePos.recalculate(gMousePos, m_pWidget->frameGeometry());

    if(m_moveMousePos.m_bOnTopLeftEdge || m_moveMousePos.m_bOnBottomRightEdge)
    {
        m_pWidget->setCursor( Qt::SizeFDiagCursor );
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnTopRightEdge || m_moveMousePos.m_bOnBottomLeftEdge)
    {
        m_pWidget->setCursor( Qt::SizeBDiagCursor );
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnLeftEdge || m_moveMousePos.m_bOnRightEdge)
    {
        m_pWidget->setCursor( Qt::SizeHorCursor );
        m_bCursorShapeChanged = true;
    }
    else if(m_moveMousePos.m_bOnTopEdge || m_moveMousePos.m_bOnBottomEdge)
    {
        m_pWidget->setCursor( Qt::SizeVerCursor );
        m_bCursorShapeChanged = true;
    }
    else
    {
        if (m_bCursorShapeChanged)
        {
            m_pWidget->unsetCursor();
            m_bCursorShapeChanged = false;
        }
    }
}

void WidgetData::resizeWidget(const QPoint &gMousePos)
{
    QRect origRect;

    if (d->m_bRubberBandOnResize)
        //origRect = m_pRubberBand->frameGeometry();
        origRect = m_rubRect;
    else
        origRect = m_pWidget->frameGeometry();

    qDebug()<<"m_pRubberBand Geometry:"<<m_pRubberBand->frameGeometry()<<" m_pWidget->frameGeometry():"<<m_pWidget->frameGeometry();

    qDebug()<<"m_bRubberBandOnResize:"<<d->m_bRubberBandOnResize<<" origRect:"<<origRect<<" gMousePos:"<<gMousePos;

    int left = origRect.left();
    int top = origRect.top();
    int right = origRect.right();
    int bottom = origRect.bottom();
    origRect.getCoords(&left, &top, &right, &bottom);
    qDebug()<<"left:"<<left<<" top:"<<top<<" right:"<<right<<" bottom:"<<bottom;

    int minWidth = m_pWidget->minimumWidth();
    int minHeight = m_pWidget->minimumHeight();
    qDebug()<<"minWidt:"<<minWidth<<" minHeight:"<<minHeight;
//    qDebug()<<"m_bOnTopLeftEdge:"<<m_pressedMousePos.m_bOnTopLeftEdge<<
//              " m_bOnBottomLeftEdge:"<<m_pressedMousePos.m_bOnBottomLeftEdge<<
//              " m_bOnTopRightEdge:"<<m_pressedMousePos.m_bOnTopRightEdge<<
//              " m_bOnBottomRightEdge:"<<m_pressedMousePos.m_bOnBottomRightEdge<<
//              " m_bOnLeftEdge:"<<m_pressedMousePos.m_bOnLeftEdge<<
//              " m_bOnRightEdge:"<<m_pressedMousePos.m_bOnRightEdge<<
//              " m_bOnTopEdge:"<<m_pressedMousePos.m_bOnTopEdge<<
//              " m_bOnBottomEdge:"<<m_pressedMousePos.m_bOnBottomEdge;
    if (m_pressedMousePos.m_bOnTopLeftEdge)
    {
        left = gMousePos.x();
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomLeftEdge)
    {
        left = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnTopRightEdge)
    {
        right = gMousePos.x();
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomRightEdge)
    {
        right = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnLeftEdge)
    {
        left = gMousePos.x();
    }
    else if (m_pressedMousePos.m_bOnRightEdge)
    {
        right = gMousePos.x();
    }
    else if (m_pressedMousePos.m_bOnTopEdge)
    {
        top = gMousePos.y();
    }
    else if (m_pressedMousePos.m_bOnBottomEdge)
    {
        bottom = gMousePos.y();
    }

    QRect newRect(QPoint(left, top), QPoint(right, bottom));
    qDebug()<<"newRect:"<<newRect;
    if (newRect.isValid())
    {
        //橡皮筋可以任意大小，但是松开时，窗口是有最大最小限制的，所以这里的判断可以去掉。
//        if (minWidth > newRect.width())
//        {
//            if (left != origRect.left())    //说明缩放的是左边，将左边设置为最小宽度
//                newRect.setLeft(origRect.right()-minWidth);
//            else    //缩放有边界
//                newRect.setRight(origRect.left()+minWidth);
//        }
//        if (minHeight > newRect.height())
//        {
//            if (top != origRect.top())  //说明缩放的是上边界
//                newRect.setTop(origRect.bottom()+minHeight);
//            else
//                newRect.setBottom(origRect.top()-minHeight);
//        }
        //四个角拉伸缩放时，要保持比例。
        if(m_pressedMousePos.m_bOnBottomRightEdge||m_pressedMousePos.m_bOnTopLeftEdge||
           m_pressedMousePos.m_bOnBottomLeftEdge||m_pressedMousePos.m_bOnTopRightEdge)
        {
            newRect.setWidth(newRect.height()*WIDTH_TO_HEIGHT);
        }
        if (d->m_bRubberBandOnResize )
        {
           m_pRubberBand->setGeometry(newRect);
        }
        else
        {
            m_pWidget->setGeometry(newRect);
        }
        lastPos = gMousePos;
    }
}

void WidgetData::moveWidget(const QPoint& gMousePos)
{
    if (d->m_bRubberBandOnMove)
    {
        m_pRubberBand->move(gMousePos - m_ptDragPos);
    }
    else
    {
        m_pWidget->move(gMousePos - m_ptDragPos);
    }
}

void WidgetData::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = true;
        m_bLeftButtonTitlePressed = event->pos().y() < m_moveMousePos.m_nTitleHeight;

        QRect frameRect = m_pWidget->frameGeometry();
        m_pressedMousePos.recalculate(event->globalPos(), frameRect);

        m_ptDragPos = event->globalPos() - frameRect.topLeft();

        if (m_pressedMousePos.m_bOnEdges)
        {
            if (d->m_bRubberBandOnResize)
            {
                m_pRubberBand->setGeometry(frameRect);
                m_rubRect =frameRect;
                m_pRubberBand->show();
            }
        }
        else if (d->m_bRubberBandOnMove)
        {
            m_pRubberBand->setGeometry(frameRect);
            m_pRubberBand->show();
        }
    }
}

void WidgetData::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = false;
        m_bLeftButtonTitlePressed = false;
        m_pressedMousePos.reset();
        if (m_pRubberBand && m_pRubberBand->isVisible())
        {
            m_pRubberBand->hide();
            m_pWidget->setGeometry(m_pRubberBand->geometry());
        }
    }
}

void WidgetData::handleMouseMoveEvent(QMouseEvent *event)
{
    //qDebug()<<"move event";
    if (m_bLeftButtonPressed)
    {
        if (d->m_bWidgetResizable && m_pressedMousePos.m_bOnEdges)
        {
            resizeWidget(event->globalPos());
        }
        else if (d->m_bWidgetMovable && m_bLeftButtonTitlePressed)
        {
            moveWidget(event->globalPos());
        }
    }
    else if (d->m_bWidgetResizable)
    {
        updateCursorShape(event->globalPos());
    }
}

void WidgetData::handleLeaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    if (!m_bLeftButtonPressed)
    {
        m_pWidget->unsetCursor();
    }
}

void WidgetData::handleHoverMoveEvent(QHoverEvent *event)
{
    if (d->m_bWidgetResizable)
    {
        updateCursorShape(m_pWidget->mapToGlobal(event->pos()));
    }
}