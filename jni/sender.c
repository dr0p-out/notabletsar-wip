#include <fcntl.h>
#include <unistd.h>

#include "com_notabletsar_app_ControllerActivity.h"

#include "../inc/protocol.h"

static int fd_ = -1;
static int last_mv_dx = -1;
static int last_mv_dy = -1;

JNIEXPORT void JNICALL
Java_com_notabletsar_app_ControllerActivity_setUSBFdJNI (JNIEnv *env, jclass clz, jint fd)
{
  (void)env;
  (void)clz;

  fd_ = fd;
}

JNIEXPORT void JNICALL
Java_com_notabletsar_app_ControllerActivity_closeUSBFdIfAnyJNI (JNIEnv *env, jclass clz)
{
  (void)env;
  (void)clz;

  close (fd_);
}

#define send_buff(fd, pkt) \
  write (fd, pkt, sizeof (*pkt))

JNIEXPORT void JNICALL
Java_com_notabletsar_app_ControllerActivity_doHandShakeJNI (JNIEnv *env, jclass clz, jint scr_w, jint scr_h)
{
  struct shake_pkt pkt = {
    .ver   = NTS_PROTO_V1,
    .scr_w = scr_w,
    .scr_h = scr_h,
    .reserved = {},
  };

  (void)env;
  (void)clz;

  send_buff(fd_, &pkt);
}

static void JNICALL
reportToHostJNI (jint action, jint cur_id, jfloat x, jfloat y)
{
  int dx;
  int dy;
  struct report_pkt_v1 pkt;

  dx = x;
  dy = y;

  action &= DROID_ACTION_MASK;
  switch (action)
  {
    case DROID_ACTION_MOVE:
      if (dx == last_mv_dx && dy == last_mv_dy)
        return;  // non-primary finger

      last_mv_dx = dx;
      last_mv_dy = dy;

      // fall-through

    /* primary finger */
    case DROID_ACTION_DOWN:
    case DROID_ACTION_UP:
      pkt.flags = action;
      break;

    /* non-primary fingers */
    case DROID_ACTION_POINTER_DOWN:
    case DROID_ACTION_POINTER_UP:
      pkt.flags = action | cur_id << POINTER_ID_SHIFT;
      break;

    default:
      return;  // discard
  }

  pkt.x = dx;
  pkt.y = dy;

  send_buff(fd_, &pkt);
}

static const JNINativeMethod funcs[] = {
  { "reportToHostJNI", "(IIFF)V", (void *)&reportToHostJNI },
};

JNIEXPORT jint JNICALL
JNI_OnLoad (JavaVM *vm, void *reserved)
{
  JNIEnv *env;
  jclass clz;

  (void)reserved;

  if ((*vm)->GetEnv (vm, (void *)&env, JNI_VERSION_1_6) != JNI_OK)
    return JNI_ERR;

  clz = (*env)->FindClass (env, "com/notabletsar/app/ControllerActivity");
  if (!clz)
    return JNI_ERR;

  if ((*env)->RegisterNatives (env, clz, funcs, sizeof(funcs) / sizeof(funcs[0])) != JNI_OK)
    return JNI_ERR;

  return JNI_VERSION_1_6;
}
