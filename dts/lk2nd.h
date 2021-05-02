#ifndef DT_LK2ND_H
#define DT_LK2ND_H

/* keys.h */
#define KEY_VOLUMEUP	0x115
#define KEY_VOLUMEDOWN	0x116

#define KEY_HOME	0x122
#define KEY_BACK	0x123
#define KEY_MENU	0x124

/* platform/gpio.h */
#define GPIO_NO_PULL    0
#define GPIO_PULL_DOWN  1
#define GPIO_KEEPER     2
#define GPIO_PULL_UP    3

/* pm8x41.h */
#define PM_GPIO_PULL_UP_30      0
#define PM_GPIO_PULL_UP_1_5     1
#define PM_GPIO_PULL_UP_31_5    2
/* 1.5uA + 30uA boost */
#define PM_GPIO_PULL_UP_1_5_30  3
#define PM_GPIO_PULLDOWN_10     4
#define PM_GPIO_NO_PULL         5

#define PM_GPIO(x) (x | (1 << 16))

#define GPIO_ACTIVE_LOW  0
#define GPIO_ACTIVE_HIGH (1 << 8)

#define LK2ND_KEY_RESIN (2 << 16) 0

#endif
