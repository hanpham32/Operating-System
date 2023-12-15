/*
 * Shop.h
 * Header file for Shop.cpp
 *
 * Han Pham
 * CSS 430 B Au 23: Operating Systems
 * Professor R. Dimpsey
 */

#ifndef _SHOP_H_
#define _SHOP_H_
#include <pthread.h>
#include <map>
#include <sstream>
#include <string>
#include <queue>
#include <vector>
#include <iostream>
using namespace std;

#define kDefaultNumChairs 3
#define kDefaultBarbers 1

class Shop
{
public:
    Shop(int num_barbers, int num_chairs);
    Shop();

    int visitShop(int id); // return return barber ID or -1 (not served)
    void leaveShop(int customer_id, int barber_id);
    void helloCustomer(int barber_id);
    void byeCustomer(int barber_id);
    int get_cust_drops() const;

    void addAvailableBarber(int id);
    void removeAvailableBarber();
    int getAvailableBarberId() const;
    void assignBarberToCustomer();
    int getAvailableWaitingChairs() const;
    int getCustomerDrops() const;
    bool isShopFull() const;

private:
    int num_chair;       // number of chairs; the max number of threads that can wait
    int num_barber;      // the number of barber in the shop
    int cust_drops_ = 0; // amount of customers who didn't get service

    enum customersState
    {
        WAIT,   // Waiting for service
        CHAIR,  // Getting service
        LEAVING // Leaving shop
    };

    struct Barber
    {
        int id;
        pthread_cond_t barber_cond; // Condition variable for the barber
        int my_customer = -1;       // // ID of assigned customer; -1 as default
        bool money_paid = false;    // Flag to indicate if the service is paid for
    };

    struct Customer
    {
        int id;
        pthread_cond_t customer_cond;
        int my_barber = -1;          // ID of assigned barber; -1 as default
        customersState state = WAIT; // Current state of customer; WAIT as default
    };

    Barber *barbers;              // Pointer to an array of barbers
    map<int, Customer> customers; // Map to store customers by their IDs

    queue<int> sleeping_barbers_; // Queue of sleeping barbers
    queue<int> waiting_chairs_;   //  Queue of waiting customers

    pthread_mutex_t mutex_;

    Barber *get_barber(int barber_id);
};

#endif