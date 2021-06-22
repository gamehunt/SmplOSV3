#pragma once

#if !defined(K_SMPLOS_LOADER)
#warning "These headers are for internal purposes and shouldnt be used outside of SmplOSV3 loader until you exactly know what are you doing!"
#endif

#define PORT 0x3f8 

int serial_init();
int is_transmit_empty();
void serial_putchar(char c);