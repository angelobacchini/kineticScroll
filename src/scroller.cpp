
#include "global.h"
#include "scroller.h"

scroller::scroller(QGraphicsView* _view, QWidget *parent) :
  QWidget(parent), m_view(_view)
{
  m_mouseLastPosition = QPoint(0, 0);
  m_direction = 0;
  m_speed = 0;
  m_friction = DEFAULT_FRICTION; // implicit int to qreal
  m_damping = DEFAULT_DAMPING; // implicit int to qreal

  m_timer = new QTimer(this);
  m_timer->setSingleShot(true);

  connect(m_timer, SIGNAL(timeout()), this, SLOT(process()));
}

scroller::~scroller()
{}

void scroller::setMousePressed(bool pressed)
{
  m_mousePressed = pressed;
}

void scroller::startScroll()
{
  m_timer->start((int)(KINETIC_PERIOD*1000.f));
}

void scroller::resetScroll()
{
  m_mouseLastPosition = QPoint(0, 0);
  m_direction = 0;
  m_speed = 0;
  m_timer->stop();
}

void scroller::setFriction(const int& _friction)
{
  m_friction = _friction; // implicit int to qreal
}

void scroller::setDamping(const int& _damping)
{
  m_damping = _damping; // implicit int to qreal
}

void scroller::process()
{
  qreal currentX = m_view->horizontalScrollBar()->value();
  qreal newX;
  qreal deltaX;

  if(m_mousePressed)
  {
    if (m_mouseLastPosition != QPoint(0, 0))
    {
      deltaX = m_mouseLastPosition.x() - QCursor::pos().x();
      m_direction = (deltaX >= 0) ? 1 : -1;
      m_speed = 0.8f*m_speed + 0.2f*fabs(deltaX)/KINETIC_PERIOD; // v = dx/dt => speed in pixels per period (smoothed with a lowpass iir)
      newX = currentX + deltaX; // update position
      m_view->horizontalScrollBar()->setValue(newX);
    }
    m_mouseLastPosition = QCursor::pos();
    m_timer->start((int)(KINETIC_PERIOD*1000.f));
  }
  else // mouse released
  {
    newX = currentX + m_direction*m_speed*KINETIC_PERIOD; // dx = v*dt => x(t) = x0 + v*dt => new position based on current speed
    m_speed = m_speed - m_friction*KINETIC_PERIOD; // v = v0 - k/dt => compute new speed

    if(newX > m_view->horizontalScrollBar()->maximum()) // bounce when end of view is reached
    {
      newX = m_view->horizontalScrollBar()->maximum()*2 - newX;
      m_direction = -1;
      m_speed = m_speed / m_damping; // decrease speed on bounce
    }
    else if(newX < m_view->horizontalScrollBar()->minimum())
    {
      newX = - newX;
      m_direction = 1;
      m_speed = m_speed / m_damping; // decrease speed on bounce
    }

    m_view->horizontalScrollBar()->setValue(newX);

    if(m_speed > 0)
      m_timer->start((int)(KINETIC_PERIOD*1000.f));
    else
    {
      emit scrollOver();
      m_speed = 0;
    }
  }
  sendSpeed("pixelsPerSecond: " + QString::number(m_speed, 'f', 2));
}
