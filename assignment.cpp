#include <iostream>
#include <iomanip>
#include <cctype>
#include <ctime>
#include <cstring>
#include <array>
#include <bitset>
#include <map>
#include <unordered_map>

using namespace std;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <conio.h>
#include <cstdlib>

char input_key_impl() {
    return getch();
}

void clear() {
    system("cls");
}

#else
#include <termios.h>
#include <unistd.h>

char input_key_impl() {
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    const char c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    return c;
}

void clear() {
    cout << "\E[H\E[2J";
}

#endif

char input_key() {
    char c;
    
    while ((c = input_key_impl()) == '\n');
    
    return c;
}

template<typename T, typename U>
bool has_key(const unordered_map<T, U> &map, const T &t) {
    return map.find(t) != map.end();
}

template<typename T, typename U>
bool has_key(const map<T, U> &map, const T &t) {
    return map.find(t) != map.end();
}

template<typename T>
struct ConstVec {
    const T *arr;
    size_t size;
    
    constexpr ConstVec(): arr(nullptr), size(0) {}
    constexpr ConstVec(initializer_list<T> a): arr(a.begin()), size(a.size()) {}
    constexpr const T& operator[](size_t i) const { return arr[i]; }
};

#define concat(x, y) x ## y
#define unpack(...) __VA_ARGS__
#define static_Vec_init(name, type, init) static constexpr initializer_list<type> concat(_, name) = { unpack init }; static constexpr ConstVec<type> name = { concat(_, name) }

auto duplicate_str(const char *s, size_t n) {
    return static_cast<const char *>(memcpy(new char[n], s, n));
}

struct ConstStr {
    const char *str;
    size_t len;
    
    constexpr ConstStr() : str(nullptr), len(0) {}
    
    template<size_t N>
    constexpr ConstStr(const char (&s)[N]) : str(s), len(N - 1) {}
    
    ConstStr(string s) : str(duplicate_str(s.c_str(), s.length() + 1)), len(s.length()) {}
};

ostream &operator <<(ostream &os, const ConstStr &s) {
    return os << s.str;
}

void operator >>(istream &is, ConstStr &s) {
    string str;
    
    is >> str;
    s = { std::move(str) };
}

bool operator ==(const ConstStr &x, const ConstStr &y) {
    return x.len == y.len && (x.len == 0 || memcmp(x.str, y.str, x.len) == 0);
}

struct Time {
    struct tm raw_time;
    time_t time_int;
    
    constexpr char hour() const { return raw_time.tm_hour; }
    constexpr short day() const { return raw_time.tm_mday; }
    constexpr char month() const { return raw_time.tm_mon; }
    constexpr short year() const { return raw_time.tm_year + 1900; }
    constexpr char day_of_the_week() const { return raw_time.tm_wday; }
    constexpr const time_t &time() const { return time_int; }
    constexpr const struct tm &raw() const { return raw_time; }
    constexpr struct tm &raw() { return raw_time; }
    
    constexpr Time copy() const {
        return *this;
    }
    
    constexpr Time &set_hour(const unsigned char hour) {
        raw().tm_hour = 0;
        raw_time.tm_hour = hour;
        raw_time.tm_min = 0;
        raw_time.tm_sec = 0;
        return *this;
    }
    
    constexpr Time &set_day(const signed char day) {
        raw_time.tm_mday = day;
        return *this;
    }
    
    constexpr Time &set_month(const signed char month) {
        raw_time.tm_mon = month;
        return *this;
    }
    
    constexpr Time &reset_day_of_week() {
        raw_time.tm_mday -= raw_time.tm_wday;
        return *this;
    }
    
    Time &calibrate() {
        time_int = mktime(&raw_time);
        return *this;
    }
    
    constexpr bool operator <(const Time &t) const {
        return time() < t.time();
    }
    
    constexpr bool operator <=(const Time &t) const {
        return time() <= t.time();
    }
    
    constexpr bool operator ==(const Time &t) const {
        return time() == t.time();
    }
    
    constexpr bool operator >(const Time &t) const {
        return time() > t.time();
    }
    
    constexpr bool operator >=(const Time &t) const {
        return time() >= t.time();
    }
};

enum State {
    MAIN_MENU,
    
    
    ABOUT_US,
    
    
    CUSTOMER_LOGIN,
    CUSTOMER_MENU,
    SERVICE_LIST,
    VIEW_SERVICE,
    EXPERT_LIST,
    APPOINTMENTS,
    
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
    BOOK_CONFIRM,
    BOOK_PAYMENT,
    BOOK_SUCCESS,
    BOOK_CANCEL,
    
    
    STAFF_LOGIN,
    STAFF_MENU,
    INVALID_USER,
    
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
    SALES_REPORT_CUSTOMER_SUMMARY,
    SALES_REPORT_EXPERT_SUMMARY,
    SALES_REPORT_FINANCE_SUMMARY,
    
    
    LOGOUT,
    
    EXIT,
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
    ConstStr name;
    ConstStr description;
    double consultation_fee;
    double treatment_fee;
};

struct Booking {
    ServiceType service_type;
    int service;
    int expert;
    Time time;
    Time day;
};

struct CustomerData {
    map<time_t, Booking> booking_history;
    int total_payment;
};

struct User {
    ConstStr username;
    ConstStr password;
    ConstStr name;
    Gender gender;
    ConstStr phone_number;
    CustomerData *customer_data;
};

struct Option {
    ConstStr description;
    State link;
};

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
    Service {"Skin Care", "blablabla", 100, 200},
    Service {"Face Makeup", "lorem ipsum", 123, 456},
    Service {"Hair Coloring", "foo bar baz", 999, 9999},
};

constexpr array EXPERTS {
    User { "aze", "12345678", "Ang Zi En", FEMALE, "999", nullptr },
    User { "wch", "12345678" , "Wai Chee Han", MALE, "999", nullptr },
    User { "wase", "12345678", "Wilson Ang Shao En", MALE, "+6011-7243 3165", nullptr },
};

constexpr const ConstStr *find_longest_expert_name() {
    const ConstStr *p = nullptr;
    
    for (const auto &s : EXPERTS) {
        if (p == nullptr || s.name.len > p->len) {
            p = &s.name;
        }
    }
    
    return p;
}

constexpr auto MAX_EXPERT_NAME_LENGTH = find_longest_expert_name()->len;
constexpr User ADMIN { "admin", "12345678", "Administrator", MALE, "-", nullptr };
constexpr auto WORK_HOURS = 6;
constexpr auto OPENING_HOUR = 12;
constexpr auto CUSTOMER_ENTRIES_PER_PAGE = 10;

void time_now(Time &t) {
    t.time_int = time(nullptr);
    t.raw() = *localtime(&t.time());
}

static array<char, 128> format_string = {};

#define define_format(name, format) const char *name(const Time &t) { \
    strftime(format_string.data(), format_string.size(), format, &t.raw()); \
    return format_string.data(); \
}

define_format(format_time, "%A %e %B %G (%F) %T");
define_format(format_date, "%A %e %B %G (%F)");
    
string format_booking_time(const Booking &booking) {
    string s { format_time(booking.time) };
    const Time end_time = booking.time.copy().set_hour(booking.time.hour() + (booking.service_type == TREATMENT ? 2 : 1));
    
    strftime(format_string.data(), format_string.size(), "~%T", &end_time.raw());
    
    s += format_string.data();
    
    return s;
}

template<char C, size_t N>
void coutfill() {
    if (N > 0)
        cout << setfill(C) << setw(N) << C;
}

template<char C>
void coutfill(const size_t n) {
    if (n > 0)
        cout << setfill(C) << setw(n) << C;
}

// void coutfill(const char c, const size_t n) {
//     if (n > 0)
//         cout << setfill(c) << setw(n) << c;
// }

template<size_t LINE_WIDTH, size_t STRING_WIDTH, typename F>
void center(F f) {
    if (STRING_WIDTH > LINE_WIDTH) f();
    else {
        constexpr auto PADDING = LINE_WIDTH - STRING_WIDTH;
        constexpr auto LEFT = PADDING / 2;
        constexpr auto RIGHT = PADDING - LEFT;
        
        coutfill<' ', LEFT>();
        f();
        coutfill<' ', RIGHT>();
    }
}

template<size_t LINE_WIDTH, typename F>
void center(const size_t string_width, F f) {
    if (string_width > LINE_WIDTH) f();
    else {
        const auto padding = LINE_WIDTH - string_width;
        const auto left = padding / 2;
        const auto right = padding - left;
        
        coutfill<' '>(left);
        f();
        coutfill<' '>(right);
    }
}

// template<typename F>
// void center(const size_t line_width, const size_t string_width, F f) {
//     if (string_width > line_width) f();
//     else {
//         const auto padding = line_width - string_width;
//         const auto left = padding / 2;
//         const auto right = padding - left;
        
//         coutfill<' '>(left);
//         f();
//         coutfill<' '>(right);
//     }
// }

void print_line() {
    coutfill<'-', 20>();
    cout << endl;
}

constexpr bool is_phone_number(const ConstStr &s) {
    for (size_t i = 0; i < s.len; i++)
        if (!isdigit(s.str[i]))
            return false;
    
    return true;
}

template<size_t EXPERT_COUNT, size_t WORK_HOURS>
struct ExpertScheduleMap {
    typedef bitset<EXPERT_COUNT * WORK_HOURS> Bitset;
    typedef map<time_t, Bitset> Map;
    
    struct ExpertSchedule {
        const Bitset *bs;
        size_t expert;
        
        bool operator[](const size_t hour) { return bs != nullptr && (*bs)[expert * 6 + hour]; }
    };
    
    struct ExpertScheduleSet {
        const Bitset *bs;
        
        constexpr ExpertSchedule operator[](const size_t expert) const { return { bs, expert }; }
    };
    
    struct BitSetter {
        Bitset &bs;
        size_t expert;
        
        typename Bitset::reference operator[](const size_t hour) { return bs[expert * 6 + hour]; }
    };
    
    struct ArraySetter {
        Bitset &bs;
        
        BitSetter operator[](const size_t expert) { return { bs, expert }; }
    };
    
    struct MapSetter {
        ExpertScheduleMap &map;
        
        ArraySetter operator[](const Time &day) {
            if (!has_key(map.get_map(), day.time()))
                map.get_map().insert({day.time(), {}});
            
            return { map.get_map()[day.time()] };
        }
    };
    
    Map _map;
    
    constexpr Map &get_map() { return _map; }
    MapSetter set() { return {*this}; }
    ExpertScheduleSet operator[](const Time &day) { return { (has_key(_map, day.time()) ? &_map[day.time()] : nullptr) }; }
};

struct Cache {
    ExpertScheduleMap<EXPERTS.size(), WORK_HOURS> schedule;
    unordered_map<string, Booking> booking_database;
    map<string, User> customer_database;
    array<map<time_t, string>, EXPERTS.size()> treatment_schedule, consultation_schedule;
    const User *login_user;
    Time view_time;
    ServiceType service_type;
    Booking booking;
    int selected_hour;
    int new_booking_id;
    int view_page;
    map<string, User>::iterator customer_view;
    
    // day must be .set_hour(0).calibrate() first
    bool check_availability(const ServiceType type, const Time &t, const int hour = -1, const int expert = -1) {
        if (hour == -1 && expert == -1 && type == CONSULTATION)
            return !(has_key(schedule.get_map(), t.time()) && schedule[t].bs->all());
        
        if (hour == -1) {
            for (int i = 0; i < WORK_HOURS - (type == TREATMENT ? 1 : 0); i++)
                if (check_availability(type, t, i, expert))
                    return true;
            
            return false;
        }
        
        if (expert == -1) {
            for (int i = 0; i < EXPERTS.size(); i++)
                if (check_availability(type, t, hour, i))
                    return true;
            
            return false;
        }
        
        switch (type) {
            case TREATMENT:
                return !(schedule[t][expert][hour] || schedule[t][expert][hour + 1]);
            
            case CONSULTATION:
                return !schedule[t][expert][hour];
        }
    }
    
    void print_calendar(const Time &today) {
        constexpr int CELL_WIDTH = 6;
        
        center<CELL_WIDTH * WEEKDAY_NAMES.size(), 3 + 4 + 1>([&]() { cout << MONTH_NAMES[view_time.month()] << " " << view_time.year(); });
        
        cout << endl;
        
        for (const auto &weekday : WEEKDAY_NAMES) {
            center<CELL_WIDTH - 1, 3>([&]() { cout << weekday; });
            cout << ' ';
        }
        
        cout << endl;
        
        Time view_day = view_time;
        
        if (view_day.day_of_the_week() > 0)
            coutfill<' '>(view_day.day_of_the_week() * CELL_WIDTH);
        
        while (view_day.month() == view_time.month()) {
            cout << setw(2) << view_day.day() << ' ';
            
            if (view_day < today)
                cout << "--";
            else
                cout << (check_availability(CONSULTATION, view_day) ? 'C' : '-') << (check_availability(TREATMENT, view_day) ? 'T' : '-');
            
            cout << ' ';
            
            if (view_day.day_of_the_week() == 6)
                cout << endl;
            
            view_day.set_day(view_day.day() + 1).calibrate();
        }
        
        if (view_day.day_of_the_week() != 0)
            cout << endl;
    }

    void print_schedule() {
        constexpr ConstStr LABEL = "Experts";
        constexpr auto COLUMN_SIZE = max(LABEL.len, MAX_EXPERT_NAME_LENGTH);
        constexpr auto CELL_SIZE = 5;
        
        cout << "Schedule for " << format_date(booking.time) << endl << endl;
        cout << setfill(' ') << setw(COLUMN_SIZE) << left << LABEL;
        
        for (int i = 0; i < WORK_HOURS; i++) {
            cout << '|' << 12 + i << ":00";
        }
        
        cout << endl;
        
        for (int i = 0; i < EXPERTS.size(); i++) {
            coutfill<'-', COLUMN_SIZE>();
            
            for (int j = 0; j < WORK_HOURS; j++) {
                cout << '+';
                
                coutfill<'-', CELL_SIZE>();
            }
            
            cout << endl << setfill(' ') << setw(COLUMN_SIZE) << left << EXPERTS[i].name;
            
            for (int j = 0; j < WORK_HOURS; j++) {
                cout << '|';
                
                center<CELL_SIZE, 1>([&]() { cout << (schedule[booking.time][i][j] ? "✗" : "✓"); });
            }
            
            cout << endl;
        }
    }
};

void print_customer_data(const User &customer) {
    const auto &data = *customer.customer_data;
    
    int treatment = 0, consultation = 0;
    
    for (const auto &booking : data.booking_history)
        switch (booking.second.service_type) {
            case TREATMENT:
                treatment += 1;
                break;
            
            case CONSULTATION:
                consultation += 1;
                break;
        }
    
    cout << "\tGender: " << (customer.gender == MALE ? "Male" : "Female") << endl;
    cout << "\tAppointment count: " << data.booking_history.size() << endl;
    cout << "\tBooked consultation count: " << consultation << endl;
    cout << "\tBooked treatment count: " << treatment << endl;
    cout << "\tTotal Payment: " << data.total_payment << endl;
}

template<size_t OPTION_COUNT, State CANCEL_STATE, typename F, typename G, typename H>
State custom_input(const State state, bool &validation, F checker, G description, H matching) {
    if (!validation)
        cout << endl << "Invalid input!" << endl;
    
    validation = true;
    
    size_t indexes[OPTION_COUNT] = {};
    size_t count = 0;
    
    cout << endl << "Please select any of the following options" << endl;
    
    for (size_t i = 0; i < OPTION_COUNT; i++)
        if (checker(i)) {
            cout << static_cast<char>('1' + count) << ": ";
            description(i);
            cout << endl;
            indexes[count++] = i;
        }
    
    cout << static_cast<char>('1' + count) << ": Cancel" << endl;
    
    const char c = input_key() - '1';
    
    if (c == count)
        return CANCEL_STATE;
    
    if (c >= 0 && c < count) {
        return matching(indexes[c]);
    }
    
    validation = false;
    return state;
}

State ui(const State state, bool &validation, Cache &cache)
{
    #define refresh return state;
    #define redirect(state) return state;
    
    Time now;
    
    time_now(now);
    
    ConstVec<Option> options {};
    
    clear();
    
    cout << "COMPANY LOGO" << endl;
    print_line();
    cout << format_time(now) << endl;
    print_line();
    
    if (cache.login_user != nullptr) {
        cout << "Currently login as " << cache.login_user->username << " (" << cache.login_user->name << ")" << endl;
        print_line();
    }
    
    switch (state)
    {
        #define opts(name, init) case name: static_Vec_init(concat(name, _OPTS), Option, init); options = concat(name, _OPTS);
        #define wip(name) opts(name, ({"Exit", EXIT})) cout << "WIP" << endl; break;
        #define OPT_MAIN_MENU(menu) {"Return to main menu", menu}
            
            
        opts(MAIN_MENU, (
            {"About us", ABOUT_US},
            {"I'm a customer", CUSTOMER_LOGIN},
            {"I'm a staff", STAFF_LOGIN},
            {"Exit", EXIT},
        ))
            cout << "Main Menu" << endl;
            break;
            
            
        opts(ABOUT_US, (OPT_MAIN_MENU(MAIN_MENU)))
            cout << "About Us" << endl;
            break;
        
        
        opts(CUSTOMER_LOGIN, ({"Retry", CUSTOMER_LOGIN}, OPT_MAIN_MENU(MAIN_MENU)))
        {
            string username;
            
            cout << "Login or Register" << endl << endl;
            cout << "Username: ";
            
            cin >> username;
            
            if (has_key(cache.customer_database, username)) {
                ConstStr password;
                const auto &user = cache.customer_database[username];
                
                cout << "Password: ";
                
                cin >> password;
                
                if (password == user.password) {
                    cache.login_user = &user;
                    redirect(CUSTOMER_MENU)
                }
                
                cout << endl << "Wrong password" << endl;
                break;
            }
            else {
                cout << endl << "This username does not exist. Do you wish to register a new account with this username?" << endl;
                cout << "Press x to cancel. Press Space to proceed with the registration process. ";
                
                char c;
                
                while ((c = input_key()) != ' ')
                    if (c == 'x')
                        redirect(MAIN_MENU)
                
                User user;
                
                cout << endl << endl << "Enter your name: ";
                cin >> user.name;
                cout << "Enter your gender (M/F): ";
                
                while (true) {
                    const char c = input_key();
                    
                    if (c == 'm') {
                        user.gender = MALE;
                        cout << "Male";
                        break;
                    }
                    if (c == 'f') {
                        user.gender = FEMALE;
                        cout << "Female";
                        break;
                    }
                }
                
                cout << endl << "Enter your phone number (numbers only): ";
                cin >> user.phone_number;
                
                if (user.phone_number.len > 11 || !is_phone_number(user.phone_number)) {
                    cout << endl << "Invalid input" << endl;
                    break;
                }
                
                cout << "Enter your password: ";
                cin >> user.password;
                
                user.username = { username };
                user.customer_data = new CustomerData;
                cache.login_user = &cache.customer_database.insert({std::move(username), user}).first->second;
                
                redirect(CUSTOMER_MENU)
            }
        }
            
            
        opts(CUSTOMER_MENU, (
            {"View a list of available services", SERVICE_LIST},
            {"View a list of available experts", EXPERT_LIST},
            {"Book service", BOOKING},
            {"View appointments", APPOINTMENTS},
            {"Log out", LOGOUT},
        ))
            cout << "Customer services" << endl;
            print_line();
            break;
            
            
        case SERVICE_LIST:
            cout << "Available services" << endl;
            print_line();
            
            cache.booking.service = -1;
            
            return custom_input<SERVICES.size(), CUSTOMER_MENU>(
                state,
                validation,
                [](size_t) { return true; },
                [](size_t i) { cout << SERVICES[i].name; },
                [&](size_t i) {
                    cache.booking.service = i;
                    redirect(VIEW_SERVICE)
                }
            );
            
            
        opts(VIEW_SERVICE, (
            {"Book now", BOOKING},
            {"Go back to service list", SERVICE_LIST},
        ))
        {
            const auto &service = SERVICES[cache.booking.service];
            
            cout << service.name << " Service (Consultation: RM " << setprecision(2) << fixed << service.consultation_fee << "; Treatment: RM " << service.treatment_fee << ")" << endl;
            print_line();
            cout << service.description << endl;
            
            break;
        }
            
            
        opts(EXPERT_LIST, (
            {"Book an appointment with our experts", BOOKING},
            {"Go back", CUSTOMER_MENU},
        ))
            cout << "Our experts:" << endl;
            
            for (const auto &expert : EXPERTS)
                cout << "- " << expert.name << " (" << (expert.gender == MALE ? "Male" : "Female") << ") 📞 " << expert.phone_number  << endl;
            
            return custom_input<EXPERTS.size(), CUSTOMER_MENU>(
                state,
                validation,
                [](size_t) { return true; },
                [](size_t i) { cout << "Book an appointment with " << EXPERTS[i].name; },
                [&](size_t i) {
                    cache.booking.expert = i;
                    return BOOKING;
                }
            );
            
            break;
            
            
        case BOOKING:
            cache.view_time = now.set_hour(0).set_day(1).calibrate();
            redirect(BOOK_VIEW)
            
            
        opts(BOOK_VIEW, (
            {"Previous month", BOOK_VIEW_PREV_MONTH},
            {"Next month", BOOK_VIEW_NEXT_MONTH},
            {"Book a day", BOOK_SELECT_DAY},
            {"Cancel", CUSTOMER_MENU},
        ))
        {
            cache.print_calendar(now.set_hour(0).calibrate());
            
            cout << "C means consultation is available to book" << endl;
            cout << "T means treatment is available to book" << endl;
            
            break;
        }
            
            
        case BOOK_VIEW_PREV_MONTH:
            {
                const auto prev_month = cache.view_time.copy().set_month(cache.view_time.month() - 1).calibrate();
                
                if (prev_month >= now.set_day(0).set_hour(0).calibrate())
                    cache.view_time = prev_month;
            }
            redirect(BOOK_VIEW)
            
            
        case BOOK_VIEW_NEXT_MONTH:
            cache.view_time.set_month(cache.view_time.month() + 1).calibrate();
            redirect(BOOK_VIEW)
            
            
        case BOOK_SELECT_DAY:
            {
                string input;
                
                const auto &today = now.set_hour(0).calibrate();
                
                cache.print_calendar(today);
                
                if (!validation)
                    cout << endl << "Please input a future day in the month that is available for booking!" << endl;
                
                cout << endl << "Enter the day to book (x to cancel): ";
                
                cin >> input;
                
                if (input == "x")
                    redirect(BOOK_CANCEL)
                
                try {
                    const unsigned int day = stoi(input);
                    
                    if (day < 32) {
                        const auto selected_day = cache.view_time.copy().set_day(day).calibrate();
                        
                        if (selected_day.month() == cache.view_time.month()) {
                            cache.booking.time = selected_day;
                            cache.booking.day = selected_day;
                            
                            const bool consultation = cache.check_availability(CONSULTATION, cache.booking.day);
                            const bool treatment = cache.check_availability(TREATMENT, cache.booking.day);
                            
                            if (selected_day >= today && (consultation || treatment)) {
                                if (consultation && treatment)
                                    redirect(BOOK_SELECT_SERVICE_TYPE)
                                else {
                                    redirect(treatment ? BOOK_TREATMENT : BOOK_CONSULTATION)
                                }
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
            {"Book treatment", BOOK_TREATMENT},
            {"Book consultation", BOOK_CONSULTATION},
            {"Cancel", BOOK_CANCEL},
        ))
            cout << "Booking " << format_date(cache.booking.time) << endl;
            break;
            
            
        case BOOK_TREATMENT:
            cache.print_schedule();
            
            cout << endl << "Select a time to book" << endl;
            
            return custom_input<WORK_HOURS - 1, BOOK_CANCEL>(
                state,
                validation,
                [&](size_t i) { return cache.check_availability(TREATMENT, cache.booking.day, i); },
                [](size_t i) { cout << OPENING_HOUR + i << ":00~" << OPENING_HOUR + i + 2 << ":00"; },
                [&](size_t i) {
                    cache.service_type = TREATMENT;
                    cache.booking.time.set_hour(OPENING_HOUR + i).calibrate();
                    cache.selected_hour = i;
                    redirect(BOOK_SELECT_EXPERT)
                }
            );
        
        
        case BOOK_CONSULTATION:
            cache.print_schedule();
            
            cout << endl << "Select a time to book" << endl;
            
            return custom_input<WORK_HOURS, BOOK_CANCEL>(
                state,
                validation,
                [&](size_t i) { return cache.check_availability(CONSULTATION, cache.booking.day, i); },
                [](size_t i) { cout << OPENING_HOUR + i << ":00~" << OPENING_HOUR + i + 1 << ":00"; },
                [&](size_t i) {
                    cache.service_type = CONSULTATION;
                    cache.booking.time.set_hour(OPENING_HOUR + i).calibrate();
                    cache.selected_hour = i;
                    redirect(BOOK_SELECT_EXPERT)
                }
            );
        
        
        case BOOK_SELECT_EXPERT:
            cout << "Booking a " << (cache.service_type == TREATMENT ? "treatment" : "consultation") << " on " << format_booking_time(cache.booking) << endl;
            cout << endl << "Select an expert to book" << endl;
            
            return custom_input<EXPERTS.size(), BOOK_CANCEL>(
                state,
                validation,
                [&](size_t i) { return cache.check_availability(cache.service_type, cache.booking.day, cache.selected_hour, i); },
                [](size_t i) { cout << "Expert " << EXPERTS[i].name; },
                [&](size_t i) {
                    cache.booking.expert = i;
                    redirect(BOOK_SELECT_SERVICE)
                }
            );
        
        
        case BOOK_SELECT_SERVICE:
            if (cache.booking.service != -1)
                redirect(BOOK_CONFIRM)
            
            cout << "Booking a " << (cache.service_type == TREATMENT ? "treatment" : "consultation") << " on " << format_booking_time(cache.booking) << endl;
            
            return custom_input<SERVICES.size(), BOOK_CANCEL>(
                state,
                validation,
                [](size_t) { return true; },
                [](size_t i) { cout << "Service " << SERVICES[i].name; },
                [&](size_t i) {
                    cache.booking.service = i;
                    redirect(BOOK_CONFIRM)
                }
            );
        
            
        opts(BOOK_CONFIRM, (
            {"Yes, I am sure that the booking information is indeed what I wanted", BOOK_PAYMENT},
            {"No, there are mistakes in the booking information", BOOK_CANCEL},
        ))
        {
            const auto &service = SERVICES[cache.booking.service];
            const auto &customer = *cache.login_user;
            
            cout << "Please confirm that the following information are correct" << endl;
            print_line();
            cout << "Name: " << customer.name << endl;
            cout << "Gender: " << (customer.gender == MALE ? "Male" : "Female") << endl;
            cout << "Contact number: " << customer.phone_number << endl << endl;
            cout << (cache.service_type == TREATMENT ? "Treatment" : "Consultation") << " during " << format_booking_time(cache.booking) << endl;
            cout << "Expert: " << EXPERTS[cache.booking.expert].name << endl;
            cout << "Service: " << service.name << endl;
            cout << service.description << endl;
            cout << endl << "Fee: RM " << setprecision(2) << fixed << (cache.service_type == TREATMENT ? service.treatment_fee : service.consultation_fee) << endl;
            break;
        }
        
        
        opts(BOOK_PAYMENT, (
            {"Credit card", BOOK_SUCCESS},
            {"EWallet", BOOK_SUCCESS},
            {"Online banking", BOOK_SUCCESS},
        ))
            cout << "Select your payment option" << endl;
            break;
        
        
        opts(BOOK_SUCCESS, (OPT_MAIN_MENU(CUSTOMER_MENU)))
        {
            const auto &service = SERVICES[cache.booking.service];
            
            string id = "B";
            
            id += to_string(cache.new_booking_id);
            
            auto &sched = (cache.service_type == TREATMENT ? cache.treatment_schedule : cache.consultation_schedule);
            
            sched[cache.booking.expert].insert({cache.booking.time.time(), id});
            
            cache.booking.service_type = cache.service_type;
            cache.booking_database.insert({std::move(id), cache.booking});
            cache.login_user->customer_data->booking_history.insert({cache.booking.time.time(), cache.booking});
            cache.login_user->customer_data->total_payment += cache.service_type == TREATMENT ? service.treatment_fee : service.consultation_fee;
            
            if (has_key(cache.schedule.get_map(), cache.booking.day.time()))
                cache.schedule.get_map().insert({cache.booking.day.time(), {}});
            
            for (int i = 0; i < (cache.service_type == TREATMENT ? 2 : 1); i++)
                cache.schedule.set()[cache.booking.day][cache.booking.expert][cache.selected_hour + i] = true;
            
            cout << "Successfully booked the appointment!" << endl;
            cout << endl << "Appointment id: B" << cache.new_booking_id++ << endl;
            cout << (cache.service_type == TREATMENT ? "Treatment" : "Consultation") << " during " << format_booking_time(cache.booking) << endl;
            cout << "Expert: " << EXPERTS[cache.booking.expert].name << endl;
            cout << "Service: " << service.name << endl;
            cout << service.description << endl;
            
            cache.booking = { .service = -1, .expert = -1 };
            
            break;
        }
        
        
        case BOOK_CANCEL:
            cache.selected_hour = -1;
            cache.booking = { .service = -1, .expert = -1 };
            redirect(BOOK_VIEW)
        
        
        opts(APPOINTMENTS, (
            {"View another appointment", APPOINTMENTS},
            {"Go back", CUSTOMER_MENU},
        ))
        {
            string id;
            
            cout << "Please enter the appointment id: ";
            cin >> id;
            cout << endl;
            
            if (has_key(cache.booking_database, id)) {
                const auto &appointment = cache.booking_database[id];
                
                cout << "Your appointment is a " << SERVICES[appointment.service].name << " " << (appointment.service_type == TREATMENT ? "treatment" : "consultation") << " during " << format_booking_time(appointment) << endl;
                cout << "Expert in charge: " << EXPERTS[appointment.expert].name << endl;
                cout << "Service description: " << SERVICES[appointment.service].description << endl;
            }
            else {
                cout << "Invalid id" << endl;
            }
            
            break;
        }
        
            
        opts(STAFF_LOGIN, (
            {"Retry", STAFF_LOGIN},
            OPT_MAIN_MENU(MAIN_MENU),
        ))
            {
                ConstStr username, password;
                
                cout << "Login as staff" << endl << endl;
                cout << "Username: ";
                cin >> username;
                
                if (username == ADMIN.username)
                    cache.login_user = &ADMIN;
                else {
                    for (const User &expert : EXPERTS) {
                        if (username == expert.username) {
                            cache.login_user = &expert;
                            break;
                        }
                    }
                }
                
                if (cache.login_user == nullptr) {
                    cout << endl << "Invalid username" << endl;
                    break;
                }
                
                cout << "Password: ";
                cin >> password;
                
                if (password == cache.login_user->password)
                    redirect(STAFF_MENU)
                else
                    cout << endl << "Invalid password" << endl;
                
                cache.login_user = nullptr;
            }
            
            break;
            
            
        opts(STAFF_MENU, (
            {"View treatment schedule", TREATMENT_SCHEDULE},
            {"View consultation schedule", CONSULTATION_SCHEDULE},
            {"View customers", CUSTOMER_DETAILS},
            {"View sales report", SALES_REPORT},
            {"Log out", LOGOUT},
        ))
            cout << "Staff menu" << endl;
            
            if (cache.login_user == &ADMIN)
                cache.booking.expert = -1;
            else {
                for (int i = 0; i < EXPERTS.size(); i++)
                    if (cache.login_user == &EXPERTS[i]) {
                        cache.booking.expert = i;
                        break;
                    }
            }
            
            break;
            
            
        case TREATMENT_SCHEDULE:
            cache.service_type = TREATMENT;
            cache.view_time = now.reset_day_of_week().calibrate();
            redirect(SCHEDULE_VIEW)
            
            
        case CONSULTATION_SCHEDULE:
            cache.service_type = CONSULTATION;
            cache.view_time = now.reset_day_of_week().calibrate();
            redirect(SCHEDULE_VIEW)
            
        
        opts(SCHEDULE_VIEW, (
            {"Previous week", SCHEDULE_VIEW_PREV_WEEK},
            {"Next week", SCHEDULE_VIEW_NEXT_WEEK},
            OPT_MAIN_MENU(STAFF_MENU),
        ))
        {
            if (cache.login_user == &ADMIN)
                redirect(INVALID_USER)
            
            constexpr auto LABEL_SIZE = 5;
            constexpr auto CELL_SIZE = 5;
            constexpr auto LINE_WIDTH = LABEL_SIZE + WEEKDAY_NAMES.size() * (CELL_SIZE + 1);
            constexpr ConstStr SEPARATOR = " ~ ";
            
            const string week_start = format_date(cache.view_time);
            const string week_end = format_date(cache.view_time.copy().set_day(cache.view_time.day() + 6).calibrate());
            
            center<LINE_WIDTH>(week_start.length() + SEPARATOR.len + week_end.length(), [&]() { cout << week_start << SEPARATOR << week_end; });
            
            cout << endl;
            coutfill<' ', 5>();
            
            for (const auto &weekday : WEEKDAY_NAMES) {
                cout << '|';
                center<5, 3>([&]() { cout << weekday; });
            }
            
            cout << endl;
            
            Time t = cache.view_time.copy().set_hour(OPENING_HOUR).calibrate();
            
            for (int i = 0; i < WORK_HOURS; i++, t = cache.view_time.copy().set_hour(OPENING_HOUR + ++i).calibrate()) {
                coutfill<'-', LABEL_SIZE>();
                
                for (int j = 0; j < WEEKDAY_NAMES.size(); j++) {
                    cout << '+';
                    coutfill<'-', CELL_SIZE>();
                }
                
                cout << endl << t.hour() << ":00";
                
                for (; t.day_of_the_week() < WEEKDAY_NAMES.size(); t.set_day(t.day() + 1).calibrate()) {
                    const string *id = nullptr;
                    
                    if (cache.service_type == TREATMENT) {
                        if (has_key(cache.treatment_schedule[cache.booking.expert], t.time()))
                            id = &cache.treatment_schedule[cache.booking.expert][t.time()];
                        else if (i > 0) {
                            const Time prev_hour = t.copy().set_hour(t.hour() - 1).calibrate();
                            
                            if (has_key(cache.treatment_schedule[cache.booking.expert], prev_hour.time()))
                                id = &cache.treatment_schedule[cache.booking.expert][prev_hour.time()];
                        }
                    }
                    else if (has_key(cache.consultation_schedule[cache.booking.expert], t.time()))
                        id = &cache.consultation_schedule[cache.booking.expert][t.time()];
                    
                    cout << '|' << (id == nullptr ? "  -  " : *id);
                }
                
                cout << endl;
            }
            
            break;
        }
        
        
        case SCHEDULE_VIEW_PREV_WEEK:
        {
            const auto prev_week = cache.view_time.copy().set_day(cache.view_time.day() - 7).calibrate();
            
            if (prev_week >= now.set_hour(0).reset_day_of_week().calibrate())
                cache.view_time = prev_week;
            
            redirect(SCHEDULE_VIEW)
        }
        
        
        case SCHEDULE_VIEW_NEXT_WEEK:
            cache.view_time.set_day(cache.view_time.day() + 7).calibrate();
            redirect(SCHEDULE_VIEW)
        
            
        case CUSTOMER_DETAILS:
            cache.customer_view = cache.customer_database.begin();
            redirect(CUSTOMER_VIEW)
        
        
        opts(CUSTOMER_VIEW, (
            {"Previous page", CUSTOMER_VIEW_PREV_PAGE},
            {"Next page", CUSTOMER_VIEW_NEXT_PAGE},
            OPT_MAIN_MENU(STAFF_MENU),
        ))
            if (cache.customer_database.size() > 0) {
                cout << "Customers (Page " << cache.view_page + 1 << " of " << (cache.customer_database.size() + CUSTOMER_ENTRIES_PER_PAGE - 1) / CUSTOMER_ENTRIES_PER_PAGE << ')' << endl;
                print_line();
                
                const auto offset = cache.view_page * CUSTOMER_ENTRIES_PER_PAGE;
                
                auto view = cache.customer_view;
                
                for (auto i = 0; i < CUSTOMER_ENTRIES_PER_PAGE && view != cache.customer_database.end(); i++, view++) {
                    const auto &customer = view->second;
                    
                    cout << offset + i + 1 << ". " << customer.name << " (" << (customer.gender == MALE ? "Male" : "Female") << ") 📞 " << customer.phone_number << endl;
                }
            }
            else
                cout << "Customer list is empty" << endl;
            
            break;
        
        
        case CUSTOMER_VIEW_PREV_PAGE:
            if (cache.view_page > 0) {
                cache.view_page--;
                
                for (auto i = 0; i < CUSTOMER_ENTRIES_PER_PAGE; i++)
                    cache.customer_view--;
            }
            redirect(CUSTOMER_VIEW)
        
        
        case CUSTOMER_VIEW_NEXT_PAGE:
            if (cache.view_page + 1 < (cache.customer_database.size() + CUSTOMER_ENTRIES_PER_PAGE - 1) / CUSTOMER_ENTRIES_PER_PAGE) {
                cache.view_page++;
                
                for (auto i = 0; i < CUSTOMER_ENTRIES_PER_PAGE; i++)
                    cache.customer_view++;
            }
            redirect(CUSTOMER_VIEW)
            
            
        opts(SALES_REPORT, (
            {"Customer summary", SALES_REPORT_CUSTOMER_SUMMARY},
            {"Expert summary", SALES_REPORT_EXPERT_SUMMARY},
            {"Finance summary", SALES_REPORT_FINANCE_SUMMARY},
        ))
            cout << "Sales report" << endl;
            break;
        
        
        opts(SALES_REPORT_CUSTOMER_SUMMARY, (OPT_MAIN_MENU(STAFF_MENU)))
            cout << "Sales Report: Customer Summary" << endl;
            print_line();
            
            if (cache.customer_database.size() > 0) {
                const User *most_appointments = nullptr;
                const User *most_paid = nullptr;
                
                for (const auto &p : cache.customer_database) {
                    const auto &customer = p.second;
                    const auto &data = *customer.customer_data;
                    
                    if (most_appointments == nullptr || most_appointments->customer_data->booking_history.size() < data.booking_history.size())
                        most_appointments = &customer;
                    
                    if (most_paid == nullptr || most_paid->customer_data->total_payment < data.total_payment)
                        most_paid = &customer;
                }
                
                cout << "Customer who booked the highest count of appointments: " << most_appointments->name << endl;
                print_customer_data(*most_appointments);
                
                print_line();
                
                cout << "Customer who paid most for our services: " << most_paid->name << endl;
                print_customer_data(*most_paid);
            }
            else
                cout << "Not enough data" << endl;
            
            break;
            
        
        
        opts(SALES_REPORT_EXPERT_SUMMARY, (OPT_MAIN_MENU(STAFF_MENU)))
        {
            constexpr ConstStr HOURS_WORKED = "Hours Worked", TOTAL_SALES = "Total Sales";
            constexpr auto MAX_RECORD_NAME_LEN = max(HOURS_WORKED.len, TOTAL_SALES.len);
            
            coutfill<' ', MAX_RECORD_NAME_LEN>();
            
            break;
        }
        
        
        opts(SALES_REPORT_FINANCE_SUMMARY, (OPT_MAIN_MENU(STAFF_MENU)))
            break;
        
        
        opts(INVALID_USER, (OPT_MAIN_MENU(STAFF_MENU)))
            cout << "Unavailable to this user" << endl;
            break;
            
            
        opts(LOGOUT, (OPT_MAIN_MENU(MAIN_MENU)))
            cache.login_user = nullptr;
            cout << "Successfully logged out!" << endl;
            break;
        
            
        case EXIT:
            cout << "Thank you for using our service" << endl;
            return EXIT;
        
        
        default:
            cout << "Invalid state " << state << endl;
            break;
    }
    
    cout << endl;
    
    if (!validation)
        cout << "Invalid option" << endl << endl;
    
    validation = true;
    
    char c;
    
    if (options.size > 1) {
        cout << "Please select any of the following options" << endl;
        
        for (int i = 0; i < options.size; i++) {
            cout << i + 1 << ": " << options[i].description << endl;
        }
    
        c = input_key() - '1';
        
        if (c >= 0 && c < options.size)
            redirect(options[c].link)
    }
    else {
        cout << "Press Space to " << options[0].description << endl;
        
        c = input_key();
        
        if (c == ' ')
            redirect(options[0].link)
    }
    
    validation = false;
    refresh
}

int main()
{
    bool validation = true;
    Cache cache {
        .schedule = {},
        .booking_database = {},
        .customer_database = {},
        .treatment_schedule = {},
        .consultation_schedule = {},
        .login_user = nullptr,
        .booking = { .service = -1, .expert = -1 },
        .selected_hour = -1,
        .new_booking_id = 1000,
        .view_page = 0,
    };
    State state = MAIN_MENU;
    
    while ((state = ui(state, validation, cache)) != EXIT);
    
    ui(EXIT, validation, cache);
    
    return 0;
}
