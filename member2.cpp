#include <string>
#include <ostream>
#include <vector>
#include <ctime>

enum class Day : int { MON = 0, TUE, WED, THU, FRI, SAT, SUN };

enum class AttendanceStatus : int { PRESENT = 0, ABSENT, LATE, EXCUSED };

std::string statusToString(AttendanceStatus s)
{
    switch (s)
    {
        case AttendanceStatus::PRESENT: 
            return "PRESENT";
        case AttendanceStatus::ABSENT:  
            return "ABSENT";
        case AttendanceStatus::LATE:    
            return "LATE";
        case AttendanceStatus::EXCUSED: 
            return "EXCUSED";
    }
    return "ABSENT";
}

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

class AttendanceRecord
{
private:
    static unsigned nextRecordId;

    unsigned recordId_;
    std::string studentId_;
    std::string sessionId_;
    std::time_t timestamp_;
    AttendanceStatus status_;
    std::string captureMethod_;

public:
    AttendanceRecord(const std::string& studentIdIn, const std::string& sessionIdIn, std::time_t timestampIn, AttendanceStatus statusIn, const std::string& captureMethodIn)
        : recordId_(nextRecordId++), studentId_(studentIdIn), sessionId_(sessionIdIn), timestamp_(timestampIn), status_(statusIn), captureMethod_(captureMethodIn) {}

    unsigned recordId() const 
    {
        return recordId_;
    }

    AttendanceStatus& status() 
    { 
        return status_; 
    }
    
    const std::string& studentId() const
    { 
        return studentId_; 
    }

    const std::string& sessionId() const
    { 
        return sessionId_; 
    }

    std::time_t timestamp() const 
    { 
        return timestamp_;
    }

    const std::string& captureMethod() const 
    { 
        return captureMethod_; 
    }

    friend std::ostream& operator<<(std::ostream& os, const AttendanceRecord& r)
    {
        os << "Record#" << r.recordId_ << " [" << r.studentId_ << "/" << r.sessionId_ << "] " << statusToString(r.status_) << " via " << r.captureMethod_;
        return os;
    }
};

unsigned AttendanceRecord::nextRecordId = 1;

class AttendanceCorrection
{
private:
    std::string studentId_;
    std::string sessionId_;
    std::string actingLecturerId_;
    std::string reason_;
    AttendanceStatus newStatus_;

public:
    AttendanceCorrection(const std::string& studentIdIn, const std::string& sessionIdIn, const std::string& actingLecturerIdIn, const std::string& reasonIn, AttendanceStatus newStatusIn)
        : studentId_(studentIdIn), sessionId_(sessionIdIn), actingLecturerId_(actingLecturerIdIn), reason_(reasonIn), newStatus_(newStatusIn) {}

    const std::string& reason() const  
    { 
        return reason_; 
    }
    const std::string& studentId() const 
    { 
        return studentId_; 
    }
    const std::string& sessionId() const 
    { 
        return sessionId_; 
    }
    const std::string& actingLecturerId() const 
    { 
        return actingLecturerId_; 
    }
    AttendanceStatus newStatus() const 
    { 
        return newStatus_; 
    }
};

class SessionCode
{
private:
    std::string code;
    bool active = false;

public:
    void generate() { code = "PLACEHOLDER"; active = true; }
    bool validate(const std::string& c) const { return active && c == code; }
    std::string value() const { return code; }
    void deactivate() { active = false; }
};

class AttendanceSession
{
private:
    std::string sessionId_;
    std::string courseCode_;
    TimeSlot slot_;
    std::time_t openTime_;
    bool open_;
    SessionCode sessionCode_;

public:
    static const int DEFAULT_DURATION_MIN;

    AttendanceSession(const std::string& sessionIdIn, const std::string& courseCodeIn, const TimeSlot& slotIn)
        : sessionId_(sessionIdIn), courseCode_(courseCodeIn), slot_(slotIn), openTime_(0), open_(false) {}

    void open()
    {
        openTime_ = std::time(nullptr);
        open_ = true;
        sessionCode_.generate();
    }

    void close()
    {
        open_ = false;
        sessionCode_.deactivate();
    }

    bool isOpen() const 
    { 
        return open_; 
    }

    const std::string& sessionId() const 
    { 
        return sessionId_; 
    }
    const std::string& courseCode() const 
    { 
        return courseCode_; 
    }
    const TimeSlot& slot() const 
    { 
        return slot_; 
    }
    SessionCode& sessionCode() 
    { 
        return sessionCode_; 
    }
    const SessionCode& sessionCode() const 
    { 
        return sessionCode_; 
    }
};

const int AttendanceSession::DEFAULT_DURATION_MIN = 15;