#include <tchar.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

// Специфичные заголовки для использования анклава
#include "sgx_urts.h"
#include "sgx_tseal.h"
#include "Lab_3_Enclave_u.h"
#define ENCLAVE_FILE _T("../Simulation/Lab_3_Enclave.signed.dll")

#define BUF_LEN 100 // Длина буфера обмена между анклавом и небезопасным приложением

/*
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
*/

int main() {
	char buffer[BUF_LEN] = { 0 };

	// Активация анклава
	sgx_enclave_id_t eid;
	sgx_status_t ret = SGX_SUCCESS;
	sgx_launch_token_t token = { 0 };
	int updated = 0;

	// Создание анклава из бибилиотеки .dll
	ret = sgx_create_enclave(ENCLAVE_FILE, SGX_DEBUG_FLAG, &token, &updated, &eid, NULL);

	// Проверка на ошибки при создании
	if (ret != SGX_SUCCESS) {
		printf("App: error %#x, failed to create enclave.\n", ret);
		return -1;
	}

	while (true) {
		printf("Input index to retrieve, or -1 to exit: \t");
		int idx = -1;
		scanf_s("%d", &idx);
		if (idx < 0) {
			return 0;
		}
		//foo(buffer, BUF_LEN, idx);
		foo(eid, buffer, BUF_LEN, idx); // + eID (ID созданного анклава)
		printf("%s\n==============================\n\n", buffer);
	}

	// Выгрузка анклава
	if (SGX_SUCCESS != sgx_destroy_enclave(eid))
		return -1;
	return 0;
}
