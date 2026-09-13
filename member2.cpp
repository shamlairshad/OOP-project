#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <ctime>
#include <exception>

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

    AttendanceStatus status() const
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

class UniversityException : public std::exception
{
protected:
    std::string msg;

public:
    explicit UniversityException(const std::string& message) : msg(message) {}
    const char* what() const noexcept override { return msg.c_str(); }
    virtual ~UniversityException() = default;
};

class SessionClosedException : public UniversityException
{
public:
    explicit SessionClosedException(const std::string& message) : UniversityException(message) {}
};

class DuplicateAttendanceException : public UniversityException
{
public:
    explicit DuplicateAttendanceException(const std::string& message) : UniversityException(message) {}
};

class AttendanceRegister
{
private:
    std::vector<AttendanceRecord> records;
    std::vector<AttendanceCorrection> corrections;
    std::vector<AttendanceSession> sessions;

public:
    AttendanceSession& openSession(const std::string& code, const std::string& /*lecturerId*/, const TimeSlot& slot)
    {
        sessions.emplace_back(code, code, slot);
        AttendanceSession& s = sessions.back();
        s.open();
        return s;
    }

    bool alreadyMarked(const std::string& sessionId, const std::string& studentId) const
    {
        for (const auto& r : records)
        {
            if (r.sessionId() == sessionId && r.studentId() == studentId)
            {
                return true;
            }
        }
        return false;
    }

    void mark(AttendanceSession& s, const std::string& studentId, AttendanceStatus status, const std::string& method)
    {
        if (!s.isOpen())
        {
            throw SessionClosedException("Session '" + s.sessionId() + "' is closed");
        }
        if (alreadyMarked(s.sessionId(), studentId))
        {
            throw DuplicateAttendanceException("Student '" + studentId + "' already marked for this session");
        }

        records.emplace_back(studentId, s.sessionId(), std::time(nullptr), status, method);
    }

    void applyCorrection(const AttendanceCorrection& c)
    {
        corrections.push_back(c); // append-only -- never edits/removes a prior record
    }

    double percentageFor(const std::string& studentId) const
    {
        int total = 0, present = 0;
        for (const auto& r : records)
        {
            if (r.studentId() != studentId) continue;
            ++total;

            AttendanceStatus effective = r.status();
            for (const auto& c : corrections)
            {
                if (c.studentId() == studentId && c.sessionId() == r.sessionId())
                {
                    effective = c.newStatus(); // latest correction wins
                }
            }

            if (effective == AttendanceStatus::PRESENT ||
                effective == AttendanceStatus::LATE ||
                effective == AttendanceStatus::EXCUSED)
            {
                ++present;
            }
        }
        return total == 0 ? 0.0 : (static_cast<double>(present) / total) * 100.0;
    }

    const std::vector<AttendanceRecord>& allRecords() const { return records; }
    const std::vector<AttendanceCorrection>& allCorrections() const { return corrections; }
};

void printAttendanceReport(const AttendanceRegister& reg, const std::vector<std::string>& studentIds)
{
    std::cout << "\n===== Attendance Report =====\n";
    for (const auto& id : studentIds)
    {
        double pct = reg.percentageFor(id);
        std::cout << id << " : " << pct << "% attendance\n";
    }
}

void printAllRecords(const AttendanceRegister& reg)
{
    std::cout << "\n===== All Attendance Records =====\n";
    for (const auto& r : reg.allRecords())
    {
        std::cout << r << "\n"; // uses AttendanceRecord::operator
    }
}

struct CaptureEvent
{
    std::string studentId;
    std::string enteredCode;
    std::time_t timestamp;
};

class AttendanceCapture
{
public:
    virtual void beginSession(const AttendanceSession& s) = 0;

    // Returns true and fills 'outEvent' if there was another event to
    // capture; returns false when there's nothing left (caller stops).
    virtual bool captureNext(CaptureEvent& outEvent) = 0;

    virtual void endSession() = 0;

    virtual ~AttendanceCapture() = default;
};

#include <iostream>

class SessionCodeCapture : public AttendanceCapture
{
private:
    bool sessionActive;

public:
    SessionCodeCapture() : sessionActive(false) {}

    void beginSession(const AttendanceSession& /*s*/) override
    {
        sessionActive = true;
    }

    bool captureNext(CaptureEvent& outEvent) override
    {
        if (!sessionActive) return false;

        std::cout << "  Enter student ID (blank to stop): ";
        std::string studentId;
        std::getline(std::cin, studentId);
        if (studentId.empty())
        {
            return false;
        }

        std::cout << "  Enter session code for " << studentId << ": ";
        std::string code;
        std::getline(std::cin, code);

        outEvent.studentId = studentId;
        outEvent.enteredCode = code;
        outEvent.timestamp = std::time(nullptr);
        return true;
    }

    void endSession() override
    {
        sessionActive = false;
    }
};

