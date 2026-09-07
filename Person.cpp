#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <functional>
using namespace std;

class Course;

//base class person
class Person
{
protected:
    string userID;
    string name;
    string email;
    string passwordHash;

public:
    Person(const string& userIDIn, const string& nameIn, const string& emailIn, const string& passwordHashIn): userID(userIDIn), name(nameIn), email(emailIn), passwordHash(passwordHashIn) {}

    bool authenticate(const string& pw)const
    {
        return hash(pw) == passwordHash;
    }

    virtual string role() const = 0;

    virtual void displayDashboard() const = 0;

    static string hash(const string& pw)
    {
        return to_string(std::hash<string>{}(pw));
    }

    const string& getId() const 
    { 
        return userID; 
    }

    const string& getName() const 
    { 
        return name; 
    }

    virtual ~Person() = default;
};

//Derived class student
class Student : public Person
{
private:
    vector<string> enrolledCourses;
    set<string> completedCourses;
    string activeSessionCode;

public:
    Student(const string& userIDIn, const string& nameIn, const string& emailIn, const string& passwordHashIn): Person(userIDIn, nameIn, emailIn, passwordHashIn) {}

    string role() const override
    {
        return "Student";
    }

    void displayDashboard() const override
    {
        cout << "\n===== STUDENT DASHBOARD =====\n";
        cout << "1. View Courses\n";
        cout << "2. Enroll in Course\n";
        cout << "3. Drop Course\n";
        cout << "4. View Timetable\n";
        cout << "5. View Attendance\n";
        cout << "6. Enter Session Code\n";
    }

    bool hasCompleted(const string& code) const
    {
        return completedCourses.find(code) != completedCourses.end();
    }

    void setActiveSessionCode(const string& code) 
    { 
        activeSessionCode = code; 
    }

    bool enterSessionCode(const string& code) const
    {
        if (code.empty()) 
            return false;
        return code == activeSessionCode;
    }

    void viewTimetable() const
    {
        cout << "\n===== MY TIMETABLE =====\n";
        // timetable display logic will be connected later
    }

    void addEnrolledCourse(const string& code) 
    { 
        enrolledCourses.push_back(code); 
    }

    void markCompleted(const string& code) 
    { 
        completedCourses.insert(code); 
    }

    friend ostream& operator<<(ostream& os, const Student& s)
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
    vector<string> assignedCourses;

public:
    Lecturer(const string& userIDIn, const string& nameIn, const string& emailIn, const string& passwordHashIn): Person(userIDIn, nameIn, emailIn, passwordHashIn) {}
    
    string role() const override
    {
        return "Lecturer";
    }

    void displayDashboard() const override
    {
        cout << "\n===== LECTURER DASHBOARD =====\n";
        cout << "1. View Enrollment List\n";
        cout << "2. Record Class Attendance\n";
        cout << "3. Manage Course Offering\n";
        cout << "4. Manage Timetable\n";
        cout << "5. View Attendance\n";
        cout << "6. Logout\n";
    }

    bool teaches(const string& code) const
    {
        for (const string& courseCode : assignedCourses)
        {
            if (courseCode == code)
            {
                return true;
            }
        }

        return false;
    }

    void assignCourse(const string& code) 
    {
        assignedCourses.push_back(code); 
    }

    vector<string> viewEnrollmentList(const Course* course)const;

};
//Derived class Administrator
class Administrator: public Person
{
public:
    Administrator(const string& userIDIn, const string& nameIn, const string& emailIn, const string& passwordHashIn): Person(userIDIn, nameIn, emailIn, passwordHashIn) {}
    
    string role() const override
    {
        return "Administrator";
    }

    void displayDashboard() const override
    {
        cout << "\n===== ADMINISTRATOR DASHBOARD =====\n";
        cout << "1. Manage Users\n";
        cout << "2. Manage Courses\n";
        cout << "3. Assign Lecturers\n";
        cout << "4. Manage Timetable\n";
        cout << "5. View Attendance Reports\n";
        cout << "6. Logout\n";
    }
};

class Course
{
private:
    string code;
    vector<string> enrolledStudents;

public:
    explicit Course(const string& codeIn) : code(codeIn) {}

    const string& getCode() const 
    { 
        return code; 
    }

    vector<string> getEnrolledStudents() const 
    { 
        return enrolledStudents; 
    }

    void addStudent(const string& id) 
    { 
        enrolledStudents.push_back(id); 
    }
};

vector<string> Lecturer::viewEnrollmentList(const Course* course) const
{
    if (course == nullptr) 
        return {};
    return course->getEnrolledStudents();
}