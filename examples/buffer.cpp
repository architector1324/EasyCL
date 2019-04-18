#include <iostream>
#include <EasyCL/EasyCL.hpp>


int main()
{
	auto plat = ecl::System::getPlatform(0);
	ecl::Computer video(0, plat, ecl::DEVICE::GPU);

	int A[] = { 1, 2, 3 ,4 };

	ecl::Buffer a(A, 4 * sizeof(int), ecl::ACCESS::READ_WRITE);
	ecl::Buffer b(nullptr, 0, ecl::ACCESS::READ_WRITE);

	video << a;
	b = a;
	video >> b;

	return 0;
}