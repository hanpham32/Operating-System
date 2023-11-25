
#include "Shop.h"
#include "driver.cpp"

Shop::Shop(int num_barber, int num_chair)
{
   this->max_barbers_ = num_barber;
   this->max_waiting_cust_ = num_chair;

   pthread_mutex_init(&mutex_, NULL);

   barbers = new Barber[num_barber];
   for (int i = 0; i < num_barber; i++)
   {
      barbers[i].id = i;
      pthread_cond_init(&barbers[i].barberCond, NULL);
   }
}

// string Shop::int2string(int i)
// {
//    stringstream out;
//    out << i;
//    return out.str();
// }

// void Shop::print(int person, string message)
// {
//    if (person > max_barbers_)
//    {
//       cout << "customer[" << person << "]: " << message << endl;
//    }
//    else
//    {
//       cout << "barber[" << person << "]: " << message << endl;
//    }
// }

int Shop::get_cust_drops() const
{
   return cust_drops_;
}

int Shop::visitShop(int customer_id)
{
   // Enter the critical section
   pthread_mutex_lock(&mutex_);

   // If all waiting chairs are full and no sleeping barber available then leave shop
   if (((int)waiting_chairs_.size() == max_waiting_cust_) && (sleeping_barbers_.empty()))
   {
      cout << "Customer[ " << customer_id << "leaves the shop because of no available waiting chairs." << endl;
      cust_drops_++;
      pthread_mutex_unlock(&mutex_); // Leave the critical section
      return -1;                     // Return -1 because not available
   }

   customers[customer_id] = Customer();
   customers[customer_id].id = customer_id;
   pthread_cond_init(&customers[customer_id].customerCond, NULL);

   int barber_id;

   // If all barbers are busy but waiting chairs are available
   if (sleeping_barbers_.empty())
   {
      waiting_chairs_.push(customer_id);
      cout << "Customer[" << customer_id << "]: takes a waiting chair. # waiting seats available = " << getAvailableWaitingChairs() << endl;

      while (customers[customer_id].my_barber = -1)
      {
         pthread_cond_wait(&customers[customer_id].customerCond, &mutex_);
      }
      barber_id = customers[customer_id].my_barber;
      waiting_chairs_.pop();
   }
   else
   {
      barber_id = sleeping_barbers_.front();
      sleeping_barbers_.pop();
      customers[customer_id].my_barber = barber_id;
      get_barber(barber_id)->my_customer = customer_id;
   }

   cout << "Customer[" << customer_id << "]: moves to a service chair " << barber_id << ", # waiting seats available = " << getAvailableWaitingChairs() << endl;

   customers[customer_id].state = CHAIR;

   // wake up the barber just in case if he is sleeping
   pthread_cond_signal(&(get_barber(barber_id)->barberCond));

   pthread_mutex_unlock(&mutex_);
   return barber_id;
}

void Shop::leaveShop(int customer_id, int barber_id)
{
   pthread_mutex_lock(&mutex_);

   // Wait for service to be completed
   cout << "Customer[" << customer_id << "]: wait for barber " << barber_id << " hair-cut to be done" << endl;

   while (customers[customer_id].my_barber = -1)
   {
      pthread_cond_wait(&customers[customer_id].customerCond, &mutex_);
   }

   // Pay the barber and signal barber appropriately
   get_barber(barber_id)->money_paid = true;

   pthread_cond_signal(&(get_barber(barber_id)->barberCond));
   cout << "Customer[" << customer_id << "]: says good-bye to barber " << barber_id << "." << endl;
   pthread_mutex_unlock(&mutex_);
}

void Shop::helloCustomer(int barber_id)
{
   pthread_mutex_lock(&mutex_);

   // If no customers then barber can sleep
   if (get_barber(barber_id)->my_customer = -1)
   {
      cout << "Barber[" << barber_id << "]: sleeps because of no customers." << endl;
      sleeping_barbers_.push(barber_id);
      while (get_barber(barber_id)->my_customer = -1)
      {
         pthread_cond_wait(&(get_barber(barber_id)->barberCond), &mutex_);
      }
   }

   while (customers[get_barber(barber_id)->my_customer].state != CHAIR)
   {
      pthread_cond_wait(&(get_barber(barber_id)->barberCond), &mutex_);
   }

   cout << "Barber[" << barber_id << "]: starts a hair-cut service for customer " << get_barber(barber_id)->my_customer << endl;
   pthread_mutex_unlock(&mutex_);
}

void Shop::byeCustomer(int barber_id)
{
   // Hair Cut-Service is done so signal customer and wait for payment
   cout << "Barber[" << barber_id << "]: says he's done with a hair-cut service for customer " << get_barber(barber_id)->my_customer << "." << endl;

   get_barber(barber_id)->money_paid = false;
   customers[get_barber(barber_id)->my_customer].my_barber = -1;

   pthread_cond_signal(&customers[get_barber(barber_id)->my_customer].customerCond);

   // Signal to customer to get next one
   while (customers[get_barber(barber_id)->my_customer].state != LEAVING)
   {
      pthread_cond_wait(&(get_barber(barber_id)->barberCond), &mutex_);
   }

   get_barber(barber_id)->my_customer = -1;

   cout << "Barber[" << barber_id << "]: calls in another customer." << endl;
   if (!waiting_chairs_.empty())
   {
      int customer_id = waiting_chairs_.front();
      waiting_chairs_.pop();
      get_barber(barber_id)->my_customer = customer_id;
      customers[customer_id].my_barber = barber_id;
      pthread_cond_signal(&customers[customer_id].customerCond);
   }

   pthread_mutex_unlock(&mutex_); // unlock
}

Shop::Barber *Shop::get_barber(int barber_id) const
{
   for (int i = 0; i < max_barbers_; i++)
   {
      if (barbers[i].id == barber_id)
      {
         return &barbers[i];
      }
   }
   return NULL;
}

int Shop::getAvailableWaitingChairs() const
{
   return max_waiting_cust_ - waiting_chairs_.size();
}
