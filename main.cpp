#include <thread>
#include "log.h"

#define EXTRA_CODE

#ifdef EXTRA_CODE
#include <iostream>
#include <chrono>
#endif

/*
Example of console output:
23.02.2020 18:06:06; INFO; (1020): Starting the app
23.02.2020 18:06:06; DEBUG; (2561): Running a thread
...
23.02.2020 18:06:06; INFO; (1020): Finishing the app
*/


void f1() {
    auto logger1 = getLogger();
    // output: 23.02.2020 18:06:06; DEBUG; (2561): Running a thread
    logger1(DEBUG) << "Running a thread";

    auto logger2 = getLogger("f1"); // Each message has f1 prefix
    // output: 23.02.2020 18:06:06; INFO; f1(2561): Running a thread
    logger2 << "Running a thread" << " but log to another logger"; // Default log level INFO
}

void f2() {
    Logger logger{ "f2" }; // Each message has f2 prefix
    // output: 23.02.2020 18:06:06; DEBUG; f2(3444): Running a thread
    logger(DEBUG) << "Running a thread";

    const double timeSpent = 10.0;
    // output: 23.02.2020 18:06:06; WARNING; f2(3444): Time spent in the thread: 10.0 seconds
    logger(WARNING) << "Time spent in the thread: " << timeSpent << " seconds";
}

void f3() {
    auto logger = getLogger("f3");  // Each message has f3 prefix
    // output: 23.02.2020 18:06:06; INFO; f3(3444): Running a thread
    logger << "Running a thread"; // Default log level INFO

    const int i = 123;
    // output: 23.02.2020 18:06:06; ERROR; f3(3444): My int is 123
    logger(ERROR) << "My int is " << i;
    // output: 23.02.2020 18:06:06; INFO; f3(3444): This is a random message to test log levels
    logger << "This is a random message " << "to test log levels";
}

int main() {
    auto logger = getLogger();
    // output: 23.02.2020 18:06:06; INFO; (1020): Starting the app
    logger << "Starting the app"; // Default log level INFO

    logger(DEBUG) << "Running the threads";
    std::thread t1(f1);
    std::thread t2(f2);
    std::thread t3(f3);

    t1.join();
    t2.join();
    t3.join();

    logger << "Finishing the app"; // Default log level INFO

#ifdef EXTRA_CODE
    const char* tmp = nullptr;

    logger << 1 << 2 << "some" << '1' << 0x001 << &tmp << std::endl;
    logger(DEBUG) << 1 << 2 << std::endl;

    auto logger2 = getLogger("some");
    logger2 << "text" << "\n";

    logger2 << tmp;
    logger2 << nullptr;

    logger2(ERROR) << tmp;
    logger2(ERROR) << nullptr;

    {
        std::jthread t([&logger2]() {
            {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                std::lock_guard<std::mutex> lock(logger2.getMutex());

                logger2 << "Hi from other thread";
            }
            });

        std::jthread tt([&logger2]() {
            // best way to fix this bug - make setPrefix func is private/protected. When system scaling bugs like this very often can broke logic and hard to find.
            // special for this test case I would like implement mutex logic
            // mutex nececcery only when logger object uses in different threads
#ifdef BASIC_LOGIC
            logger2.setPrefix("other");
            logger2 << "Concurency test";
#else
            {
                std::lock_guard<std::mutex> lock(logger2.getMutex());

                std::string prev_prefix = logger2.getPrefix();
                logger2.setPrefix("other");
                logger2 << "Concurency test";
                logger2.setPrefix(prev_prefix.c_str());
            }
            
#endif // BASIC_LOGIC

            });

    }


    int a = 0;
    std::cin >> a;
#endif // EXTRA_CODE
    
    return 0;
}