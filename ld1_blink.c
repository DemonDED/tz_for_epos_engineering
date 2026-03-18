// Мигаем синим светодиодом LD2 на PE1 (NUCLEO-H745ZI-Q)

// --- Адреса регистров (из справочного руководства RM0399) ---
#define RCC_BASE            0x58024400UL
#define RCC_AHB4ENR         (*(volatile unsigned int *)(RCC_BASE + 0xE0))
// Бит 4 включает тактирование порта E
#define RCC_AHB4ENR_GPIOEEN (1 << 4)

#define GPIOE_BASE          0x58021000UL
#define GPIOE_MODER         (*(volatile unsigned int *)(GPIOE_BASE + 0x00))
#define GPIOE_BSRR          (*(volatile unsigned int *)(GPIOE_BASE + 0x18))

#define PIN_LD2             1   // PE1

// Простая задержка (примерно полсекунды при частоте 400 МГц, но точность не важна)
static void delay(volatile unsigned int count) {
    while (count--) {
        __asm volatile("nop");
    }
}

// Точка входа
void Reset_Handler(void) {
    // 1. Включаем тактирование GPIOE
    RCC_AHB4ENR |= RCC_AHB4ENR_GPIOEEN;

    // 2. Настраиваем PE1 как выход (push-pull)
    GPIOE_MODER &= ~(0x3 << (2 * PIN_LD2));  // очищаем биты режима
    GPIOE_MODER |=  (0x1 << (2 * PIN_LD2));  // режим выхода (01)

    // 3. Бесконечный цикл мигания
    while (1) {
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
