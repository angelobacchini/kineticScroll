# kineticScroll
Kinetic scrolling in Qt.

Kinetic scrolling adds a momentum effect and elastic edge to traditional scrolling. It feels like spinning a prize wheel: when you lift your hand and stop moving the wheel, it will continue to spin for a while until eventually lose momentum and stop. 

I think kinetic scrolling first came out with the iPhone, to later being adopted on practically every touch-screen device. I tried to imagine what would be a simple implementation of it and wrote some C++ code in Qt.

## QGraphicsView

QGraphicsView class implements a widget to display a graphics scene. The graphics scene is implemented by **QGraphicsScene** class, which acts as a container for one or more graphical items (lines, text, shapes, images ...).

```c++
myWidget::myWidget(QWidget* parent) :
  QWidget(parent)
{
  QImage image(":/resources/pictures/panorama.jpg");
  if(image.isNull()) qDebug("null image!!!");

  m_graphicsItem = new QGraphicsPixmapItem(QPixmap::fromImage(image));

  m_scene = new QGraphicsScene(this);
  m_scene->addItem(m_graphicsItem);

  m_view = new QGraphicsView(this);
  m_view->setScene(m_scene);
...

```

First, a *graphics item* is created (an image read from a jpg.). Second, a *graphics scene* is created and the item is added to the scene. Lastly, a *graphics view* is created, and the graphic scene is assigned to it.

The widget now contains a view of the jpg image and provides a standard scroll functionality. To hijack the boring standard scroll and install a custom-made kinetic scroll, the **QGraphicsView** class has to be subclassed to override the default mouse buttons event listeners.

```c++
class myView : public QGraphicsView {
  Q_OBJECT
public:
  myView(QWidget* _widget) : 
  QGraphicsView(_widget) {
    widget = qobject_cast<myWidget*>(_widget);
  }

protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  myWidget *widget;
};
```

The new scroll code is built upon these two events: mouse button press and mouse button release (corresponding to pressing and lifting a finger from the touch-screen).

As the user taps and drags the view around, the graphics scene has to move accordingly, giving the illusion that the view follows the finger movement. That is the same as in basic scrolling. As the finger is released, the kinetic part of the scrolling kicks in, keeping momentum from the previous scrolling, smoothly slowing down and eventually coming to a halt.

From the old high school physics book, equations of motion can be written as two coupled differential equations:

dx(t)/dt=v(t)
dv(t)/dt=a(t)

To integrate this set of equations and being able to translate them to code, time must be discretized as *t=t0, t1*,..., with a constant time step *Δt=tn+1−tn*.

The simplest way to advance time from tn to tn+1 is then to use a first-order approximation (Euler algorithm):

x[n+1]=x[n]+Δtv[n]
v[n+1]=v[n]+ΔtA

This is pretty much all the math needed to recreate a smooth deceleration effect (given "A" a negative constant value).

* set intial values for x and v
* repeat every Δt:
    * x = x + Δt∗v
    * v = x + Δt∗A
    * if v>0: break

The algorithm starts running when the user lifts the finger from the screen and speed and position values are updated after every Δt. 

How to estimate the initial speed *v0* and position *x0*? A basic solution is to track speed and position periodically whenever the view is being dragged by the user.

So, let's translate all this to c++: the mouse press event handler calls three methods from a custom built **scroller** class:

```c++
void myView::mousePressEvent(QMouseEvent* event) {
  widget->m_scroller->resetScroll();
  widget->m_scroller->setMousePressed(true);
  widget->m_scroller->startScroll();
}
And here are the methods from the scroller class:

void scroller::setMousePressed(bool pressed) {
  m_mousePressed = pressed;
}

void scroller::startScroll() {
  m_timer->start((int)(KINETIC_PERIOD*1000.f));
}

void scroller::resetScroll() {
  m_mouseLastPosition = QPoint(0, 0);
  m_direction = 0;
  m_speed = 0;
  m_timer->stop();
}
```

**resetScroll()** is used to reset several class properties to their default value (scroll position, speed etc..) while **setMousePressed** is pretty much self-explicative: **m_mousePressed** property shall be set to true when the mouse button is pressed (or when the finger is touching the screen) and to false when is lifted. 

**startScroll()** starts a periodic timer that is at the core of this class. This timer is used to track velocity when the user is dragging the view and to implement the inertial motion when the dragging is over. The timer period is set to 10 ms.

At timer expiration, **process()** method from scroller class is called, and this code branch will be executed if **m_mousePressed** property is true:

```c++
...
  qreal currentX = m_view->horizontalScrollBar()->value();
  deltaX = m_mouseLastPosition.x() - QCursor::pos().x();
  // v dx/dt => speed in pixels per period (smoothed with a lowpass iir)
  m_speed = 0.8f*m_speed + 0.2f*fabs(deltaX)/KINETIC_PERIOD; 
  newX = currentX + deltaX; // update position
  m_view->horizontalScrollBar()->setValue(newX);
  m_mouseLastPosition = QCursor::pos();
  m_timer->start((int)(KINETIC_PERIOD*1000.f));
...

```

Speed is computed as the ratio of the number of pixels the cursor moved in the timer interval to the time interval itself. 

An averaging filter is applied to smooth and protect the tracking from any wild erroneous variation. The same amount of relative scrolling movement is also applied to the view scroll-bar and the cursor position is saved for the next iteration.

When the user dragging is over, the mousePressed property is set to false so that when **process()** method is called, a different code branch will be executed:

```c++
  // dx = v*dt => x(t) = x0 + v*dt => new position based on current speed
  newX = currentX + m_direction*m_speed*KINETIC_PERIOD;
  // v = v0 - k/dt => compute new speed
  m_speed = m_speed - m_friction*KINETIC_PERIOD;
```

The new position is computed from previous position and speed values, and the new speed value is decreased by a quantity proportional to the m_friction class property.

The view's offset is then updated and the timer is triggered again until speed value eventually reaches 0.

```c++
m_view->horizontalScrollBar()->setValue(newX);

if(m_speed > 0)
  m_timer->start((int)(KINETIC_PERIOD*1000.f));
else {
  emit scrollOver();
  m_speed = 0;
}

```

### Edge Bouncing
What should happen when the edge of the view is reached? A cool effect would be a sort of damped "bounce": part of the momentum of the scroll shall be kept but in the opposite direction. Here's the code:

```
// bounce when end of view is reached
if(newX > m_view->horizontalScrollBar()->maximum()) { 
  newX = m_view->horizontalScrollBar()->maximum()*2 - newX;
  m_direction = -1;
  m_speed = m_speed / m_damping; // decrease speed on bounce
}
else if(newX < m_view->horizontalScrollBar()->minimum()) 
  newX = - newX;
  m_direction = 1;
  m_speed = m_speed / m_damping; // decrease speed on bounce
}
```

### build
Builds with Qt 5.9.1

Build with 

        qmake kineticScroll.pro
        make

