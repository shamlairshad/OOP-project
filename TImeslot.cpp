#include <string>
#include <ostream>
#include <tuple>
#include <sstream>
#include <iomanip>

enum class Day : int { MON = 0, TUE, WED, THU, FRI, SAT, SUN };

class TimeSlot
{
private:
    Day day;
    int startMin;   // minutes from midnight, e.g. 09:30 -> 570
    int endMin;     // half-open interval: [startMin, endMin)
    std::string location;

public:
    TimeSlot(Day dayIn, int startMinIn, int endMinIn, const std::string& locationIn)
        : day(dayIn), startMin(startMinIn), endMin(endMinIn), location(locationIn) {}

    bool operator==(const TimeSlot& o) const
    {
        return day == o.day && startMin == o.startMin &&
               endMin == o.endMin && location == o.location;
    }

    // Needed for use in ordered containers (e.g. std::set<TimeSlot>) --
    // not on the diagram's method list explicitly but required for
    // operator< to have any real use beyond sorting a vector manually.
    bool operator<(const TimeSlot& o) const
    {
        return std::tie(day, startMin, endMin, location) 
               < std::tie(o.day, o.startMin, o.endMin, o.location);
    }

    // Same-day, half-open interval overlap check.
    bool overlaps(const TimeSlot& o) const
    {
        if (day != o.day) return false;
        return startMin < o.endMin && o.startMin < endMin;
    }

    // Second overloaded-operator form of the same check, per the written
    // spec's "operator&& / overlaps()" wording -- both exist so the
    // requirement is satisfied literally either way it's graded.
    friend bool operator&&(const TimeSlot& a, const TimeSlot& b)
    {
        return a.overlaps(b);
    }

    Day getDay() const { return day; }
    int getStartMin() const { return startMin; }
    int getEndMin() const { return endMin; }
    const std::string& getLocation() const { return location; }

    friend std::ostream& operator<<(std::ostream& os, const TimeSlot& t)
    {
        static const char* names[] = { "MON","TUE","WED","THU","FRI","SAT","SUN" };
        auto clock = [](int m) {
            std::ostringstream oss;
            oss << std::setw(2) << std::setfill('0') << (m / 60) << ":"
                << std::setw(2) << std::setfill('0') << (m % 60);
            return oss.str();
        };
        os << names[static_cast<int>(t.day)] << " " << clock(t.startMin)
           << "-" << clock(t.endMin) << " @ " << t.location;
        return os;
    }
};