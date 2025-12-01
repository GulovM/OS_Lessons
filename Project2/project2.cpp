#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <ctime>

std::mutex logMutex;

// Logging
void logEvent(const std::string& msg) {
    std::lock_guard<std::mutex> lock(logMutex);

    auto now = std::chrono::system_clock::now();
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    std::tm* local = std::localtime(&timeNow);

    std::cout << "[" << std::put_time(local, "%Y-%m-%d %H:%M:%S") << "] "
              << msg << std::endl;
}

struct Station {
    std::string name;
    std::mutex mutexStation;

    Station(const std::string& n) : name(n) {}
};

class Train {
public:
    Train(int id, const std::vector<Station*>& route)
        : trainID(id), route(route) {}

    void operator()() {
        // Forward direction
        for (size_t i = 0; i < route.size(); i++) {
            moveToStation(i);
        }

        // Backward direction
        for (int i = (int)route.size() - 2; i >= 0; i--) {
            moveToStation(i);
        }

        logEvent("Train #" + std::to_string(trainID) + " has completed its round trip.");
    }

private:
    int trainID;
    const std::vector<Station*>& route;

    void moveToStation(size_t index) {
        if (index > 0)
            std::this_thread::sleep_for(std::chrono::seconds(7));

        Station* st = route[index];

        std::unique_lock<std::mutex> lock(st->mutexStation);

        logEvent("Train #" + std::to_string(trainID) +
                 " arrived at station " + st->name);

        std::this_thread::sleep_for(std::chrono::seconds(3));

        logEvent("Train #" + std::to_string(trainID) +
                 " departed from station " + st->name);
    }
};

int main() {

    // Stations stored as pointers
    std::vector<Station*> stations;
    stations.push_back(new Station("Dushanbe"));
    stations.push_back(new Station("Vahdat"));
    stations.push_back(new Station("Fayzobod"));
    stations.push_back(new Station("Obigarm"));
    stations.push_back(new Station("Nurobod"));
    stations.push_back(new Station("Dangara"));

    std::vector<std::vector<Station*>> trainRoutes;

    // First 4 trains: Dushanbe -> Dangara
    for (int i = 0; i < 4; i++) {
        std::vector<Station*> route;
        for (size_t s = 0; s < stations.size(); s++)
            route.push_back(stations[s]);
        trainRoutes.push_back(route);
    }

    // Next 4 trains: Dangara -> Dushanbe
    for (int i = 0; i < 4; i++) {
        std::vector<Station*> route;
        for (int s = (int)stations.size() - 1; s >= 0; s--)
            route.push_back(stations[s]);
        trainRoutes.push_back(route);
    }

    std::vector<std::thread> threads;
    for (int i = 0; i < 8; i++) {
        threads.emplace_back(Train(i + 1, trainRoutes[i]));
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    for (auto& t : threads)
        t.join();

    return 0;
}
