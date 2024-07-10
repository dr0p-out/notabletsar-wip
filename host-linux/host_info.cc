#include <qapplication.h>
#include <qrect.h>
#include <qscreen.h>
#include <qwidget.h>

#include "../common/host_info.h"

int
host_scrsize (uint16_t *host_w,
              uint16_t *host_h)
{
  int unused;
  QApplication *app;
  QWidget *win;
  QScreen *scr;
  QRect rect;

  app = new QApplication ((unused = 1), (char *[]){ (char *)"foo", nullptr });
  win = new QWidget ();

  win->show ();
  app->processEvents ();

  scr = win->screen ();
  rect = scr->geometry ();

  *host_w = rect.width ();
  *host_h = rect.height ();

  win->close ();
  app->processEvents ();

  delete win;
  delete app;

  return 0;
}
