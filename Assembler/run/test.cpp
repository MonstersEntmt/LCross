#include <chrono>
#include <iostream>

int main() {
	auto start = std::chrono::high_resolution_clock::now();
	__asm__(".intel_syntax noprefix\n\t"
	        "mov rcx, 4000000\n\t"
	        "OuterLoop:\n\t"
	        "rdrand rax\n\t"
	        "sub rcx, 1\n\t"
	        "jnz OuterLoop\n\t"
	        ".att_syntax prefix");
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << (end - start).count() << std::endl;
}