#include "config.h"

#if PRODUCT_VARIANT == 42
#	define GPIO_PWM		4
int something[256];
#endif

#if PRODUCT_VARIANT == 43
#	define GPIO_PWM		12
char something[256];
#endif

void setup() {
}

void loop() {

    while (true) {
    }

}
