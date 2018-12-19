

#include <iostream>
#include <chrono>
#include <thread>
#include <condition_variable>

// These could be member variables if you have a class.
std::condition_variable g_terminate;
std::mutex g_termiante_mutex;
bool g_terminate_value;

// Based on https://stackoverflow.com/questions/21057676/need-to-call-a-function-at-periodic-time-intervals-in-c/21058232
// and http://www.cplusplus.com/reference/condition_variable/condition_variable/wait_for/
// and https://stackoverflow.com/questions/41394670/c-condition-variable-wait-for-returns-instantly

void do_something()
{
  std::cout << "I am doing something" << std::endl;
}

bool condition_check()
{
  return g_terminate_value;
}

void timer_start(unsigned int interval)
{
  std::thread([interval]()
  {
    std::unique_lock<std::mutex> lk(g_termiante_mutex);
    while (g_terminate.wait_for(lk, std::chrono::seconds(interval), condition_check) == false)
    {
      do_something();
    }
    std::cout << "Finished\n";
  }).detach();
}

int main()
{
  std::cout << "Periodic processing will start after you hit enter.  Hit enter again to stop the program.\n";
  std::cin.ignore();

  timer_start(1);

  std::cout << "Processing is started\n";
  std::cin.ignore();

  // Terminate periodic processing.
  {
    std::unique_lock<std::mutex> lk(g_termiante_mutex);
    g_terminate_value = true;
  }
  g_terminate.notify_one();

}


