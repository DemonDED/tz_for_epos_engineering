// Задача 1 - светодиод + функционал на кнопку

// --- Адреса регистров из reference manual ---
#define RCC_BASE            0x58024400UL
#define RCC_AHB4ENR         (*(volatile unsigned int *)(RCC_BASE + 0xE0))
// --- Адреса GPIO ---
#define GPIOE_BASE          0x58021000UL
#define GPIOB_BASE			 0x58020400UL
#define GPIOC_BASE			 0x58020800UL
// --- Настройка GPIO ---
#define GPIOE_MODER         (*(volatile unsigned int *)(GPIOE_BASE + 0x00))
#define GPIOB_MODER			 (*(volatile unsigned int *)(GPIOB_BASE + 0x00))
#define GPIOC_MODER			 (*(volatile unsigned int *)(GPIOC_BASE + 0x00))

#define GPIOE_BSRR          (*(volatile unsigned int *)(GPIOE_BASE + 0x18))
#define GPIOB_BSRR			 (*(volatile unsigned int *)(GPIOB_BASE + 0x18))

#define GPIOC_IDR				 (*(volatile unsigned int *)(GPIOC_BASE + 0x10))
#define GPIOC_PUPDR			 (*(volatile unsigned int *)(GPIOC_BASE + 0x0C))

#define PIN_LD2             1   // PE1
#define PIN_LD3				 14  // PB14
#define BUTTON_PIN			 13  // PC13

// Простая задержка
static void delay(volatile unsigned int count) {
    while (count--) {
        __asm volatile("nop");
    }
}



// Точка входа
void Reset_Handler(void) {
    // Включаем тактирование GPIOE
   // RCC_AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
	
	 RCC_AHB4ENR |= (1 << 4); // GPIOEEN
	 RCC_AHB4ENR |= (1 << 1); // GPIOBEN
	 RCC_AHB4ENR |= (1 << 2); // GPIOCEN

    // Настраиваем PE1 как выход (push-pull)
    GPIOE_MODER &= ~(0x3 << (2 * PIN_LD2));  // Очищаем биты режима
    GPIOE_MODER |=  (0x1 << (2 * PIN_LD2));  // Режим выхода (01)
	 // Настраиваем PB14 как выход (push-pull)
	 GPIOB_MODER &= ~(0x3 << (2 * PIN_LD2));
	 GPIOB_MODER |=  (0x1 << (2 * PIN_LD2));
	 // Настройка кнопки B1 (PC13) как выход с подтяжкой вверх
	 GPIOC_MODER &= ~(0x3 << (2 * BUTTON_PIN)); // Режим входа
	 GPIOC_PUPDR &= ~(0x3 << (2 * BUTTON_PIN)); // Очищаем биты режима
	 GPIOC_PUPDR |=  (0x2 << (2 * BUTTON_PIN)); // Подтяжка вниз (pull-down)

	 GPIOB_BSRR = (1 << (PIN_LD3 + 16)); // Reset bit
	
	 unsigned char last_button_state = 0;
	 unsigned char ld3_state = 0;
	 //unsigned char counter = 0;

    // Код программы
    while (1) {
		  // Читаем состояние кнопки (PC13)
		  unsigned char button_state = (GPIOC_IDR & (1 << BUTTON_PIN)) ? 1 : 0;
			
		  if (button_state == 1 && last_button_state == 0) {
			 delay(500);

			 if (GPIOC_IDR & (1 << BUTTON_PIN)) {
				if (ld3_state) {
					GPIOB_BSRR = (1 << (PIN_LD3 + 16)); // OFF LD3
					ld3_state = 0;
				} else {
					GPIOB_BSRR = (1 << PIN_LD3); // ON LD3
					ld3_state = 0;
				}
			 }
		  }

		  last_button_state = button_state;

        // Включаем светодиод (высокий уровень) — активный HIGH
        GPIOE_BSRR = (1 << PIN_LD2);
        delay(2000000);

        // Выключаем светодиод (низкий уровень)
        GPIOE_BSRR = (1 << (PIN_LD2 + 16));
        delay(2000000);

    }
}

// Минимальная таблица векторов (обязательна для корректного старта)
extern unsigned int _estack;  // определена в скрипте линкера
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void)) &_estack,
    Reset_Handler
};

// Заглушка для обработчика аппаратной ошибки (на всякий случай)
void HardFault_Handler(void) { while(1); }
