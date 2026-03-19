struct gpio {
	volatile uint32_t MODER, OTYPER, OSPEEDR,	PURDR, IDR, ODR, BSRR, LCKR, AFR[2];
};

#define GPIOA ((struct gpio *) 0x58020000)


