// Pre-processor overrides used in libs.
// Note: when changing these pre-processor macros, do a: PlatformIO: Clean
//  (trash can icon). HOLD_THRESHOLD is used by the lib IOAsbtraction and it
//  defines how long you should hold a button to consider it a hold, rather than
//  a regular press. 20~=.5 seconds is the default. The max allowed value is
//  254~=6.35sec. For 2 secs use 80.
#define HOLD_THRESHOLD 80  // 80~=2 seconds.
