#include "Lab_3_Enclave_t.h"
#include "sgx_trts.h"
#include <string.h>

const char table[6][41] = { // Тестовые данные
	"20AB2126338A42868E4C0859DB78EAED",
	"B8AB6AC0E6BB4B9896BE6653D0FB68BA",
	"54A67B6333514BFFB433898F893C38ED",
	"8485BE169E2B487681EF4121593736C2",
	"A1241715CEA0460EA9BD7B6E93F05193"
};

void foo(char* buf, size_t len, size_t idx) {
	if (idx < 5) {
		const char* data_ptr = data_ptr = table[idx];
		memcpy(buf, data_ptr, strlen(data_ptr + 1));
	}
	else {
		memset(buf, 0, strlen(table[0]));
	}
	return;
}