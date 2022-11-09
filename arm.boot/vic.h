/*
 * isr.h
 *
 *  Created on: Oct 8, 2022
 *      Author: ogruber
 */

#ifndef ISR_H_
#define ISR_H_

/*
 * Versatile Application Baseboard for ARM926EJ-S User Guide HBI-0118
 *   (https://developer.arm.com/documentation/dui0225/latest)
 *   3. Hardware Description
 *   3.10 Interrupts
 *     Page 111, Figure 3.23 UART0 IRQ = 12
 */
#define UART0_IRQ 12
#define UART0_IRQ_MASK (1 << UART0_IRQ)

#define UART1_IRQ 13
#define UART1_IRQ_MASK (1 << UART1_IRQ)

#define UART2_IRQ 14
#define UART2_IRQ_MASK (1 << UART2_IRQ)

#define UART2_IRQ 14
#define UART2_IRQ_MASK (1 << UART2_IRQ)

// #define UART3_IRQ 3
// #define UART3_IRQ_MASK (1 << UART3_IRQ)

void wfi(void);

void vic_setup();
void vic_enable();
void vic_disable();

void vic_irq_enable(int irq,
                    void (*handler)(void *),
                    void *cookie);

void vic_irq_disable(int irq);

#endif /* ISR_H_ */
