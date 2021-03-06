#include <iostream>

#include "mainframe.h"

int main() {
	try {
		Mainframe* frame = Mainframe::instance();
		frame->start();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}