/*
 * Shop.cpp
 * Implementation of the Shop class for simulating a barber shop environment using threads.
 *
 * Han Pham
 * CSS 430 B Au 23: Operating Systems
 * Professor R. Dimpsey
 */

#include "Shop.h"

Shop::Shop(int num_barber, int num_chair)
{
    this->num_barber = num_barber;
    this->num_chair = num_chair;

    pthread_mutex_init(&mutex_, NULL);

    barbers = new Barber[num_barber];
    for (int i = 0; i < num_barber; i++)
    {
        barbers[i].id = i;
        pthread_cond_init(&barbers[i].barber_cond, NULL);
    }
}

Shop::Shop()
{
    this->num_barber = kDefaultBarbers;
    this->num_chair = kDefaultNumChairs;

    pthread_mutex_init(&mutex_, NULL);

    barbers = new Barber[num_barber];
    for (int i = 0; i < num_barber; i++)
    {
        barbers[i].id = i;
        pthread_cond_init(&barbers[i].barber_cond, NULL);
    }
}

int Shop::visitShop(int customer_id)
{
    pthread_mutex_lock(&mutex_);

    // Check if the shop is full
    if (isShopFull())
    {
        cout << "Customer[" << customer_id << "]: leaves the shop because of no available waiting chairs." << endl;
        cust_drops_++;
        pthread_mutex_unlock(&mutex_);
        return -1;
    }

    // Initialize customers details
    customers[customer_id] = Customer();
    customers[customer_id].id = customer_id;

    pthread_cond_init(&customers[customer_id].customer_cond, NULL);

    int barber_id;

    // Assign the customer to a waiting chair if all barbers are busy
    if (sleeping_barbers_.empty())
    {
        waiting_chairs_.push(customer_id);
        cout << "Customer[" << customer_id << "]: takes a waiting chair. # waiting seats available = " << getAvailableWaitingChairs() << endl;

        while (customers[customer_id].my_barber == -1)
        {
            pthread_cond_wait(&customers[customer_id].customer_cond, &mutex_);
        }
        barber_id = customers[customer_id].my_barber;
    }
    else // A barber is available for service, assign barber to customer
    {
        barber_id = sleeping_barbers_.front();            // Get the ID of the first sleeping barber
        sleeping_barbers_.pop();                          // Remove the barber from the sleeping queue
        customers[customer_id].my_barber = barber_id;     // Assign the barber to the customer
        get_barber(barber_id)->my_customer = customer_id; // Assign the customer to the barber
    }

    cout << "Customer[" << customer_id << "]: moves to a service chair " << barber_id << ", # waiting seats available = " << getAvailableWaitingChairs() << endl;

    // Update the customer state
    customers[customer_id].state = CHAIR;

    // Signal barber
    pthread_cond_signal(&(get_barber(barber_id)->barber_cond));

    pthread_mutex_unlock(&mutex_);

    return barber_id;
}

void Shop::leaveShop(int customer_id, int barber_id)
{
    pthread_mutex_lock(&mutex_);

    // Wait for service to be completed
    cout << "Customer[" << customer_id << "]: wait for barber " << barber_id << " hair-cut to be done" << endl;

    // Wait until the barber signals that the haircut is done
    while (customers[customer_id].my_barber != -1)
    {
        pthread_cond_wait(&customers[customer_id].customer_cond, &mutex_);
    }

    cout << "Customer[" << customer_id << "]: says good-bye to barber " << barber_id << "." << endl;

    customers[customer_id].state = LEAVING;
    pthread_cond_signal(&(get_barber(barber_id)->barber_cond));

    pthread_mutex_unlock(&mutex_);
}

void Shop::helloCustomer(int barber_id)
{
    pthread_mutex_lock(&mutex_);

    // Check if the barber does not have a customer
    if (get_barber(barber_id)->my_customer == -1)
    {
        cout << "Barber[" << barber_id << "]: sleeps because of no customers." << endl;
        sleeping_barbers_.push(barber_id);               // Add the barber to the queue of sleeping barbers
        while (get_barber(barber_id)->my_customer == -1) // Wait until a customer is assigned to this barber
        {
            pthread_cond_wait(&(get_barber(barber_id)->barber_cond), &mutex_);
        }
    }

    // Wait until the customer is seated in the chair
    while (customers[get_barber(barber_id)->my_customer].state != CHAIR)
    {
        pthread_cond_wait(&(get_barber(barber_id)->barber_cond), &mutex_);
    }

    cout << "Barber[" << barber_id << "]: starts a hair-cut service for customer " << get_barber(barber_id)->my_customer << endl;

    pthread_mutex_unlock(&mutex_);
}

void Shop::byeCustomer(int barber_id)
{
    pthread_mutex_lock(&mutex_);
    cout << "Barber[" << barber_id << "]: says he's done with a hair-cut service for customer " << get_barber(barber_id)->my_customer << "." << endl;

    customers[get_barber(barber_id)->my_customer].my_barber = -1;
    pthread_cond_signal(&customers[get_barber(barber_id)->my_customer].customer_cond);

    // Wait until the customer leaves the chair
    while (customers[get_barber(barber_id)->my_customer].state != LEAVING)
    {
        pthread_cond_wait(&(get_barber(barber_id)->barber_cond), &mutex_);
    }

    // Reset barber availability
    get_barber(barber_id)->my_customer = -1;

    cout << "Barber[" << barber_id << "]: calls in another customer." << endl;

    if (!waiting_chairs_.empty())
    {
        // There are customers in queue, Barber prepares to be in service
        int customer_id = waiting_chairs_.front();
        waiting_chairs_.pop();
        get_barber(barber_id)->my_customer = customer_id;
        customers[customer_id].my_barber = barber_id; //?
        pthread_cond_signal(&customers[customer_id].customer_cond);
    }

    pthread_mutex_unlock(&mutex_);
}

// Helper function to retrieve a barber based on their ID
Shop::Barber *Shop::get_barber(int barber_id)
{
    for (int i = 0; i < num_barber; i++)
    {
        if (barbers[i].id == barber_id)
        {
            return &barbers[i];
        }
    }
    return NULL;
}

// Helper function to get the number of available waiting chairs in the shop
int Shop::getAvailableWaitingChairs() const
{
    return num_chair - waiting_chairs_.size();
}

// Helper function to get the number of customers who left without getting serviced
int Shop::getCustomerDrops() const
{
    return cust_drops_;
}

// Helper function to check if the shop is full (no waiting chairs and no sleeping barbers)
bool Shop::isShopFull() const
{
    return ((int)waiting_chairs_.size() == num_chair) && (sleeping_barbers_.empty());
}