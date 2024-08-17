#include <iostream>
#include <iomanip>
#include <array>
#include <vector>
#include <bitset>
#include <ctime>
#include <map>
#include <unordered_map>
#include <algorithm>

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
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    const char c = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return c;
}

void clear() {
    cout << "\E[H\E[2J";
}

#endif

template<typename T>
struct static_vec {
    const T *arr;
    size_t size;
    
    constexpr static_vec(initializer_list<T> a): arr(a.begin()), size(a.size()) {}
    constexpr const T& operator[](size_t i) const { return arr[i]; }
};

#define static_Vec_init(name, type, init) static constexpr initializer_list<type> concat(_, name) = { unpack init }; static constexpr static_vec<type> name = { concat(_, name) }

struct static_str {
    const char *str;
    size_t len;
    
    template<size_t N>
    constexpr static_str(const char (&s)[N]) : str(s), len(N - 1) {}
};

ostream &operator <<(ostream &os, static_str s) {
    return os << s.str;
}

enum State {
    MENU,
    ABOUT_US,
    CUSTOMER,
    STAFF,
    EXIT,
    SERVICE_LIST,
    VIEW_SERVICE,
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
    BOOK_ENTER_DETAILS,
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
    SCHEDULE_VIEW,
    SCHEDULE_VIEW_PREV_WEEK,
    SCHEDULE_VIEW_NEXT_WEEK,
    CUSTOMER_DETAILS,
    CUSTOMER_VIEW,
    CUSTOMER_VIEW_PREV_PAGE,
    CUSTOMER_VIEW_NEXT_PAGE,
    SALES_REPORT,
};

enum ServiceType : bool {
    TREATMENT,
    CONSULTATION,
};

enum Gender : bool {
    MALE,
    FEMALE,
};

struct Service {
    static_str name;
    static_str description;
    double fee;
};

struct Staff {
    static_str name;
    Gender gender;
    static_str username;
    static_str password;
};

struct Booking {
    ServiceType service_type;
    int service;
    int expert;
    struct tm time;
};

struct Option {
    char key;
    static_str description;
    State link;
};

struct Customer {
    string name;
    Gender gender;
    string phone_number;
    vector<Booking> booking_history;
    int total_payment;
    
};

#define unpack(...) __VA_ARGS__
#define concat(x, y) x ## y

constexpr array MONTH_NAMES {
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

constexpr array WEEKDAY_NAMES {
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
};

constexpr array SERVICES {
    Service {"Skin Care", "blablabla", 100},
    Service {"Face Makeup", "lorem ipsum", 123},
    Service {"Hair Coloring", "foo bar baz", 999},
};

constexpr array EXPERTS {
    Staff { "Ang Zi En", MALE, "aze", "12345678" },
    Staff { "Wilson Ang Shao En", MALE, "wase", "12345678" },
    Staff { "Wai Chee Han", MALE, "wch", "12345678" },
};

constexpr const static_str *find_longest_expert_name() {
    const static_str *p = nullptr;
    
    for (const auto &s : EXPERTS) {
        if (p == nullptr || s.name.len > p->len) {
            p = &s.name;
        }
    }
    
    return p;
}

constexpr auto LONGEST_EXPERT_NAME_LENGTH = find_longest_expert_name()->len;
constexpr Staff ADMIN { "Administrator", MALE, "admin", "12345678" };
constexpr auto WORK_HOURS = 6;
constexpr auto OPENING_HOUR = 12;
constexpr auto CUSTOMER_ENTRIES_PER_PAGE = 10;

static unordered_map<time_t, bitset<WORK_HOURS * EXPERTS.size()>> schedule {};
static unordered_map<string, Booking> all_bookings {};
static map<string, Customer> all_customers {};
static array<unordered_map<time_t, string>, EXPERTS.size()> treatment_schedule {}, consultation_schedule {};
static const Staff *login_user = nullptr;
static struct tm view_time;
static time_t book_day;
static ServiceType service_type;
static Booking booking { .service = -1, .expert = -1 };
static Customer customer;
static int selected_hour = -1;
static int new_booking_id = 1000;
static int view_page = 0;
static map<string, Customer>::iterator customer_view;

auto time_now(struct tm &tm) {
    const auto t = time(nullptr);
    tm = *localtime(&t);
    return t;
}

template<typename T, typename U>
bool has_key(const unordered_map<T, U> &map, const T &t) {
    return map.find(t) != map.end();
}

template<typename T, typename U>
bool has_key(const map<T, U> &map, const T &t) {
    return map.find(t) != map.end();
}

#define center(qualifier, line_width, string_width, string) \
    { \
        qualifier size_t _LINE_WIDTH = line_width; \
        qualifier size_t _STRING_WIDTH = string_width; \
        if (_STRING_WIDTH > _LINE_WIDTH) cout << string; \
        else { \
            qualifier auto PADDING = _LINE_WIDTH - _STRING_WIDTH; \
            qualifier auto LEFT_PADDING = PADDING / 2; \
            qualifier auto RIGHT_PADDING = PADDING - LEFT_PADDING; \
            coutfill(' ', LEFT_PADDING); \
            cout << string; \
            coutfill(' ', RIGHT_PADDING); \
        } \
    }

static array<char, 128> format_string = {};

#define define_format(name, format) const char *name(const struct tm &t) { \
    strftime(format_string.data(), format_string.size(), format, &t); \
    return format_string.data(); \
}

define_format(format_time, "%A %e %B %G (%F) %T");
define_format(format_date, "%A %e %B %G (%F)");

string format_duration() {
    auto s = string(format_time(booking.time));
    auto t = booking.time;
    
    t.tm_hour += service_type == TREATMENT ? 2 : 1;
    mktime(&t);
    
    strftime(format_string.data(), format_string.size(), "~%T", &t);
    
    s += format_string.data();
    
    return s;
}

void reset_day(struct tm &t) {
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
}

bool try_book_treatment(const time_t t, const int hour, const int expert) {
    return has_key(schedule, t) || !(schedule[t][expert * WORK_HOURS + hour] || schedule[t][expert * WORK_HOURS + hour + 1]);
}

bool try_book_consultation(const time_t t, const int hour, const int expert) {
    return has_key(schedule, t) || !schedule[t][expert * WORK_HOURS + hour];
}

bool check_hour_treatment_availability(const time_t t, const int hour) {
    for (int i = 0; i < EXPERTS.size(); i++)
        if (try_book_treatment(t, hour, i))
            return true;
    
    return false;
}

bool check_hour_consultation_availability(const time_t t, const int hour) {
    for (int i = 0; i < EXPERTS.size(); i++)
        if (try_book_consultation(t, hour, i))
            return true;
    
    return false;
}

bool check_day_treatment_availability(const time_t t) {
    for (int i = 0; i < WORK_HOURS - 1; i++) {
        if (check_hour_treatment_availability(t, i)) {
            return true;
        }
    }
    
    return false;
}

bool check_day_consultation_availability(const time_t t) {
    return has_key(schedule, t) || !schedule[t].all();
}

void coutfill(char c, size_t n) {
    if (n > 0)
        cout << setfill(c) << setw(n) << c;
}

void line() {
    coutfill('-', 20);
    cout << endl;
}

void calendar(time_t today) {
    constexpr int CELL_WIDTH = 6;
    
    center(constexpr, CELL_WIDTH * WEEKDAY_NAMES.size(), 3 + 4 + 1, MONTH_NAMES[view_time.tm_mon] << " " << view_time.tm_year + 1900)
    
    cout << endl;
    
    for (const auto &weekday : WEEKDAY_NAMES) {
        center(constexpr, CELL_WIDTH - 1, 3, weekday)
        cout << ' ';
    }
    
    cout << endl;
    
    auto view_day = view_time;
    view_day.tm_mday = 1;
    reset_day(view_day);
    
    auto t = mktime(&view_day);
    
    if (view_day.tm_wday > 0)
        coutfill(' ', view_day.tm_wday * CELL_WIDTH);
    
    while (true) {
        t = mktime(&view_day);
        
        if (view_day.tm_mon != view_time.tm_mon)
            break;
        
        cout << setw(2) << view_day.tm_mday++ << ' ';
        
        if (t < today)
            cout << "--";
        else
            cout << (check_day_treatment_availability(t) ? 'T' : ' ') << (check_day_consultation_availability(t) ? 'C' : ' ');
        
        cout << ' ';
        
        if (view_day.tm_wday == 6)
            cout << endl;
    }
    
    if (view_day.tm_wday != 0)
        cout << endl;
}

void print_day_schedule(struct tm &date) {
    constexpr static_str LABEL = "Experts";
    constexpr auto COLUMN_SIZE = max(LABEL.len, LONGEST_EXPERT_NAME_LENGTH);
    constexpr auto CELL_SIZE = 5;
    
    const auto t = mktime(&date);
    
    cout << "Schedule for " << format_date(date) << endl << endl;
    cout << setfill(' ') << setw(COLUMN_SIZE) << left << LABEL;
    
    for (int i = 0; i < WORK_HOURS; i++) {
        cout << '|' << 12 + i << ":00";
    }
    
    cout << endl;
    
    for (int i = 0; i < EXPERTS.size(); i++) {
        coutfill('-', COLUMN_SIZE);
        
        for (int j = 0; j < WORK_HOURS; j++) {
            cout << '+';
            
            coutfill('-', CELL_SIZE);
        }
        
        cout << endl << setfill(' ') << setw(COLUMN_SIZE) << left << EXPERTS[i].name;
        
        for (int j = 0; j < WORK_HOURS; j++) {
            cout << '|';
            
            center(constexpr, CELL_SIZE, 1, (has_key(schedule, t) && schedule[t][i * WORK_HOURS + j] ? "✗" : "✓"));
        }
        
        cout << endl;
    }
}

State ui(const State state, bool &validation)
{
    #define refresh return state;
    #define redirect(state) return state;
    
    struct tm now;
    
    time_now(now);
    
    static_vec<Option> options {};
    
    clear();
    
    cout << "COMPANY LOGO" << endl;
    line();
    cout << format_time(now) << endl;
    line();
    
    switch (state)
    {
        #define opts(name, init) case name: static_Vec_init(concat(name, _OPTS), Option, init); options = concat(name, _OPTS);
        #define wip(name) opts(name, ({'e', "Exit", EXIT})) cout << "WIP" << endl; break;
        #define custom_input(opt_count, cancel_state, opt_check, opt_description, opt_match) \
            { \
                if (!validation) \
                    cout << endl << "Invalid input!" << endl; \
                \
                validation = true; \
                \
                constexpr int count = opt_count; \
                \
                cout << endl << "Please select any of the following options" << endl; \
                \
                for (int i = 0; i < count; i++) \
                    if (opt_check) \
                        cout << static_cast<char>('1' + i) << ": " << opt_description << endl; \
                \
                cout << "x: Cancel" << endl; \
                \
                const char c = input(); \
                \
                if (c == 'x') \
                    return cancel_state; \
                \
                for (int i = 0; i < count; i++) \
                    if (c == '1' + i && (opt_check)) { \
                        unpack opt_match \
                    } \
                \
                validation = false; \
                refresh \
            }
            
            
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
            
            
        case SERVICE_LIST:
            cout << "Available services" << endl;
            line();
            
            booking.service = -1;
            
            custom_input(
                SERVICES.size(),
                CUSTOMER,
                true,
                SERVICES[i].name,
                ({
                    booking.service = i;
                    redirect(VIEW_SERVICE)
                })
            )
            
            
        opts(VIEW_SERVICE, (
            {'b', "Book now", BOOKING},
            {'s', "Go back to service list", SERVICE_LIST},
        ))
            cout << SERVICES[booking.service].name << " Service (RM " << setprecision(2) << fixed << SERVICES[booking.service].fee << ")" << endl;
            line();
            cout << SERVICES[booking.service].description << endl;
            break;
            
            
        opts(EXPERT_LIST, (
            {'b', "Book an appointment with our experts", BOOKING},
            {'r', "Go back", CUSTOMER},
        ))
            cout << "Our experts:" << endl;
            
            for (const auto &expert : EXPERTS) {
                cout << "- " << expert.name << " (" << (expert.gender == MALE ? "Male" : "Female") << ')' << endl;
            }
            
            break;
            
            
        case BOOKING:
            view_time = now;
            reset_day(view_time);
            redirect(BOOK_VIEW)
            
            
        opts(BOOK_VIEW, (
            {'a', "Previous month", BOOK_VIEW_PREV_MONTH},
            {'d', "Next month", BOOK_VIEW_NEXT_MONTH},
            {'b', "Book a day", BOOK_SELECT_DAY},
            {'x', "Cancel", CUSTOMER},
        ))
        {
            auto today = now;
            
            reset_day(today);
            
            calendar(mktime(&today));
            break;
        }
            
            
        case BOOK_VIEW_PREV_MONTH:
            {
                auto this_month = now;
                
                this_month.tm_mday = 1;
                reset_day(this_month);
                
                auto prev_month = view_time;
                
                prev_month.tm_mon -= 1;
                
                if (mktime(&prev_month) >= mktime(&this_month))
                    view_time = prev_month;
            }
            redirect(BOOK_VIEW)
            
            
        case BOOK_VIEW_NEXT_MONTH:
            view_time.tm_mon += 1;
            redirect(BOOK_VIEW)
            
            
        case BOOK_SELECT_DAY:
            {
                string input;
                
                auto today = now;
                
                reset_day(today);
                
                calendar(mktime(&today));
                
                if (!validation)
                    cout << endl << "Please input a future day in the month that is available for booking!" << endl;
                
                cout << endl << "Enter the day to book (x to cancel): ";
                
                cin >> input;
                
                if (input == "x")
                    redirect(BOOK_CANCEL)
                
                try {
                    const unsigned int day = stoi(input);
                    
                    if (day < 32) {
                        booking.time = view_time;
                        booking.time.tm_mday = day;
                        book_day = mktime(&booking.time);
                        
                        const bool consultation = check_day_consultation_availability(book_day);
                        const bool treatment = check_day_treatment_availability(book_day);
                        
                        auto today = now;
                        
                        reset_day(today);
                        
                        if (book_day >= mktime(&today) && (consultation || treatment)) {
                            if (consultation && treatment)
                                redirect(BOOK_SELECT_SERVICE_TYPE)
                            else {
                                redirect(treatment ? BOOK_TREATMENT : BOOK_CONSULTATION)
                            }
                        }
                    }
                }
                catch (invalid_argument e) {}
                catch (out_of_range e) {}
                
                validation = false;
                refresh
            }
        
        
        opts(BOOK_SELECT_SERVICE_TYPE, (
            {'t', "Book treatment", BOOK_TREATMENT},
            {'c', "Book consultation", BOOK_CONSULTATION},
            {'x', "Cancel", BOOK_CANCEL},
        ))
            cout << "Booking " << format_date(booking.time) << endl;
            break;
            
            
        case BOOK_TREATMENT:
            print_day_schedule(booking.time);
            
            cout << endl << "Select a time to book" << endl;
            
            custom_input(
                WORK_HOURS - 1,
                BOOK_CANCEL,
                check_hour_treatment_availability(book_day, i),
                OPENING_HOUR + i << ":00~" << OPENING_HOUR + i + 2 << ":00",
                ({
                    service_type = TREATMENT;
                    booking.time.tm_hour = OPENING_HOUR + i;
                    selected_hour = i;
                    redirect(BOOK_SELECT_EXPERT)
                })
            )
        
        
        case BOOK_CONSULTATION:
            print_day_schedule(booking.time);
            
            cout << endl << "Select a time to book" << endl;
            
            custom_input(
                WORK_HOURS,
                BOOK_CANCEL,
                check_hour_consultation_availability(book_day, i),
                OPENING_HOUR + i << ":00~" << OPENING_HOUR + i + 1 << ":00",
                ({
                    service_type = CONSULTATION;
                    booking.time.tm_hour = OPENING_HOUR + i;
                    selected_hour = i;
                    redirect(BOOK_SELECT_EXPERT)
                })
            )
        
        
        case BOOK_SELECT_EXPERT:
            cout << "Booking a " << (service_type == TREATMENT ? "treatment" : "consultation") << " on " << format_duration() << endl;
            cout << endl << "Select an expert to book" << endl;
            
            custom_input(
                EXPERTS.size(),
                BOOK_CANCEL,
                service_type == TREATMENT ? try_book_treatment(book_day, selected_hour, i) : try_book_consultation(book_day, selected_hour, i),
                "Expert " << EXPERTS[i].name,
                ({
                    booking.expert = i;
                    redirect(BOOK_SELECT_SERVICE)
                })
            )
        
        
        case BOOK_SELECT_SERVICE:
            if (booking.service != -1)
                redirect(BOOK_ENTER_DETAILS)
            
            cout << "Booking a " << (service_type == TREATMENT ? "treatment" : "consultation") << " on " << format_duration() << endl;
            
            custom_input(
                SERVICES.size(),
                BOOK_CANCEL,
                true,
                "Service " << SERVICES[i].name,
                ({
                    booking.service = i;
                    redirect(BOOK_ENTER_DETAILS)
                })
            )
        
        
        opts(BOOK_ENTER_DETAILS, ({'r', "Retry", BOOK_ENTER_DETAILS}))
            cout << "Customer Details" << endl;
            line();
            cout << "Enter your name: ";
            cin >> customer.name;
            
            if (!has_key(all_customers, customer.name)) {
                cout << "Enter your gender (M/F): ";
                
                while (true) {
                    const char c = input();
                    
                    if (c == 'm') {
                        customer.gender = MALE;
                        cout << "Male";
                        break;
                    }
                    if (c == 'f') {
                        customer.gender = FEMALE;
                        cout << "Female";
                        break;
                    }
                }
                
                cout << endl << "Enter your phone number (numbers only): ";
                cin >> customer.phone_number;
                
                if (customer.phone_number.size() > 11 || !all_of(customer.phone_number.begin(), customer.phone_number.end(), ::isdigit)) {
                    cout << "Invalid input" << endl;
                    break;
                }
            }
            
            redirect(BOOK_CONFIRM)
        
            
        opts(BOOK_CONFIRM, (
            {'y', "Yes, I am sure that the booking information is indeed what I wanted", BOOK_PAYMENT},
            {'n', "No, there are mistakes in the booking information", BOOK_CANCEL},
        ))
            cout << "Please confirm that the following information are correct" << endl;
            line();
            cout << "Name: " << customer.name << endl;
            cout << "Gender: " << (customer.gender == MALE ? "Male" : "Female") << endl;
            cout << "Contact number: " << customer.phone_number << endl << endl;
            cout << (service_type == TREATMENT ? "Treatment" : "Consultation") << " during " << format_duration() << endl;
            cout << "Expert: " << EXPERTS[booking.expert].name << endl;
            cout << "Service: " << SERVICES[booking.service].name << endl;
            cout << SERVICES[booking.service].description << endl;
            cout << endl << "Fee: RM " << setprecision(2) << fixed << SERVICES[booking.service].fee << endl;
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
            
            auto &sched = (service_type == TREATMENT ? treatment_schedule : consultation_schedule);
            
            sched[booking.expert].insert({mktime(&booking.time), id});
            
            booking.service_type = service_type;
            
            all_bookings.insert({id, booking});
            
            if (has_key(all_customers, customer.name)) {
                auto &c = all_customers[customer.name];
                
                c.booking_history.push_back(booking);
                c.total_payment += SERVICES[booking.service].fee;
            }
            else {
                customer.booking_history = {};
                customer.total_payment = 0;
                
                all_customers.insert({customer.name, customer});
            }
            
            if (has_key(schedule, book_day))
                schedule.insert({book_day, {}});
            
            for (int i = 0; i < (service_type == TREATMENT ? 2 : 1); i++)
                schedule[book_day][booking.expert * WORK_HOURS + selected_hour + i] = true;
            
            cout << "Successfully booked the appointment!" << endl;
            cout << endl << "Appointment id: B" << new_booking_id++ << endl;
            cout << (service_type == TREATMENT ? "Treatment" : "Consultation") << " during " << format_duration() << endl;
            cout << "Expert: " << EXPERTS[booking.expert].name << endl;
            cout << "Service: " << SERVICES[booking.service].name << endl;
            cout << SERVICES[booking.service].description << endl;
            
            booking = { .service = -1, .expert = -1 };
            
            break;
        }
        
        
        case BOOK_CANCEL:
            selected_hour = -1;
            booking = { .service = -1, .expert = -1 };
            redirect(BOOK_VIEW)
        
        
        opts(APPOINTMENTS, (
            {'v', "View another appointment", APPOINTMENTS},
            {'m', "Return to menu", CUSTOMER},
        ))
        {
            string id;
            
            cout << "Please enter the appointment id: ";
            cin >> id;
            cout << endl;
            
            if (has_key(all_bookings, id)) {
                const auto &appointment = all_bookings[id];
                
                booking.time = appointment.time;
                service_type = appointment.service_type;
                
                cout << "Your appointment is a " << SERVICES[appointment.service].name << " " << (service_type == TREATMENT ? "treatment" : "consultation") << " during " << format_duration() << endl;
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
                
                if (username == ADMIN.username.str)
                    login_user = &ADMIN;
                else {
                    for (const Staff &expert : EXPERTS) {
                        if (expert.username.str == username) {
                            login_user = &expert;
                            break;
                        }
                    }
                }
                
                if (login_user == nullptr)
                    cout << "Invalid username" << endl;
                else if (password == login_user->password.str)
                    redirect(STAFF_MENU)
                else
                    cout << "Invalid password" << endl;
                
                login_user = nullptr;
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
            
            if (login_user == &ADMIN)
                booking.expert = -1;
            else {
                for (int i = 0; i < EXPERTS.size(); i++)
                    if (login_user == &EXPERTS[i]) {
                        booking.expert = i;
                        break;
                    }
            }
            
            break;
            
            
        case TREATMENT_SCHEDULE:
            service_type = TREATMENT;
            view_time = now;
            redirect(SCHEDULE_VIEW)
            
            
        case CONSULTATION_SCHEDULE:
            service_type = CONSULTATION;
            view_time = now;
            redirect(SCHEDULE_VIEW)
            
        
        opts(SCHEDULE_VIEW, (
            {'a', "Previous week", SCHEDULE_VIEW_PREV_WEEK},
            {'d', "Next week", SCHEDULE_VIEW_NEXT_WEEK},
            {'m', "Go back to staff menu", STAFF_MENU},
        ))
        {
            view_time.tm_mday -= view_time.tm_wday;
            reset_day(view_time);
            view_time.tm_hour = OPENING_HOUR;
            
            constexpr auto LABEL_SIZE = 5;
            constexpr auto CELL_SIZE = 5;
            constexpr auto LINE_WIDTH = LABEL_SIZE + WEEKDAY_NAMES.size() * (CELL_SIZE + 1);
            constexpr static_str SEPARATOR = " ~ ";
            
            auto we = view_time;
            
            we.tm_mday += WEEKDAY_NAMES.size() - 1;
            
            const string week_start = format_date(view_time);
            const string week_end = format_date(we);
            
            center(const, LINE_WIDTH, week_start.length() + SEPARATOR.len + week_end.length(), week_start << SEPARATOR.str << week_end)
            
            cout << endl;
            coutfill(' ', 5);
            
            for (const auto &weekday : WEEKDAY_NAMES) {
                cout << '|';
                center(constexpr, 5, 3, weekday);
            }
            
            cout << endl;
            
            if (booking.expert != -1) // Admin special case not handled yet
            
            for (int i = 0; i < WORK_HOURS; i++, view_time.tm_hour++) {
                coutfill('-', LABEL_SIZE);
                
                for (int j = 0; j < WEEKDAY_NAMES.size(); j++) {
                    cout << '+';
                    coutfill('-', CELL_SIZE);
                }
                
                cout << endl << OPENING_HOUR + i << ":00";
                
                for (int j = 0; j < WEEKDAY_NAMES.size(); j++, view_time.tm_mday++) {
                    const auto t = mktime(&view_time);
                    const string *id = nullptr;
                    
                    if (service_type == TREATMENT) {
                        if (has_key(treatment_schedule[booking.expert], t))
                            id = &treatment_schedule[booking.expert][t];
                        else if (i > 0) {
                            auto prev_hour = view_time;
                            
                            prev_hour.tm_hour -= 1;
                            
                            const auto pt = mktime(&prev_hour);
                            
                            if (has_key(treatment_schedule[booking.expert], pt))
                                id = &treatment_schedule[booking.expert][pt];
                        }
                    }
                    else if (has_key(consultation_schedule[booking.expert], t))
                        id = &consultation_schedule[booking.expert][t];
                    
                    cout << '|' << (id == nullptr ? "  -  " : *id);
                }
                
                view_time.tm_mday -= WEEKDAY_NAMES.size();
                
                cout << endl;
            }
            break;
        }
        
        
        case SCHEDULE_VIEW_PREV_WEEK:
        {
            auto this_week = now;
            auto prev_week = view_time;
            
            this_week.tm_mday -= this_week.tm_wday;
            prev_week.tm_mday -= WEEKDAY_NAMES.size();
            
            reset_day(this_week);
            
            if (mktime(&prev_week) >= mktime(&this_week))
                view_time = prev_week;
            
            redirect(SCHEDULE_VIEW)
        }
        
        
        case SCHEDULE_VIEW_NEXT_WEEK:
            view_time.tm_mday += WEEKDAY_NAMES.size();
            redirect(SCHEDULE_VIEW)
        
            
        case CUSTOMER_DETAILS:
            customer_view = all_customers.begin();
            redirect(CUSTOMER_VIEW)
        
        
        opts(CUSTOMER_VIEW, (
            {'a', "Previous page", CUSTOMER_VIEW_PREV_PAGE},
            {'d', "Next page", CUSTOMER_VIEW_NEXT_PAGE},
            {'m', "Main menu", STAFF_MENU},
        ))
            if (all_customers.size() > 0) {
                cout << "Customers (Page " << view_page + 1 << " of " << (all_customers.size() + CUSTOMER_ENTRIES_PER_PAGE - 1) / CUSTOMER_ENTRIES_PER_PAGE << ')' << endl;
                line();
                
                const auto offset = view_page * CUSTOMER_ENTRIES_PER_PAGE;
                
                auto view = customer_view;
                
                for (auto i = 0; i < CUSTOMER_ENTRIES_PER_PAGE && view != all_customers.end(); i++, view++) {
                    const auto &customer = view->second;
                    
                    cout << offset + i + 1 << ". " << customer.name << " (" << (customer.gender == MALE ? "Male" : "Female") << ") 📞 " << customer.phone_number << endl;
                }
            }
            else
                cout << "Customer list is empty" << endl;
            
            break;
        
        
        case CUSTOMER_VIEW_PREV_PAGE:
            if (view_page > 0) {
                view_page--;
                
                for (auto i = 0; i < CUSTOMER_ENTRIES_PER_PAGE; i++)
                    customer_view--;
            }
            redirect(CUSTOMER_VIEW)
        
        
        case CUSTOMER_VIEW_NEXT_PAGE:
            if (view_page + 1 < (all_customers.size() + CUSTOMER_ENTRIES_PER_PAGE - 1) / CUSTOMER_ENTRIES_PER_PAGE) {
                view_page++;
                
                for (auto i = 0; i < CUSTOMER_ENTRIES_PER_PAGE; i++)
                    customer_view++;
            }
            redirect(CUSTOMER_VIEW)
            
            
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
    
    if (options.size > 1) {
        cout << "Please select any of the following options" << endl;
        
        for (int i = 0; i < options.size; i++) {
            cout << options[i].key << ": " << options[i].description << endl;
        }
    }
    else
        cout << "Press " << options[0].key << " to " << options[0].description << endl;
    
    char c;
    
    while ((c = input()) == '\n');
    
    for (int i = 0; i < options.size; i++) {
        if (options[i].key == c) {
            redirect(options[i].link)
        }
    }
    
    validation = false;
    refresh
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
