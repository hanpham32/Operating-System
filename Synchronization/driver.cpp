#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include "Shop.h"
#include <queue>
using namespace std;

void *barber(void *);
void *customer(void *);

// ThreadParam class
// This class is used as a way to pass more
// than one argument to a thread.
class ThreadParam
{
public:
    ThreadParam(Shop *shop, int id, int service_time) : shop(shop), id(id), service_time(service_time){};
    Shop *shop;       // A pointer to the Shop object
    int id;           // A thread identifier
    int service_time; // service time (usec) for barber; 0 for customer
};

int main(int argc, char *argv[])
{

    // Read arguments from command line
    // TODO: Validate values
    if (argc != 5)
    {
        cout << "Usage: num_barbers num_chairs num_customers service_time" << endl;
        return -1;
    }
    int num_barbers = atoi(argv[1]);
    int num_chairs = atoi(argv[2]);
    int num_customers = atoi(argv[3]);
    int service_time = atoi(argv[4]);

    // Initializing threads
    // Multiple barbers, one shop, many customers
    pthread_t barber_threads[num_barbers];
    pthread_t customer_threads[num_customers];

    // Initializing shop
    Shop shop(num_barbers, num_chairs);

    // ThreadParam *barber_param = new ThreadParam(&shop, 0, service_time);
    // pthread_create(&barber_thread, NULL, barber, barber_param);

    // Creating barber threads
    for (int i = 0; i < num_barbers; i++)
    {
        ThreadParam *barber_param = new ThreadParam(&shop, i, service_time);
        pthread_create(&barber_threads[i], NULL, barber, barber_param);
    }

    // Creating customer threads
    for (int i = 0; i < num_customers; i++)
    {
        usleep(rand() % 1000);
        ThreadParam *customer_param = new ThreadParam(&shop, i, 0);
        pthread_create(&customer_threads[i], NULL, customer, customer_param);
    }

    // TODO
    // Wait for customers to finish and cancel barber
    for (int i = 0; i < num_customers; i++)
    {
        pthread_join(customer_threads[i], NULL);
    }

    for (int i = 0; i < num_barbers; i++)
    {
        pthread_cancel(barber_threads[i]);
    }

    // ---------------------------
    cout << "# customers who didn't receive a service = " << shop.get_cust_drops() << endl;
    return 0;
}

// Barber thread function
void *barber(void *arg)
{
    // Extract parameters
    ThreadParam *barber_param = (ThreadParam *)arg;
    Shop &shop = *barber_param->shop;
    int id = barber_param->id;
    int service_time = barber_param->service_time;
    delete barber_param;

    // Keeping working until terminated by main
    while (true)
    {
        shop.helloCustomer(id); // Pick up a new customer
        usleep(service_time);
        shop.byeCustomer(id); // Release the customer
    }
}

void *customer(void *arg)
{
    // Extract parameters
    ThreadParam *customer_param = (ThreadParam *)arg;
    Shop &shop = *customer_param->shop;
    int id = customer_param->id;
    delete customer_param;

    int barber = -1;
    if ((barber = shop.visitShop(id)) != -1)
    {
        // If assigned to a barber then wait for service to finish
        shop.leaveShop(id, barber);
    }
}
