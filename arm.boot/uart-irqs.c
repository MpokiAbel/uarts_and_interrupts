#include "uart-irqs.h"

struct cb rxcb;
struct cb txcb;

void uart_initialization(int uart)
{

    uint16_t lcr = *(uint16_t *)(uart + CUARTLCR_H);
    lcr |= CUARTLCR_H_FEN;
    *(uint16_t *)(uart + CUARTLCR_H) = lcr;

    vic_setup();

    rxcb.head = 0;
    rxcb.tail = 0;
    txcb.head = 0;
    txcb.tail = 0;
}

void uart_tx_handler()
{
}

void uart_rx_handler()
{
}

void uart_receive(int uart, uint8_t *bytePt)
{
    cb_get(&rxcb, bytePt);
}

void uart_send(int uart, uint8_t *bytePt)
{
    cb_put(&txcb, bytePt);
}