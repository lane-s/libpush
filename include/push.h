#include "exported.h"

extern "C"
{
  EXPORTED bool libpush_connect(bool use_live_port);
  EXPORTED bool libpush_disconnect();
}
