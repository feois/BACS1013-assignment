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
struct ArrayList {
    T *array;
    size_t length;
    size_t count;
    
    constexpr ArrayList(): array(nullptr), length(0), count(0) {}
    
    ArrayList(const size_t n): array(new T[n]), length(n), count(0) {}
    
    ~ArrayList() {
        if (array != nullptr)
            delete array;
    }
    
    void add(T t) {
        if (count + 1 >= length) {
            const auto new_length = length + 10;
            T *new_array = new T[new_length];
            
            if (array != nullptr) {
                for (int i = 0; i < count; i++)
                    new_array[i] = array[i];
                
                delete[] array;
            }
            
            array = new_array;
            length = new_length;
        }
        
        array[count++] = t;
    }
    
    template<typename F>
    constexpr size_t count_if(F f) const {
        size_t n = 0;
        
        for (int i = 0; i < count; i++)
            if (f(array[count]))
                n++;
        
        return n;
    }
    
    constexpr T *move_array() {
        const auto a = array;
        
        array = nullptr;
        
        return a;
    }
};

template<typename T>
struct ConstVec {
    const T *arr;
    size_t size;
    
    constexpr ConstVec(): arr(nullptr), size(0) {}
    constexpr ConstVec(initializer_list<T> a): arr(a.begin()), size(a.size()) {}
    constexpr ConstVec(ArrayList<T> a): arr(a.move_array()), size(a.count) {}
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
    struct tm time;
};

struct CustomerData {
    ArrayList<Booking> booking_history;
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

auto time_now(struct tm &tm) {
    const auto t = time(nullptr);
    tm = *localtime(&t);
    return t;
}

static array<char, 128> format_string = {};

#define define_format(name, format) const char *name(const struct tm &t) { \
    strftime(format_string.data(), format_string.size(), format, &t); \
    return format_string.data(); \
}

define_format(format_time, "%A %e %B %G (%F) %T");
define_format(format_date, "%A %e %B %G (%F)");

constexpr struct tm reset_day(const struct tm &t) {
    auto u = t;
    u.tm_hour = 0;
    u.tm_min = 0;
    u.tm_sec = 0;
    return u;
}

void coutfill(char c, size_t n) {
    if (n > 0)
        cout << setfill(c) << setw(n) << c;
}

template<typename F>
void center(const size_t line_width, const size_t string_width, F f) {
    if (string_width > line_width) f();
    else {
        const auto padding = line_width - string_width;
        const auto left = padding / 2;
        const auto right = padding - left;
        
        coutfill(' ', left);
        f();
        coutfill(' ', right);
    }
}

void line() {
    coutfill('-', 20);
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
        
        bool operator[](size_t hour) { return bs != nullptr && (*bs)[expert * 6 + hour]; }
    };

    struct ExpertScheduleSet {
        const Bitset *bs;
        
        constexpr ExpertSchedule operator[](size_t expert) const { return { bs, expert }; }
    };
    
    struct BitSetter {
        Bitset &bs;
        size_t expert;
        
        typename Bitset::reference operator[](size_t hour) { return bs[expert * 6 + hour]; }
    };
    
    struct ArraySetter {
        Bitset &bs;
        
        BitSetter operator[](size_t expert) { return { bs, expert }; }
    };
    
    struct MapSetter {
        ExpertScheduleMap &map;
        
        ArraySetter operator[](time_t day) {
            if (!has_key(map.get_map(), day))
                map.get_map().insert({day, {}});
            
            return { map.get_map()[day] };
        }
    };
    
    Map _map;
    
    constexpr Map &get_map() { return _map; }
    ExpertScheduleSet operator[](time_t day) { return { (has_key(_map, day) ? &_map[day] : nullptr) }; }
    constexpr ExpertScheduleSet operator[](const struct tm &t) const { auto u = reset_day(t); return this[mktime(&u)]; }
    
    MapSetter set() { return {*this}; }
};

struct Cache {
    ExpertScheduleMap<EXPERTS.size(), WORK_HOURS> schedule;
    unordered_map<string, Booking> booking_database;
    map<string, User> customer_database;
    array<map<time_t, string>, EXPERTS.size()> treatment_schedule, consultation_schedule;
    const User *login_user;
    struct tm view_time;
    time_t book_day;
    ServiceType service_type;
    Booking booking;
    int selected_hour;
    int new_booking_id;
    int view_page;
    map<string, User>::iterator customer_view;
    
    bool try_book_treatment(const time_t t, const int hour, const int expert) {
        return !(schedule[t][expert][hour] || schedule[t][expert][hour + 1]);
    }

    bool try_book_consultation(const time_t t, const int hour, const int expert) {
        return !schedule[t][expert][hour];
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
        for (int i = 0; i < WORK_HOURS - 1; i++)
            if (check_hour_treatment_availability(t, i))
                return true;
        
        return false;
    }

    bool check_day_consultation_availability(const time_t t) {
        return !(has_key(schedule.get_map(), t) && schedule[t].bs->all());
    }
    
    void print_calendar(time_t today) {
        constexpr int CELL_WIDTH = 6;
        
        center(CELL_WIDTH * WEEKDAY_NAMES.size(), 3 + 4 + 1, [&]() { cout << MONTH_NAMES[view_time.tm_mon] << " " << view_time.tm_year + 1900; });
        
        cout << endl;
        
        for (const auto &weekday : WEEKDAY_NAMES) {
            center(CELL_WIDTH - 1, 3, [&]() { cout << weekday; });
            cout << ' ';
        }
        
        cout << endl;
        
        auto view_day = reset_day(view_time);
        view_day.tm_mday = 1;
        
        auto t = mktime(&view_day);
        
        if (view_day.tm_wday > 0)
            coutfill(' ', view_day.tm_wday * CELL_WIDTH);
        
        while (view_day.tm_mon == view_time.tm_mon) {
            cout << setw(2) << view_day.tm_mday++ << ' ';
            
            if (t < today)
                cout << "--";
            else
                cout << (check_day_consultation_availability(t) ? 'C' : '-') << (check_day_treatment_availability(t) ? 'T' : '-');
            
            cout << ' ';
            
            if (view_day.tm_wday == 6)
                cout << endl;
            
            t = mktime(&view_day);
        }
        
        if (view_day.tm_wday != 0)
            cout << endl;
    }

    void print_day_schedule(struct tm &date) {
        constexpr ConstStr LABEL = "Experts";
        constexpr auto COLUMN_SIZE = max(LABEL.len, MAX_EXPERT_NAME_LENGTH);
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
                
                center(CELL_SIZE, 1, [&]() { cout << (schedule[t][i][j] ? "✗" : "✓"); });
            }
            
            cout << endl;
        }
    }
    
    string format_duration() const {
        auto s = string(format_time(booking.time));
        auto t = booking.time;
        
        t.tm_hour += service_type == TREATMENT ? 2 : 1;
        mktime(&t);
        
        strftime(format_string.data(), format_string.size(), "~%T", &t);
        
        s += format_string.data();
        
        return s;
    }
};

template<typename F, typename G, typename H>
State custom_input(const State state, bool &validation, const size_t option_count, const State cancel_state, F checker, G description, H matching) {
    if (!validation)
        cout << endl << "Invalid input!" << endl;
    
    validation = true;
    
    ArrayList<size_t> indexes { option_count };
    
    cout << endl << "Please select any of the following options" << endl;
    
    for (size_t i = 0; i < option_count; i++)
        if (checker(i)) {
            cout << static_cast<char>('1' + indexes.count) << ": ";
            description(i);
            cout << endl;
            indexes.add(i);
        }
    
    cout << static_cast<char>('1' + indexes.count) << ": Cancel" << endl;
    
    const char c = input_key() - '1';
    
    if (c == indexes.count)
        return cancel_state;
    
    if (c >= 0 && c < option_count) {
        return matching(indexes.array[c]);
    }
    
    validation = false;
    return state;
}

State ui(const State state, bool &validation, Cache &cache)
{
    #define refresh return state;
    #define redirect(state) return state;
    
    struct tm now;
    
    time_now(now);
    
    ConstVec<Option> options {};
    
    clear();
    
    cout << "COMPANY LOGO" << endl;
    line();
    cout << format_time(now) << endl;
    line();
    
    if (cache.login_user != nullptr) {
        cout << "Currently login as " << cache.login_user->username << " (" << cache.login_user->name << ")" << endl;
        line();
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
            line();
            break;
            
            
        case SERVICE_LIST:
            cout << "Available services" << endl;
            line();
            
            cache.booking.service = -1;
            
            return custom_input(
                state,
                validation,
                SERVICES.size(),
                CUSTOMER_MENU,
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
            line();
            cout << service.description << endl;
            
            break;
        }
            
            
        opts(EXPERT_LIST, (
            {"Book an appointment with our experts", BOOKING},
            {"Go back", CUSTOMER_MENU},
        ))
            cout << "Our experts:" << endl;
            
            for (const auto &expert : EXPERTS) {
                cout << "- " << expert.name << " (" << (expert.gender == MALE ? "Male" : "Female") << ')' << endl;
            }
            
            break;
            
            
        case BOOKING:
            cache.view_time = reset_day(now);
            redirect(BOOK_VIEW)
            
            
        opts(BOOK_VIEW, (
            {"Previous month", BOOK_VIEW_PREV_MONTH},
            {"Next month", BOOK_VIEW_NEXT_MONTH},
            {"Book a day", BOOK_SELECT_DAY},
            {"Cancel", CUSTOMER_MENU},
        ))
        {
            auto today = reset_day(now);
            cache.print_calendar(mktime(&today));
            
            cout << "C means consultation is available to book" << endl;
            cout << "T means treatment is available to book" << endl;
            
            break;
        }
            
            
        case BOOK_VIEW_PREV_MONTH:
            {
                auto this_month = reset_day(now);
                this_month.tm_mday = 1;
                
                auto prev_month = cache.view_time;
                
                prev_month.tm_mon -= 1;
                
                if (mktime(&prev_month) >= mktime(&this_month))
                    cache.view_time = prev_month;
            }
            redirect(BOOK_VIEW)
            
            
        case BOOK_VIEW_NEXT_MONTH:
            cache.view_time.tm_mon += 1;
            mktime(&cache.view_time);
            redirect(BOOK_VIEW)
            
            
        case BOOK_SELECT_DAY:
            {
                string input;
                
                auto today = reset_day(now);

                cache.print_calendar(mktime(&today));
                
                if (!validation)
                    cout << endl << "Please input a future day in the month that is available for booking!" << endl;
                
                cout << endl << "Enter the day to book (x to cancel): ";
                
                cin >> input;
                
                if (input == "x")
                    redirect(BOOK_CANCEL)
                
                try {
                    const unsigned int day = stoi(input);
                    
                    if (day < 32) {
                        auto selected_day = cache.view_time;
                        
                        selected_day.tm_mday = day;
                        
                        const auto t = mktime(&selected_day);
                        
                        if (selected_day.tm_mon == cache.view_time.tm_mon) {
                            cache.booking.time = selected_day;
                            cache.book_day = t;
                            
                            const bool consultation = cache.check_day_consultation_availability(t);
                            const bool treatment = cache.check_day_treatment_availability(t);
                            
                            auto today = reset_day(now);
                            
                            if (t >= mktime(&today) && (consultation || treatment)) {
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
            cache.print_day_schedule(cache.booking.time);
            
            cout << endl << "Select a time to book" << endl;
            
            return custom_input(
                state,
                validation,
                WORK_HOURS - 1,
                BOOK_CANCEL,
                [&](size_t i) { return cache.check_hour_treatment_availability(cache.book_day, i); },
                [](size_t i) { cout << OPENING_HOUR + i << ":00~" << OPENING_HOUR + i + 2 << ":00"; },
                [&](size_t i) {
                    cache.service_type = TREATMENT;
                    cache.booking.time.tm_hour = OPENING_HOUR + i;
                    cache.selected_hour = i;
                    redirect(BOOK_SELECT_EXPERT)
                }
            );
        
        
        case BOOK_CONSULTATION:
            cache.print_day_schedule(cache.booking.time);
            
            cout << endl << "Select a time to book" << endl;
            
            return custom_input(
                state,
                validation,
                WORK_HOURS,
                BOOK_CANCEL,
                [&](size_t i) { return cache.check_hour_consultation_availability(cache.book_day, i); },
                [](size_t i) { cout << OPENING_HOUR + i << ":00~" << OPENING_HOUR + i + 1 << ":00"; },
                [&](size_t i) {
                    cache.service_type = CONSULTATION;
                    cache.booking.time.tm_hour = OPENING_HOUR + i;
                    cache.selected_hour = i;
                    redirect(BOOK_SELECT_EXPERT)
                }
            );
        
        
        case BOOK_SELECT_EXPERT:
            cout << "Booking a " << (cache.service_type == TREATMENT ? "treatment" : "consultation") << " on " << cache.format_duration() << endl;
            cout << endl << "Select an expert to book" << endl;
            
            return custom_input(
                state,
                validation,
                EXPERTS.size(),
                BOOK_CANCEL,
                [&](size_t i) { return cache.service_type == TREATMENT ? cache.try_book_treatment(cache.book_day, cache.selected_hour, i) : cache.try_book_consultation(cache.book_day, cache.selected_hour, i); },
                [](size_t i) { cout << "Expert " << EXPERTS[i].name; },
                [&](size_t i) {
                    cache.booking.expert = i;
                    redirect(BOOK_SELECT_SERVICE)
                }
            );
        
        
        case BOOK_SELECT_SERVICE:
            if (cache.booking.service != -1)
                redirect(BOOK_CONFIRM)
            
            cout << "Booking a " << (cache.service_type == TREATMENT ? "treatment" : "consultation") << " on " << cache.format_duration() << endl;
            
            return custom_input(
                state,
                validation,
                SERVICES.size(),
                BOOK_CANCEL,
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
            line();
            cout << "Name: " << customer.name << endl;
            cout << "Gender: " << (customer.gender == MALE ? "Male" : "Female") << endl;
            cout << "Contact number: " << customer.phone_number << endl << endl;
            cout << (cache.service_type == TREATMENT ? "Treatment" : "Consultation") << " during " << cache.format_duration() << endl;
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
            
            sched[cache.booking.expert].insert({mktime(&cache.booking.time), id});
            
            cache.booking.service_type = cache.service_type;
            cache.booking_database.insert({std::move(id), cache.booking});
            cache.login_user->customer_data->booking_history.add(cache.booking);
            cache.login_user->customer_data->total_payment += cache.service_type == TREATMENT ? service.treatment_fee : service.consultation_fee;
            
            if (has_key(cache.schedule.get_map(), cache.book_day))
                cache.schedule.get_map().insert({cache.book_day, {}});
            
            for (int i = 0; i < (cache.service_type == TREATMENT ? 2 : 1); i++)
                cache.schedule.set()[cache.book_day][cache.booking.expert][cache.selected_hour + i] = true;
            
            cout << "Successfully booked the appointment!" << endl;
            cout << endl << "Appointment id: B" << cache.new_booking_id++ << endl;
            cout << (cache.service_type == TREATMENT ? "Treatment" : "Consultation") << " during " << cache.format_duration() << endl;
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
                
                cache.booking.time = appointment.time;
                cache.service_type = appointment.service_type;
                
                cout << "Your appointment is a " << SERVICES[appointment.service].name << " " << (cache.service_type == TREATMENT ? "treatment" : "consultation") << " during " << cache.format_duration() << endl;
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
            cache.view_time = reset_day(now);
            cache.view_time.tm_mday -= cache.view_time.tm_wday;
            redirect(SCHEDULE_VIEW)
            
            
        case CONSULTATION_SCHEDULE:
            cache.service_type = CONSULTATION;
            cache.view_time = reset_day(now);
            cache.view_time.tm_mday -= cache.view_time.tm_wday;
            redirect(SCHEDULE_VIEW)
            
        
        opts(SCHEDULE_VIEW, (
            {"Previous week", SCHEDULE_VIEW_PREV_WEEK},
            {"Next week", SCHEDULE_VIEW_NEXT_WEEK},
            OPT_MAIN_MENU(STAFF_MENU),
        ))
        {
            if (cache.login_user == &ADMIN)
                redirect(INVALID_USER)
            
            cache.view_time.tm_hour = OPENING_HOUR;
            
            constexpr auto LABEL_SIZE = 5;
            constexpr auto CELL_SIZE = 5;
            constexpr auto LINE_WIDTH = LABEL_SIZE + WEEKDAY_NAMES.size() * (CELL_SIZE + 1);
            constexpr ConstStr SEPARATOR = " ~ ";
            
            auto we = cache.view_time;
            
            we.tm_mday += WEEKDAY_NAMES.size() - 1;
            
            mktime(&cache.view_time);
            mktime(&we);
            
            const string week_start = format_date(cache.view_time);
            const string week_end = format_date(we);
            
            center(LINE_WIDTH, week_start.length() + SEPARATOR.len + week_end.length(), [&]() { cout << week_start << SEPARATOR << week_end; });
            
            cout << endl;
            coutfill(' ', 5);
            
            for (const auto &weekday : WEEKDAY_NAMES) {
                cout << '|';
                center(5, 3, [&]() { cout << weekday; });
            }
            
            cout << endl;
            
            for (int i = 0; i < WORK_HOURS; i++, cache.view_time.tm_hour++) {
                coutfill('-', LABEL_SIZE);
                
                for (int j = 0; j < WEEKDAY_NAMES.size(); j++) {
                    cout << '+';
                    coutfill('-', CELL_SIZE);
                }
                
                cout << endl << OPENING_HOUR + i << ":00";
                
                for (int j = 0; j < WEEKDAY_NAMES.size(); j++, cache.view_time.tm_mday++) {
                    const auto t = mktime(&cache.view_time);
                    const string *id = nullptr;
                    
                    if (cache.service_type == TREATMENT) {
                        if (has_key(cache.treatment_schedule[cache.booking.expert], t))
                            id = &cache.treatment_schedule[cache.booking.expert][t];
                        else if (i > 0) {
                            auto prev_hour = cache.view_time;
                            
                            prev_hour.tm_hour -= 1;
                            
                            const auto pt = mktime(&prev_hour);
                            
                            if (has_key(cache.treatment_schedule[cache.booking.expert], pt))
                                id = &cache.treatment_schedule[cache.booking.expert][pt];
                        }
                    }
                    else if (has_key(cache.consultation_schedule[cache.booking.expert], t))
                        id = &cache.consultation_schedule[cache.booking.expert][t];
                    
                    cout << '|' << (id == nullptr ? "  -  " : *id);
                }
                
                cache.view_time.tm_mday -= WEEKDAY_NAMES.size();
                
                cout << endl;
            }
            
            break;
        }
        
        
        case SCHEDULE_VIEW_PREV_WEEK:
        {
            auto this_week = reset_day(now);
            auto prev_week = cache.view_time;
            
            this_week.tm_mday -= this_week.tm_wday;
            prev_week.tm_mday -= WEEKDAY_NAMES.size();
            
            if (mktime(&prev_week) >= mktime(&this_week))
                cache.view_time = prev_week;
            
            redirect(SCHEDULE_VIEW)
        }
        
        
        case SCHEDULE_VIEW_NEXT_WEEK:
            cache.view_time.tm_mday += WEEKDAY_NAMES.size();
            mktime(&cache.view_time);
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
                line();
                
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
            line();
            
            if (cache.customer_database.size() > 0) {
                const User *most_appointments = nullptr;
                const User *most_paid = nullptr;
                
                for (const auto &p : cache.customer_database) {
                    const auto &customer = p.second;
                    
                    if (most_appointments == nullptr || most_appointments->customer_data->booking_history.count < customer.customer_data->booking_history.count)
                        most_appointments = &customer;
                    
                    if (most_paid == nullptr || most_paid->customer_data->total_payment < customer.customer_data->total_payment)
                        most_paid = &customer;
                }
                
                {
                    const auto &customer = *most_appointments;
                    const auto &data = *customer.customer_data;
                    const auto treatment_count = data.booking_history.count_if([](const Booking &a) { return a.service_type == TREATMENT; });
                    const auto consultation_count = data.booking_history.count - treatment_count;
                    
                    cout << "Customer who booked the highest count of appointments: " << customer.name << endl;
                    cout << "\tGender: " << (customer.gender == MALE ? "Male" : "Female") << endl;
                    cout << "\tAppointment count: " << data.booking_history.count << endl;
                    cout << "\tBooked consultation count: " << consultation_count << endl;
                    cout << "\tBooked treatment count: " << treatment_count << endl;
                    cout << "\tTotal Payment: " << data.total_payment << endl;
                }
                
                line();
                
                {
                    const auto &customer = *most_paid;
                    const auto &data = *customer.customer_data;
                    const auto treatment_count = data.booking_history.count_if([](const Booking &a) { return a.service_type == TREATMENT; });
                    const auto consultation_count = data.booking_history.count - treatment_count;
                    
                    cout << "Customer who paid most for our services: " << customer.name << endl;
                    cout << "\tGender: " << (customer.gender == MALE ? "Male" : "Female") << endl;
                    cout << "\tAppointment count: " << data.booking_history.count << endl;
                    cout << "\tBooked consultation count: " << consultation_count << endl;
                    cout << "\tBooked treatment count: " << treatment_count << endl;
                    cout << "\tTotal Payment: " << data.total_payment << endl;
                }
            }
            else
                cout << "Not enough data" << endl;
            
            break;
            
        
        
        opts(SALES_REPORT_EXPERT_SUMMARY, (OPT_MAIN_MENU(STAFF_MENU)))
        {
            constexpr ConstStr HOURS_WORKED = "Hours Worked", TOTAL_SALES = "Total Sales";
            constexpr auto MAX_RECORD_NAME_LEN = max(HOURS_WORKED.len, TOTAL_SALES.len);
            
            coutfill(' ', MAX_RECORD_NAME_LEN);
            
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
    State state = ui(MAIN_MENU, validation, cache);
    
    while (state != EXIT) {
        state = ui(state, validation, cache);
    }
    
    ui(state, validation, cache);
    
    return 0;
}
