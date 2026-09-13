#include <string>
#include <ostream>
#include <vector>

enum class Day : int { MON = 0, TUE, WED, THU, FRI, SAT, SUN };

class TimeSlot
{
private:
    Day day;
    int startMin;
    int endMin;
    std::string location;

public:
    TimeSlot(Day dayIn, int startMinIn, int endMinIn, const std::string& locationIn)
        : day(dayIn), startMin(startMinIn), endMin(endMinIn), location(locationIn) {}

    bool operator==(const TimeSlot& o) const
    {
        return day == o.day && startMin == o.startMin && endMin == o.endMin && location == o.location;
    }

    bool operator<(const TimeSlot& o) const
    {
        if (day != o.day) return day < o.day;
        if (startMin != o.startMin) return startMin < o.startMin;
        if (endMin != o.endMin) return endMin < o.endMin;
        return location < o.location;
    }

    bool overlaps(const TimeSlot& o) const
    {
        if (day != o.day) 
            return false;
        return startMin < o.endMin && o.startMin < endMin;
    }

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
        static const std::string names[] = { "MON","TUE","WED","THU","FRI","SAT","SUN" };

        int startH = t.startMin / 60, startM = t.startMin % 60;
        int endH = t.endMin / 60, endM = t.endMin % 60;

        os << names[static_cast<int>(t.day)] << " ";
        if (startH < 10) os << "0";
        os << startH << ":";
        if (startM < 10) os << "0";
        os << startM << "-";
        if (endH < 10) os << "0";
        os << endH << ":";
        if (endM < 10) os << "0";
        os << endM;
        os << " @ " << t.location;
        return os;
    }
};

class Timetable
{
private:
    std::vector<TimeSlot> slots;

public:
    void addSlot(const TimeSlot& s)
    {
        slots.push_back(s);
    }

    Timetable& operator+=(const TimeSlot& s)
    {
        addSlot(s);
        return *this;
    }

    bool clashesWith(const Timetable& other) const
    {
        for (const auto& mine : slots)
            for (const auto& theirs : other.slots)
                if (mine && theirs) return true; // uses TimeSlot::operator&&
        return false;
    }

    const std::vector<TimeSlot>& getSlots() const
    {
        return slots; 
    }
};