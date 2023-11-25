#ifndef SHOP_H_
#define SHOP_H_
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>
#include <vector>
#include <map>
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

private:
   int max_waiting_cust_; // number of chairs; the max number of threads that can wait
   int max_barbers_;      // the number of barber in the shop

   enum customersState
   {
      WAIT,
      CHAIR,
      LEAVING
   };

   struct Barber
   {
      int id;
      pthread_cond_t barberCond;
      int my_customer = -1; // no assigned customer
      bool money_paid = false;
   };

   struct Customer
   {
      int id;
      pthread_cond_t customerCond;
      int my_barber = -1; // no assigned barber
      customersState state = WAIT;
   };

   Barber *barbers;
   map<int, Customer> customers;

   queue<int> sleeping_barbers_; // queue of sleeping barbers
   queue<int> waiting_chairs_;   //  queue of waiting customers

   int cust_drops_;

   pthread_mutex_t mutex_;

   Barber *get_barber(int barber_id) const;

   // string int2string(int i);
   // void print(int person, string message);
};
#endif
