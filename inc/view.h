
#ifndef VIEW_H
#define VIEW_H

#include "scroller.h"

class myWidget;

class myView : public QGraphicsView
{
  Q_OBJECT
public:
  myView(QWidget* _widget) :
    QGraphicsView(_widget)
  {
    widget = qobject_cast<myWidget*>(_widget);
  }

protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  myWidget *widget;
};

class myScene : public QGraphicsScene
{
  Q_OBJECT
public:
  myScene(QWidget* _widget) :
    QGraphicsScene(_widget)
  {
    widget = qobject_cast<myWidget*>(_widget);
  }

private:
  myWidget *widget;
};

class myWidget : public QWidget
{
  Q_OBJECT

  friend class myView;
  friend class myScene;

public:
  explicit myWidget(QWidget* parent = 0);
  virtual ~myWidget();

private:
  QGraphicsPixmapItem* m_graphicsItem;
  scroller* m_scroller;
  myView* m_view;
  myScene* m_scene;
  QLabel* m_speedLabel;
  QSlider* m_frictionSlider;
  QSlider* m_dampingSlider;
};

#endif // VIEW_H
