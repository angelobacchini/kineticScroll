
#include "global.h"
#include "view.h"

#include <QtWidgets>

void myView::mousePressEvent(QMouseEvent* event)
{
  widget->m_scroller->resetScroll();
  widget->m_scroller->setMousePressed(true);
  widget->m_scroller->startScroll();
}

void myView::mouseReleaseEvent(QMouseEvent* event)
{
  widget->m_scroller->setMousePressed(false);
}

myWidget::myWidget(QWidget* parent) :
  QWidget(parent)
{
  QImage image(":/resources/pictures/panorama.jpg");
  if(image.isNull()) qDebug("null image!!!");

  m_graphicsItem = new QGraphicsPixmapItem(QPixmap::fromImage(image));

  m_scene = new myScene(this);
  m_scene->addItem(m_graphicsItem);

  m_view = new myView(this);
  m_view->setDragMode(QGraphicsView::NoDrag);
  m_view->setFocusPolicy(Qt::NoFocus);
  m_view->setInteractive(true);
  m_view->setRenderHint(QPainter::Antialiasing, true);
  m_view->setStyleSheet("border-style: none; background: transparent;");
  m_view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
  m_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff );
  m_view->setScene(m_scene);

  m_scroller = new scroller(m_view, this);

  QHBoxLayout* hLayout = new QHBoxLayout();

  m_speedLabel = new QLabel("");
  m_speedLabel->setFixedWidth(400);
  m_speedLabel->setStyleSheet(SPEED_LABEL_STYLE);
  hLayout->addWidget(m_speedLabel);

  QLabel* sliderLabel = new QLabel("friction");
  sliderLabel->setStyleSheet(SLIDER_LABEL_STYLE);
  m_frictionSlider = new QSlider();
  m_frictionSlider->setOrientation(Qt::Horizontal);
  m_frictionSlider->setMinimum(0);
  m_frictionSlider->setMaximum(20000);
  m_frictionSlider->setValue(DEFAULT_FRICTION);
  hLayout->addWidget(sliderLabel);
  hLayout->addWidget(m_frictionSlider);

  sliderLabel = new QLabel("bounce damping");
  sliderLabel->setStyleSheet(SLIDER_LABEL_STYLE);
  m_dampingSlider = new QSlider();
  m_dampingSlider->setOrientation(Qt::Horizontal);
  m_dampingSlider->setMinimum(1);
  m_dampingSlider->setMaximum(10);
  m_dampingSlider->setValue(DEFAULT_DAMPING);
  hLayout->addWidget(sliderLabel);
  hLayout->addWidget(m_dampingSlider);

  QGridLayout* layout = new QGridLayout;
  layout->addWidget(m_view, 0, 0);
  layout->addLayout(hLayout, 1, 0);

  setLayout(layout);

  connect(m_frictionSlider, SIGNAL(valueChanged(int)), m_scroller, SLOT(setFriction(int)));
  connect(m_dampingSlider, SIGNAL(valueChanged(int)), m_scroller, SLOT(setDamping(int)));
  connect(m_scroller, SIGNAL(sendSpeed(QString)), m_speedLabel, SLOT(setText(QString)));
}

myWidget::~myWidget()
{}
