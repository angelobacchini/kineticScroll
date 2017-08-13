
#ifndef SCROLLER_H
#define SCROLLER_H

#include <QtWidgets>

class scroller : public QWidget
{
  Q_OBJECT
public:
  explicit scroller(QGraphicsView* _view, QWidget* parent = 0);
  ~scroller();

private slots:
  void process();

public slots:
  void setFriction(const int& _friction);
  void setDamping(const int& _damping);

public:
  void startScroll();
  void resetScroll();
  void setMousePressed(bool pressed);

signals:
  void scrollOver();
  void sendSpeed(const QString& _speed);

public:
  bool m_mousePressed;

private:
  QGraphicsView* m_view;
  QTimer* m_timer;

  QPoint m_mouseLastPosition;
  qreal m_direction;
  qreal m_speed;
  qreal m_friction;
  qreal m_damping;
};

#endif /*SCROLLER_H */
