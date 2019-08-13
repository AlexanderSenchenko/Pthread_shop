#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define COUNT_SHOPS 5
#define COUNT_THREAD 3

int shops[COUNT_SHOPS];
pthread_mutex_t mutex[COUNT_SHOPS];

void* customer(void* ptr)
{
	int* gold = (int*) ptr;
	unsigned int buf = (unsigned int) *gold;
	int rand_shop;

	#if 1
	while (*gold > 0) {
		rand_shop = rand_r(&buf) % COUNT_SHOPS;

		pthread_mutex_lock(&mutex[rand_shop]);

		printf("shop %d: old %d gold %d\n", rand_shop, shops[rand_shop], *gold);

		if (shops[rand_shop] <= *gold) {
			*gold -= shops[rand_shop];
			shops[rand_shop] = 0;
		} else {
			shops[rand_shop] -= *gold;
			*gold = 0;
		}

		printf("shop %d: new %d gold %d\n", rand_shop, shops[rand_shop], *gold);

		pthread_mutex_unlock(&mutex[rand_shop]);
		
		sleep(3);
	}
	#endif
	return NULL;
}

void* loader(void* ptr)
{
	int gold = 500;
	int buf = gold;
	int rand_shop;

	#if 1
	while (1) {
		rand_shop = rand_r(&buf) % COUNT_SHOPS;

		pthread_mutex_lock(&mutex[rand_shop]);

		printf("shop %d: old %d\n", rand_shop, shops[rand_shop]);

		shops[rand_shop] += gold;

		printf("shop %d: new %d\n", rand_shop, shops[rand_shop]);

		pthread_mutex_unlock(&mutex[rand_shop]);

		sleep(2);
	}
	#endif
	return NULL;
}

int main()
{
	int gold[COUNT_THREAD];
	pthread_t tid[COUNT_THREAD];
	void* status[COUNT_THREAD];
	pthread_t loader_id;
	void* loader_status;
 
	int i, j;

	srand(time(NULL));

	printf("Start\n");
	for (i = 0; i < COUNT_SHOPS; ++i) {
		shops[i] = 900 + rand() % 200;
		pthread_mutex_init(&mutex[i], NULL);
		printf("shop %d: %d\n", i + 1, shops[i]);
	}

	for (i = 0; i < COUNT_THREAD; ++i) {
		gold[i] = 9000 + rand() % 2000;
		printf("%d\n", gold[i]);
	}

	for (i = 0; i < COUNT_THREAD; ++i)
		pthread_create(&tid[i], NULL, customer, &gold[i]);

	pthread_create(&loader_id, NULL, loader, NULL);

	for (i = 0; i < COUNT_THREAD; ++i)
		pthread_join(tid[i], &status[i]);

	pthread_cancel(loader_id);
	pthread_join(loader_id, &loader_status);

	for (i = 0; i < COUNT_SHOPS; ++i)
		pthread_mutex_destroy(&mutex[i]);

	return 0;
}
