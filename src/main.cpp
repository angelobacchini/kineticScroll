
#include <QApplication>
#include <QMainWindow>
#include <QStyleFactory>

#include "view.h"

int main(int argc, char *argv[])
{
  QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));
  QApplication qtApp(argc, argv);
  qtApp.setStyle(QStyleFactory::create("Fusion"));
  QMainWindow window;
  myWidget* widget = new myWidget();

  window.setCentralWidget(widget);
  window.show();
  qtApp.exec();
  //-----
  return 0;
}
