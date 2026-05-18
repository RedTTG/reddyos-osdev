#pragma once
void panic(const char* msg);
void panic_isr(const char* msg, const interrupt_frame_t* frame);