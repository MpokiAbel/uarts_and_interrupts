/*
 * cb.h
 *
 *  Created on: Oct 8, 2022
 *      Author: ogruber
 */

#ifndef CB_H_
#define CB_H_

#define CAPACITY 80
struct cb {
  volatile int head;
  volatile uint8_t buffer[CAPACITY];
  volatile int tail;
};

int cb_put(struct cb *cb, uint8_t byte);
int cb_get(struct cb*cb, uint8_t *byte);
void cb_init(struct cb*cb);
int cb_full(struct cb*cb);
int cb_empty(struct cb*cb);
#endif /* CB_H_ */
