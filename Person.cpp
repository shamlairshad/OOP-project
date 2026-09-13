#include <iostream>
#include <string>
#include <vector>
#include <set>

class Course;

//base class person
class Person
{
protected:
    std::string userID;
    std::string name;
    std::string email;
    std::string passwordHash;

public:
    Person(const std::string& userIDIn, const std::string& nameIn, const std::string& emailIn, const std::string& passwordHashIn)
        : userID(userIDIn), name(nameIn), email(emailIn), passwordHash(passwordHashIn) {}

    bool authenticate(const std::string& pw)const
    {
        return hash(pw) == passwordHash;
    }

    virtual std::string role() const = 0;

    virtual void displayDashboard() const = 0;

    static std::string hash(const std::string& pw) {
        unsigned long hashValue = 5381;
        for (char c : pw) {
            hashValue = ((hashValue << 5) + hashValue) + c; // hash * 33 + c
        }
        return std::to_string(hashValue);
    }

    const std::string& getId() const 
    { 
        return userID; 
    }

    const std::string& getName() const 
    { 
        return name; 
    }

    virtual ~Person() = default;
};

//Derived class student
class Student : public Person
{
private:
    std::vector<std::string> enrolledCourses;
    std::set<std::string> completedCourses;
    std::string activeSessionCode;

public:
    Student(const std::string& userIDIn, const std::string& nameIn, const std::string& emailIn, const std::string& passwordHashIn) 
        : Person(userIDIn, nameIn, emailIn, passwordHashIn) {}

    std::string role() const override
    {
        return "Student";
    }

    void displayDashboard() const override
    {
        std::cout << "\n===== STUDENT DASHBOARD =====\n";
        std::cout << "1. View Courses\n";
        std::cout << "2. Enroll in Course\n";
        std::cout << "3. Drop Course\n";
        std::cout << "4. View Timetable\n";
        std::cout << "5. View Attendance\n";
        std::cout << "6. Enter Session Code\n";
    }

    bool hasCompleted(const std::string& code) const
    {
        return completedCourses.find(code) != completedCourses.end();
    }

    void setActiveSessionCode(const std::string& code) 
    { 
        activeSessionCode = code; 
    }

    bool enterSessionCode(const std::string& code) const
    {
        if (code.empty()) 
            return false;
        return code == activeSessionCode;
    }

    void viewTimetable() const
    {
        std::cout << "\n===== MY TIMETABLE =====\n";
        // timetable display logic will be connected later
    }

    void addEnrolledCourse(const std::string& code) 
    { 
        enrolledCourses.push_back(code); 
    }

    void markCompleted(const std::string& code) 
    { 
        completedCourses.insert(code); 
    }

    friend std::ostream& operator<<(std::ostream& os, const Student& s)
    {
        os << "Student ID: " << s.userID << '\n';
        os << "Name: " << s.name << '\n';
        os << "Email: " << s.email << '\n';

        return os;
    }
};

//Derived class Lecturer
class Lecturer: public Person
{
private:
    std::vector<std::string> assignedCourses;

public:
    Lecturer(const std::string& userIDIn, const std::string& nameIn, const std::string& emailIn, const std::string& passwordHashIn)
        : Person(userIDIn, nameIn, emailIn, passwordHashIn) {}
    
    std::string role() const override
    {
        return "Lecturer";
    }

    void displayDashboard() const override
    {
        std::cout << "\n===== LECTURER DASHBOARD =====\n";
        std::cout << "1. View Enrollment List\n";
        std::cout << "2. Record Class Attendance\n";
        std::cout << "3. Manage Course Offering\n";
        std::cout << "4. Manage Timetable\n";
        std::cout << "5. View Attendance\n";
        std::cout << "6. Logout\n";
    }

    bool teaches(const std::string& code) const
    {
        for (const std::string& courseCode : assignedCourses)
        {
            if (courseCode == code)
            {
                return true;
            }
        }

        return false;
    }

    void assignCourse(const std::string& code) 
    {
        assignedCourses.push_back(code); 
    }

    std::vector<std::string> viewEnrollmentList(const Course* course)const;

};
//Derived class Administrator
class Administrator: public Person
{
public:
    Administrator(const std::string& userIDIn, const std::string& nameIn, const std::string& emailIn, const std::string& passwordHashIn)
        : Person(userIDIn, nameIn, emailIn, passwordHashIn) {}

    std::string role() const override
    {
        return "Administrator";
    }

    void displayDashboard() const override
    {
        std::cout << "\n===== ADMINISTRATOR DASHBOARD =====\n";
        std::cout << "1. Manage Users\n";
        std::cout << "2. Manage Courses\n";
        std::cout << "3. Assign Lecturers\n";
        std::cout << "4. Manage Timetable\n";
        std::cout << "5. View Attendance Reports\n";
        std::cout << "6. Logout\n";
    }
};

class Course
{
private:
    std::string code;
    std::vector<std::string> enrolledStudents;

public:
    explicit Course(const std::string& codeIn) : code(codeIn) {}

    const std::string& getCode() const 
    { 
        return code; 
    }

    std::vector<std::string> getEnrolledStudents() const 
    { 
        return enrolledStudents; 
    }

    void addStudent(const std::string& id) 
    { 
        enrolledStudents.push_back(id); 
    }
};

std::vector<std::string> Lecturer::viewEnrollmentList(const Course* course) const
{
    if (course == nullptr) 
        return {};
    return course->getEnrolledStudents();
}