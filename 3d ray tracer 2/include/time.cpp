#include <chrono>

using namespace std;

int now(){
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return chrono::duration_cast<chrono::nanoseconds>(duration).count();
}
