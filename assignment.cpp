#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <bitset>
#include <ctime>
#include <unordered_map>

using namespace std;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <conio.h>
#include <cstdlib>

char input() {
    return getch();
}

void clear() {
    system("cls");
}

#else
#include <termios.h>
#include <unistd.h>

char input() {
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

void clear() {
    cout << "\033[2J\033[1;1H";
}

#endif

constexpr array MONTH_NAMES = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
};

constexpr array WEEKDAY_NAMES = {
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
};

enum State {
    MENU,
    ABOUT_US,
    CUSTOMER,
    STAFF,
    EXIT,
    SERVICE_LIST,
    SERVICE_1,
    SERVICE_2,
    SERVICE_3,
    EXPERT_LIST,
    BOOKING,
    BOOK_VIEW,
    BOOK_VIEW_PREV_MONTH,
    BOOK_VIEW_NEXT_MONTH,
    BOOK_SELECT_DAY,
    BOOK_SELECT_SERVICE_TYPE,
    BOOK_TREATMENT,
    BOOK_CONSULTATION,
    BOOK_SELECT_EXPERT,
    BOOK_SELECT_SERVICE,
    BOOK_SELECT_SERVICE_1,
    BOOK_SELECT_SERVICE_2,
    BOOK_SELECT_SERVICE_3,
    BOOK_CONFIRM,
    BOOK_PAYMENT,
    BOOK_SUCCESS,
    BOOK_CANCEL,
    APPOINTMENTS,
    LOGIN,
    STAFF_MENU,
    LOGOUT,
    TREATMENT_SCHEDULE,
    CONSULTATION_SCHEDULE,
    CUSTOMER_DETAILS,
    SALES_REPORT,
};

typedef struct {
    string name;
    string description;
} Service;

const Service SERVICES[] = {
    {"Skin Care", "blablabla"},
    {"Face Makeup", "lorem ipsum"},
    {"Hair Coloring", "foo bar baz"},
};

typedef struct {
    string name;
    string username;
    string password;
} Staff;

const array EXPERTS = {
    Staff { "Ang Zi En", "aze", "12345678" },
    Staff { "Wilson Ang Shao En", "wase", "12345678" },
    Staff { "Wai Chee Han", "wch", "12345678" },
};

const string *find_longest_expert_name() {
    const string *p = nullptr;
    
    for (const auto &s : EXPERTS) {
        if (p == nullptr || s.name.size() > p->size()) {
            p = &s.name;
        }
    }
    
    return p;
}

const auto LONGEST_EXPERT_NAME_LENGTH = find_longest_expert_name()->size();

const Staff ADMIN = { "Administrator", "admin", "12345678" };
constexpr auto WORK_HOURS = 6;
constexpr auto OPENING_HOUR = 12;

typedef struct {
    bool is_treatment;
    int service;
    int expert;
    struct tm time;
} Booking;

static unordered_map<time_t, bitset<WORK_HOURS * EXPERTS.size()>> schedule = {};
static unordered_map<string, Booking> bookings = {};
static const Staff *login_user = nullptr;
static struct tm view_month;
static struct tm book_time;
static time_t book_day;
static bool is_booking_treatment;
static int selected_hour = -1;
static int selected_expert = -1;
static int selected_service = -1;
static int new_booking_id = 100;

typedef vector<tuple<char, string, State>> Options;

#define unpack(...) __VA_ARGS__
#define concat(x, y) x ## y
#define opts(name, init) case name: static const Options concat(name, _OPTS) = { unpack init }; options = &concat(name, _OPTS);
#define wip(name) opts(name, ({'e', "Exit", EXIT})) cout << "WIP" << endl; break;

void line() { cout << setw(20) << setfill('-') << '-' << endl; }

void time_now(struct tm &tm) {
    auto t = time(nullptr);
    tm = *localtime(&t);
}

static array<char, 128> format_string = {};

#define define_format(name, format) const char *name(const struct tm &t) { \
    strftime(format_string.data(), format_string.size(), format, &t); \
    return format_string.data(); \
}

define_format(format_time, "%A %e %B %G (%F) %T");
define_format(format_date, "%A %e %B %G (%F)");

string format_duration() {
    auto s = string(format_time(book_time));
    auto t = book_time;
    
    t.tm_hour += is_booking_treatment ? 2 : 1;
    mktime(&t);
    strftime(format_string.data(), format_string.size(), "~%T", &t);
    
    return s + format_string.data();
}

void reset_day(struct tm &t) {
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
}

bool try_book_treatment(time_t t, int hour, int expert) {
    return (schedule.find(t) == schedule.end()) || !(schedule[t][expert * WORK_HOURS + hour] || schedule[t][expert * WORK_HOURS + hour + 1]);
}

bool try_book_consultation(time_t t, int hour, int expert) {
    return (schedule.find(t) == schedule.end()) || !schedule[t][expert * WORK_HOURS + hour];
}

bool check_hour_treatment_availability(time_t t, int hour) {
    for (int i = 0; i < EXPERTS.size(); i++)
        if (try_book_treatment(t, hour, i))
            return true;
    
    return false;
}

bool check_hour_consultation_availability(time_t t, int hour) {
    for (int i = 0; i < EXPERTS.size(); i++)
        if (try_book_consultation(t, hour, i))
            return true;
    
    return false;
}

bool check_day_treatment_availability(time_t t) {
    for (int i = 0; i < WORK_HOURS - 1; i++) {
        if (check_hour_treatment_availability(t, i)) {
            return true;
        }
    }
    
    return false;
}

bool check_day_consultation_availability(time_t t) {
    return schedule.find(t) == schedule.end() || !schedule[t].all();
}


void calendar() {
    constexpr int CELL_WIDTH = 6;
    constexpr int LINE_WIDTH = CELL_WIDTH * WEEKDAY_NAMES.size();
    constexpr int MY_LEN = 3 + 4 + 1;
    constexpr int LEFT_PADDING = (LINE_WIDTH  - MY_LEN) / 2;
    constexpr int RIGHT_PADDING = LINE_WIDTH - MY_LEN - LEFT_PADDING;
    
    cout << setw(LEFT_PADDING) << setfill(' ') << ' ' << MONTH_NAMES[view_month.tm_mon] << " " << view_month.tm_year + 1900 << setw(RIGHT_PADDING) << ' ' << endl;
    
    for (const auto &weekday : WEEKDAY_NAMES) {
        cout << left << setw(CELL_WIDTH) << weekday;
    }
    
    cout << endl;
    
    auto view_day = view_month;
    view_day.tm_mday = 1;
    reset_day(view_day);
    
    time_t t = mktime(&view_day);
    
    if (view_day.tm_wday > 0)
        cout << setw(view_day.tm_wday * CELL_WIDTH) << ' ';
    
    while (true) {
        t = mktime(&view_day);
        
        if (view_day.tm_mon != view_month.tm_mon)
            break;
        
        cout << setw(2) << view_day.tm_mday++ << ' ' << (check_day_treatment_availability(t) ? 'T' : ' ') << (check_day_consultation_availability(t) ? 'C' : ' ') << ' ';
        
        if (view_day.tm_wday == 6)
            cout << endl;
    }
    
    if (view_day.tm_wday != 0)
        cout << endl;
}

void print_day_schedule(struct tm &date) {
    const string LABEL = "Experts";
    const auto COLUMN_SIZE = max(LABEL.size(), LONGEST_EXPERT_NAME_LENGTH);
    
    time_t t = mktime(&date);
    
    cout << "Schedule for " << format_date(date) << endl << endl;
    cout << setfill(' ') << setw(COLUMN_SIZE) << LABEL;
    
    for (int i = 0; i < WORK_HOURS; i++) {
        cout << '|' << 12 + i << ":00";
    }
    
    cout << endl;
    
    for (int i = 0; i < EXPERTS.size(); i++) {
        cout << setfill('-') << setw(COLUMN_SIZE) << '-';
        
        for (int j = 0; j < WORK_HOURS; j++)
            cout << '+' << setw(5) << '-';
        
        cout << endl << setfill(' ') << setw(COLUMN_SIZE) << EXPERTS[i].name;
        
        for (int j = 0; j < WORK_HOURS; j++)
            cout << "|  " << (schedule.find(t) == schedule.end() || !schedule[t][i * WORK_HOURS + j] ? "✓" : "✗") << "  ";
        
        cout << endl;
    }
}

State ui(State state, bool &validation)
{
    struct tm now;
    
    time_now(now);
    
    const Options *options = nullptr;
    
    clear();
    
    cout << "COMPANY LOGO" << endl;
    line();
    cout << format_time(now) << endl;
    line();
    
    switch (state)
    {
    opts(MENU, (
        {'1', "About us", ABOUT_US},
        {'2', "I'm a customer", CUSTOMER},
        {'3', "I'm a staff", STAFF},
        {'4', "Exit", EXIT},
    ))
        cout << "Main Menu" << endl;
        break;
        
        
    opts(ABOUT_US, ({'m', "Return to main menu", MENU}))
        cout << "About Us" << endl;
        break;
        
        
    opts(CUSTOMER, (
        {'s', "View a list of available services", SERVICE_LIST},
        {'e', "View a list of available experts", EXPERT_LIST},
        {'b', "Book service", BOOKING},
        {'v', "View appointments", APPOINTMENTS},
        {'m', "Return to main menu", MENU},
    ))
        cout << "Customer services" << endl;
        line();
        break;
        
        
    opts(SERVICE_LIST, (
        {'1', SERVICES[0].name, SERVICE_1},
        {'2', SERVICES[1].name, SERVICE_2},
        {'3', SERVICES[2].name, SERVICE_3},
        {'x', "Cancel", CUSTOMER},
    ))
        selected_service = -1;
        cout << "Available services" << endl;
        line();
        break;
        
        
    #define service(service_name, index) opts(service_name, ({'b', "Book now", BOOKING}, {'s', "Go back to service list", SERVICE_LIST})) \
        selected_service = index; \
        cout << SERVICES[index].name << " Service" << endl; \
        line(); \
        cout << SERVICES[index].description << endl; \
        break;
    
    service(SERVICE_1, 0)
    service(SERVICE_2, 1)
    service(SERVICE_3, 2)
        
        
    opts(EXPERT_LIST, (
        {'b', "Book an appointment with our experts", BOOKING},
        {'x', "Go back", CUSTOMER},
    ))
        cout << "Our experts:" << endl;
        
        for (const auto &expert : EXPERTS) {
            cout << "- " << expert.name << endl;
        }
        break;
        
        
    case BOOKING:
        view_month = now;
        view_month.tm_mday = 1;
        reset_day(view_month);
        return BOOK_VIEW;
        
        
    opts(BOOK_VIEW, (
        {'a', "Previous month", BOOK_VIEW_PREV_MONTH},
        {'d', "Next month", BOOK_VIEW_NEXT_MONTH},
        {'b', "Book a day", BOOK_SELECT_DAY},
        {'x', "Cancel", CUSTOMER},
    ))
        calendar();
        break;
        
        
    case BOOK_VIEW_PREV_MONTH:
        {
            auto this_month = now;
            
            this_month.tm_mday = 1;
            reset_day(this_month);
            
            auto prev_month = view_month;
            
            prev_month.tm_mon -= 1;
            
            if (mktime(&prev_month) >= mktime(&this_month))
                view_month = prev_month;
        }
        return BOOK_VIEW;
        
        
    case BOOK_VIEW_NEXT_MONTH:
        view_month.tm_mon += 1;
        mktime(&view_month);
        return BOOK_VIEW;
        
        
    case BOOK_SELECT_DAY:
        {
            string input;
            
            calendar();
            
            if (!validation)
                cout << endl << "Please input a future day in the month that is available for booking!" << endl;
            
            cout << endl << "Enter the day to book (x to cancel): ";
            
            cin >> input;
            
            if (input == "x")
                return BOOK_CANCEL;
            
            try {
                unsigned int day = stoi(input);
                
                if (day < 32) {
                    book_time = view_month;
                    book_time.tm_mday = day;
                    book_day = mktime(&book_time);
                    
                    bool consultation = check_day_consultation_availability(book_day);
                    bool treatment = check_day_treatment_availability(book_day);
                    
                    auto today = now;
                    
                    reset_day(today);
                    
                    if (book_day >= mktime(&today) && (consultation || treatment)) {
                        if (consultation && treatment)
                            return BOOK_SELECT_SERVICE_TYPE;
                        else {
                            is_booking_treatment = treatment;
                            return treatment ? BOOK_TREATMENT : BOOK_CONSULTATION;
                        }
                    }
                }
            }
            catch (invalid_argument e) {}
            catch (out_of_range e) {}
            
            validation = false;
            return BOOK_SELECT_DAY;
        }
    
    
    opts(BOOK_SELECT_SERVICE_TYPE, (
        {'t', "Book treatment", BOOK_TREATMENT},
        {'c', "Book consultation", BOOK_CONSULTATION},
        {'x', "Cancel", BOOK_CANCEL},
    ))
        cout << "Booking " << format_date(book_time) << endl;
        break;
        
        
    case BOOK_TREATMENT:
    {
        print_day_schedule(book_time);
        
        cout << endl << "Select a time to book" << endl;
        
        if (!validation)
            cout << endl << "Invalid input!" << endl;
        
        validation = true;
        
        cout << endl;
        
        for (int i = 0; i < WORK_HOURS - 1; i++)
            if (check_hour_treatment_availability(book_day, i))
                cout << static_cast<char>('1' + i) << ": " << OPENING_HOUR + i << ":00~" << OPENING_HOUR + i + 2 << ":00" << endl;
        
        cout << "x: Cancel" << endl;
        
        char c = input();
        
        if (c == 'x')
            return BOOK_CANCEL;
        
        for (int i = 0; i < WORK_HOURS - 1; i++)
            if (c == '1' + i && check_hour_treatment_availability(book_day, i)) {
                is_booking_treatment = true;
                book_time.tm_hour = OPENING_HOUR + i;
                selected_hour = i;
                return BOOK_SELECT_EXPERT;
            }
        
        validation = false;
        
        return BOOK_TREATMENT;
    }
    
    
    case BOOK_CONSULTATION:
    {
        print_day_schedule(book_time);
        
        cout << endl << "Select a time to book" << endl;
        
        if (!validation)
            cout << endl << "Invalid input!" << endl;
        
        validation = true;
        
        cout << endl;
        
        for (int i = 0; i < WORK_HOURS; i++)
            if (check_hour_consultation_availability(book_day, i))
                cout << static_cast<char>('1' + i) << ": " << OPENING_HOUR + i << ":00~" << OPENING_HOUR + i + 1 << ":00" << endl;
        
        cout << "x: Cancel" << endl;
        
        char c = input();
        
        if (c == 'x')
            return BOOK_CANCEL;
        
        for (int i = 0; i < WORK_HOURS; i++)
            if (c == '1' + i && check_hour_consultation_availability(book_day, i)) {
                is_booking_treatment = false;
                book_time.tm_hour = OPENING_HOUR + i;
                selected_hour = i;
                return BOOK_SELECT_EXPERT;
            }
        
        validation = false;
        
        return BOOK_CONSULTATION;
    }
    
    
    case BOOK_SELECT_EXPERT:
    {
        cout << endl << "Select an expert to book" << endl;
        
        if (!validation)
            cout << endl << "Invalid input!" << endl;
        
        validation = true;
        
        cout << endl;
        
        for (int i = 0; i < EXPERTS.size(); i++)
            if (is_booking_treatment ? try_book_treatment(book_day, selected_hour, i) : try_book_consultation(book_day, selected_hour, i))
                cout << static_cast<char>('1' + i) << ": Expert " << EXPERTS[i].name << endl;
        
        cout << "x: Cancel" << endl;
        
        char c = input();
        
        if (c == 'x')
            return BOOK_CANCEL;
        
        for (int i = 0; i < EXPERTS.size(); i++)
            if (c == '1' + i && (is_booking_treatment ? try_book_treatment(book_day, selected_hour, i) : try_book_consultation(book_day, selected_hour, i))) {
                selected_expert = i;
                return BOOK_SELECT_SERVICE;
            }
            
        
        validation = false;
        
        return BOOK_SELECT_EXPERT;
    }
    
    
    opts(BOOK_SELECT_SERVICE, (
        {'1', "Service " + SERVICES[0].name, BOOK_SELECT_SERVICE_1},
        {'2', "Service " + SERVICES[1].name, BOOK_SELECT_SERVICE_2},
        {'3', "Service " + SERVICES[2].name, BOOK_SELECT_SERVICE_3},
        {'x', "Cancel", BOOK_CANCEL},
    ))
        if (selected_service != -1)
            return BOOK_CONFIRM;
        
        cout << "Booking a " << (is_booking_treatment ? "consultation" : "treatment") << " on " << format_duration() << endl;
        break;
        
        
    case BOOK_SELECT_SERVICE_1:
        selected_service = 0;
        return BOOK_CONFIRM;
        
        
    case BOOK_SELECT_SERVICE_2:
        selected_service = 1;
        return BOOK_CONFIRM;
        
        
    case BOOK_SELECT_SERVICE_3:
        selected_service = 2;
        return BOOK_CONFIRM;
        
        
    opts(BOOK_CONFIRM, (
        {'y', "Yes, I am sure that the booking information is indeed what I wanted", BOOK_PAYMENT},
        {'n', "No, there are mistakes in the booking information", BOOK_CANCEL},
    ))
        cout << "Please confirm that the following information are correct" << endl;
        cout << (is_booking_treatment ? "Treatment" : "Consultation") << " during " << format_duration() << endl;
        cout << "Expert: " << EXPERTS[selected_expert].name << endl;
        cout << "Service: " << SERVICES[selected_service].name << endl;
        cout << SERVICES[selected_service].description << endl;
        cout << endl << "Fee: RM 100" << endl;
        break;
    
    
    opts(BOOK_PAYMENT, (
        {'c', "Credit card", BOOK_SUCCESS},
        {'e', "EWallet", BOOK_SUCCESS},
        {'b', "Online banking", BOOK_SUCCESS},
    ))
        cout << "Select your payment option" << endl;
        break;
    
    
    opts(BOOK_SUCCESS, ({'m', "Return to menu", CUSTOMER}))
    {
        string id = "B";
        
        id += to_string(new_booking_id);
        
        bookings.insert({id, {
            .is_treatment = is_booking_treatment,
            .service = selected_service,
            .expert = selected_expert,
            .time = book_time,
        }});
        
        if (schedule.find(book_day) == schedule.end())
            schedule.insert({book_day, {}});
        
        for (int i = 0; i < (is_booking_treatment ? 2 : 1); i++)
            schedule[book_day][selected_expert * WORK_HOURS + selected_hour + i] = true;
        
        cout << "Successfully booked the appointment!" << endl;
        cout << endl << "Appointment id: B" << new_booking_id++ << endl;
        cout << (is_booking_treatment ? "Treatment" : "Consultation") << " during " << format_duration() << endl;
        cout << "Expert: " << EXPERTS[selected_expert].name << endl;
        cout << "Service: " << SERVICES[selected_service].name << endl;
        cout << SERVICES[selected_service].description << endl;
        break;
    }
    
    
    case BOOK_CANCEL:
        selected_hour = -1;
        selected_service = -1;
        selected_expert = -1;
        return BOOK_VIEW;
    
    
    opts(APPOINTMENTS, (
        {'v', "View another appointment", APPOINTMENTS},
        {'m', "Return to menu", CUSTOMER},
    ))
    {
        string id;
        
        cout << "Please enter the appointment id: ";
        cin >> id;
        cout << endl;
        
        if (bookings.find(id) != bookings.end()) {
            const auto &appointment = bookings[id];
            
            book_time = appointment.time;
            is_booking_treatment = appointment.is_treatment;
            
            cout << "Your appointment is a " << SERVICES[appointment.service].name << " " << (is_booking_treatment ? "treatment" : "consultation") << " during " << format_duration() << endl;
            cout << "Expert in charge: " << EXPERTS[appointment.expert].name << endl;
            cout << "Service description: " << SERVICES[appointment.service].description << endl;
        }
        else {
            cout << "Invalid id" << endl;
        }
        
        break;
    }
    
        
    opts(STAFF, (
        {'l', "Login as staff", LOGIN},
        {'m', "Return to main menu", MENU},
    ))
        break;
        
        
    opts(LOGIN, (
        {'r', "Retry", LOGIN},
        {'m', "Return to main menu", MENU},
    ))
        
        {
            string username, password;
            
            cout << "Username: ";
            cin >> username;
            cout << "Password: ";
            cin >> password;
            
            cout << endl;
            
            if (username == ADMIN.username)
                login_user = &ADMIN;
            else {
                for (const Staff &expert : EXPERTS) {
                    if (expert.username == username) {
                        login_user = &expert;
                        break;
                    }
                }
            }
            
            if (login_user == nullptr)
                cout << "Invalid username" << endl;
            else if (password == login_user->password)
                return STAFF_MENU;
            else
                cout << "Invalid password" << endl;
        }
        
        break;
        
        
    opts(STAFF_MENU, (
        {'1', "View treatment schedule", TREATMENT_SCHEDULE},
        {'2', "View consultation schedule", CONSULTATION_SCHEDULE},
        {'3', "View customers", CUSTOMER_DETAILS},
        {'4', "View sales report", SALES_REPORT},
        {'5', "Log out", LOGOUT},
    ))
        cout << "Staff menu" << endl;
        cout << "Currently login as " << login_user->username << " (" << login_user->name << ")" << endl;
        break;
        
        
    wip(TREATMENT_SCHEDULE)
        
        
    wip(CONSULTATION_SCHEDULE)
        
        
    wip(CUSTOMER_DETAILS)
        
        
    wip(SALES_REPORT)
        
        
    opts(LOGOUT, (
        {'l', "Log in", LOGIN},
        {'m', "Return to main menu", MENU},
    ))
        login_user = nullptr;
        cout << "Successfully logged out!" << endl;
        break;
        
        
    case EXIT:
        cout << "Thank you for using our service" << endl;
        return EXIT;
        
    default:
        break;
    }
    
    cout << endl;
    
    if (!validation)
        cout << "Invalid option" << endl << endl;
    
    validation = true;
    
    if (options->size() > 1) {
        cout << "Please select the options" << endl;
        
        for (const auto &opt : *options) {
            cout << get<0>(opt) << ": " << get<1>(opt) << endl;
        }
    }
    else
        cout << "Press " << get<0>((*options)[0]) << " to " << get<1>((*options)[0]) << endl;
    
    char c;
    
    while ((c = input()) == '\n');
    
    for (const auto &opt : *options) {
        if (get<0>(opt) == c) {
            return get<2>(opt);
        }
    }
    
    validation = false;
    
    return state;
}

int main()
{
    bool validation = true;
    State state = ui(MENU, validation);
    
    while (state != EXIT) {
        state = ui(state, validation);
    }
    
    ui(state, validation);
    
    return 0;
}
