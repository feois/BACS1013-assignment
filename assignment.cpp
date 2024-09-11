#include <iostream>
#include <iomanip>
#include <cctype>
#include <ctime>
#include <array>
#include <bitset>
#include <map>
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

template<typename T, typename U>
bool has_key(const unordered_map<T, U> &map, const T &t) {
    return map.find(t) != map.end();
}

template<typename T, typename U>
bool has_key(const map<T, U> &map, const T &t) {
    return map.find(t) != map.end();
}

template<typename T>
struct static_vec {
    const T *arr;
    size_t size;
    
    constexpr static_vec(): arr(nullptr), size(0) {}
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

template<typename T>
struct ArrayList {
    T *array;
    size_t length;
    size_t count;
    
    constexpr ArrayList(): array(nullptr), length(0), count(0) {}
    
    void add(T t) {
        if (count + 1 >= length) {
            const auto new_length = length + 10;
            T *new_array = new T[new_length];
            
            if (array != nullptr) {
                for (int i = 0; i < count; i++)
                    new_array = array[i];
                
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
};

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
    SALES_REPORT_CUSTOMER_SUMMARY,
    SALES_REPORT_EXPERT_SUMMARY,
    SALES_REPORT_FINANCE_SUMMARY,
    INVALID_USER,
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
    double consultation_fee;
    double treatment_fee;
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
    ArrayList<Booking> booking_history;
    int total_payment;
};

#define unpack(...) __VA_ARGS__
#define concat(x, y) x ## y
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

constexpr auto MAX_EXPERT_NAME_LENGTH = find_longest_expert_name()->len;
constexpr Staff ADMIN { "Administrator", MALE, "admin", "12345678" };
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

struct tm reset_day(const struct tm &t) {
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

void line() {
    coutfill('-', 20);
    cout << endl;
}

bool is_phone_number(const string &s) {
    for (const char &c : s)
        if (!isdigit(c))
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
    unordered_map<string, Booking> all_bookings;
    map<string, Customer> all_customers;
    array<map<time_t, string>, EXPERTS.size()> treatment_schedule, consultation_schedule;
    const Staff *login_user;
    struct tm view_time;
    time_t book_day;
    ServiceType service_type;
    Booking booking;
    Customer customer;
    int selected_hour;
    int new_booking_id;
    int view_page;
    map<string, Customer>::iterator customer_view;
    
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
        
        center(constexpr, CELL_WIDTH * WEEKDAY_NAMES.size(), 3 + 4 + 1, MONTH_NAMES[view_time.tm_mon] << " " << view_time.tm_year + 1900)
        
        cout << endl;
        
        for (const auto &weekday : WEEKDAY_NAMES) {
            center(constexpr, CELL_WIDTH - 1, 3, weekday)
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
        constexpr static_str LABEL = "Experts";
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
                
                center(constexpr, CELL_SIZE, 1, (schedule[t][i][j] ? "✗" : "✓"));
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

State ui(const State state, bool &validation, Cache &cache)
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
        #define OPT_MAIN_MENU(menu) {'m', "Return to main menu", menu}
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
            
            
        opts(ABOUT_US, (OPT_MAIN_MENU(MENU)))
            cout << "About Us" << endl;
            break;
            
            
        opts(CUSTOMER, (
            {'s', "View a list of available services", SERVICE_LIST},
            {'e', "View a list of available experts", EXPERT_LIST},
            {'b', "Book service", BOOKING},
            {'v', "View appointments", APPOINTMENTS},
            OPT_MAIN_MENU(MENU),
        ))
            cout << "Customer services" << endl;
            line();
            break;
            
            
        case SERVICE_LIST:
            cout << "Available services" << endl;
            line();
            
            cache.booking.service = -1;
            
            custom_input(
                SERVICES.size(),
                CUSTOMER,
                true,
                SERVICES[i].name,
                ({
                    cache.booking.service = i;
                    redirect(VIEW_SERVICE)
                })
            )
            
            
        opts(VIEW_SERVICE, (
            {'b', "Book now", BOOKING},
            {'s', "Go back to service list", SERVICE_LIST},
        ))
        {
            const auto &service = SERVICES[cache.booking.service];
            
            cout << service.name << " Service (Consultation: RM " << setprecision(2) << fixed << service.consultation_fee << "; Treatment: RM " << service.treatment_fee << ")" << endl;
            line();
            cout << service.description << endl;
            
            break;
        }
            
            
        opts(EXPERT_LIST, (
            {'b', "Book an appointment with our experts", BOOKING},
            {'c', "Go back", CUSTOMER},
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
            {'a', "Previous month", BOOK_VIEW_PREV_MONTH},
            {'d', "Next month", BOOK_VIEW_NEXT_MONTH},
            {'b', "Book a day", BOOK_SELECT_DAY},
            {'x', "Cancel", CUSTOMER},
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
            {'t', "Book treatment", BOOK_TREATMENT},
            {'c', "Book consultation", BOOK_CONSULTATION},
            {'x', "Cancel", BOOK_CANCEL},
        ))
            cout << "Booking " << format_date(cache.booking.time) << endl;
            break;
            
            
        case BOOK_TREATMENT:
            cache.print_day_schedule(cache.booking.time);
            
            cout << endl << "Select a time to book" << endl;
            
            custom_input(
                WORK_HOURS - 1,
                BOOK_CANCEL,
                cache.check_hour_treatment_availability(cache.book_day, i),
                OPENING_HOUR + i << ":00~" << OPENING_HOUR + i + 2 << ":00",
                ({
                    cache.service_type = TREATMENT;
                    cache.booking.time.tm_hour = OPENING_HOUR + i;
                    cache.selected_hour = i;
                    redirect(BOOK_SELECT_EXPERT)
                })
            )
        
        
        case BOOK_CONSULTATION:
            cache.print_day_schedule(cache.booking.time);
            
            cout << endl << "Select a time to book" << endl;
            
            custom_input(
                WORK_HOURS,
                BOOK_CANCEL,
                cache.check_hour_consultation_availability(cache.book_day, i),
                OPENING_HOUR + i << ":00~" << OPENING_HOUR + i + 1 << ":00",
                ({
                    cache.service_type = CONSULTATION;
                    cache.booking.time.tm_hour = OPENING_HOUR + i;
                    cache.selected_hour = i;
                    redirect(BOOK_SELECT_EXPERT)
                })
            )
        
        
        case BOOK_SELECT_EXPERT:
            cout << "Booking a " << (cache.service_type == TREATMENT ? "treatment" : "consultation") << " on " << cache.format_duration() << endl;
            cout << endl << "Select an expert to book" << endl;
            
            custom_input(
                EXPERTS.size(),
                BOOK_CANCEL,
                cache.service_type == TREATMENT ? cache.try_book_treatment(cache.book_day, cache.selected_hour, i) : cache.try_book_consultation(cache.book_day, cache.selected_hour, i),
                "Expert " << EXPERTS[i].name,
                ({
                    cache.booking.expert = i;
                    redirect(BOOK_SELECT_SERVICE)
                })
            )
        
        
        case BOOK_SELECT_SERVICE:
            if (cache.booking.service != -1)
                redirect(BOOK_ENTER_DETAILS)
            
            cout << "Booking a " << (cache.service_type == TREATMENT ? "treatment" : "consultation") << " on " << cache.format_duration() << endl;
            
            custom_input(
                SERVICES.size(),
                BOOK_CANCEL,
                true,
                "Service " << SERVICES[i].name,
                ({
                    cache.booking.service = i;
                    redirect(BOOK_ENTER_DETAILS)
                })
            )
        
        
        opts(BOOK_ENTER_DETAILS, ({'r', "Retry", BOOK_ENTER_DETAILS}))
            cout << "Customer Details" << endl;
            line();
            cout << "Enter your name: ";
            cin >> cache.customer.name;
            
            if (!has_key(cache.all_customers, cache.customer.name)) {
                cout << "Enter your gender (M/F): ";
                
                while (true) {
                    const char c = input();
                    
                    if (c == 'm') {
                        cache.customer.gender = MALE;
                        cout << "Male";
                        break;
                    }
                    if (c == 'f') {
                        cache.customer.gender = FEMALE;
                        cout << "Female";
                        break;
                    }
                }
                
                cout << endl << "Enter your phone number (numbers only): ";
                cin >> cache.customer.phone_number;
                
                if (cache.customer.phone_number.size() > 11 || !is_phone_number(cache.customer.phone_number)) {
                    cout << "Invalid input" << endl;
                    break;
                }
            }
            
            redirect(BOOK_CONFIRM)
        
            
        opts(BOOK_CONFIRM, (
            {'y', "Yes, I am sure that the booking information is indeed what I wanted", BOOK_PAYMENT},
            {'n', "No, there are mistakes in the booking information", BOOK_CANCEL},
        ))
        {
            const auto &service = SERVICES[cache.booking.service];
            
            cout << "Please confirm that the following information are correct" << endl;
            line();
            cout << "Name: " << cache.customer.name << endl;
            cout << "Gender: " << (cache.customer.gender == MALE ? "Male" : "Female") << endl;
            cout << "Contact number: " << cache.customer.phone_number << endl << endl;
            cout << (cache.service_type == TREATMENT ? "Treatment" : "Consultation") << " during " << cache.format_duration() << endl;
            cout << "Expert: " << EXPERTS[cache.booking.expert].name << endl;
            cout << "Service: " << service.name << endl;
            cout << service.description << endl;
            cout << endl << "Fee: RM " << setprecision(2) << fixed << (cache.service_type == TREATMENT ? service.treatment_fee : service.consultation_fee) << endl;
            break;
        }
        
        
        opts(BOOK_PAYMENT, (
            {'c', "Credit card", BOOK_SUCCESS},
            {'e', "EWallet", BOOK_SUCCESS},
            {'b', "Online banking", BOOK_SUCCESS},
        ))
            cout << "Select your payment option" << endl;
            break;
        
        
        opts(BOOK_SUCCESS, (OPT_MAIN_MENU(CUSTOMER)))
        {
            const auto &service = SERVICES[cache.booking.service];
            
            string id = "B";
            
            id += to_string(cache.new_booking_id);
            
            auto &sched = (cache.service_type == TREATMENT ? cache.treatment_schedule : cache.consultation_schedule);
            
            sched[cache.booking.expert].insert({mktime(&cache.booking.time), id});
            
            cache.booking.service_type = cache.service_type;
            
            cache.all_bookings.insert({std::move(id), cache.booking});
            
            if (has_key(cache.all_customers, cache.customer.name)) {
                auto &c = cache.all_customers[cache.customer.name];
                
                c.booking_history.add(cache.booking);
                c.total_payment += cache.service_type == TREATMENT ? service.treatment_fee : service.consultation_fee;
            }
            else {
                cache.customer.booking_history = {};
                cache.customer.total_payment = 0;
                
                cache.all_customers.insert({cache.customer.name, cache.customer});
            }
            
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
            {'v', "View another appointment", APPOINTMENTS},
            {'c', "Go back", CUSTOMER},
        ))
        {
            string id;
            
            cout << "Please enter the appointment id: ";
            cin >> id;
            cout << endl;
            
            if (has_key(cache.all_bookings, id)) {
                const auto &appointment = cache.all_bookings[id];
                
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
        
            
        opts(STAFF, (
            {'l', "Login as staff", LOGIN},
            OPT_MAIN_MENU(MENU),
        ))
            break;
            
            
        opts(LOGIN, (
            {'r', "Retry", LOGIN},
            OPT_MAIN_MENU(MENU),
        ))
            
            {
                string username, password;
                
                cout << "Username: ";
                cin >> username;
                cout << "Password: ";
                cin >> password;
                
                cout << endl;
                
                if (username == ADMIN.username.str)
                    cache.login_user = &ADMIN;
                else {
                    for (const Staff &expert : EXPERTS) {
                        if (expert.username.str == username) {
                            cache.login_user = &expert;
                            break;
                        }
                    }
                }
                
                if (cache.login_user == nullptr)
                    cout << "Invalid username" << endl;
                else if (password == cache.login_user->password.str)
                    redirect(STAFF_MENU)
                else
                    cout << "Invalid password" << endl;
                
                cache.login_user = nullptr;
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
            cout << "Currently login as " << cache.login_user->username << " (" << cache.login_user->name << ")" << endl;
            
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
            {'a', "Previous week", SCHEDULE_VIEW_PREV_WEEK},
            {'d', "Next week", SCHEDULE_VIEW_NEXT_WEEK},
            OPT_MAIN_MENU(STAFF_MENU),
        ))
        {
            if (cache.login_user == &ADMIN)
                redirect(INVALID_USER)
            
            cache.view_time.tm_hour = OPENING_HOUR;
            
            constexpr auto LABEL_SIZE = 5;
            constexpr auto CELL_SIZE = 5;
            constexpr auto LINE_WIDTH = LABEL_SIZE + WEEKDAY_NAMES.size() * (CELL_SIZE + 1);
            constexpr static_str SEPARATOR = " ~ ";
            
            auto we = cache.view_time;
            
            we.tm_mday += WEEKDAY_NAMES.size() - 1;
            
            mktime(&cache.view_time);
            mktime(&we);
            
            const string week_start = format_date(cache.view_time);
            const string week_end = format_date(we);
            
            center(const, LINE_WIDTH, week_start.length() + SEPARATOR.len + week_end.length(), week_start << SEPARATOR << week_end)
            
            cout << endl;
            coutfill(' ', 5);
            
            for (const auto &weekday : WEEKDAY_NAMES) {
                cout << '|';
                center(constexpr, 5, 3, weekday);
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
            cache.customer_view = cache.all_customers.begin();
            redirect(CUSTOMER_VIEW)
        
        
        opts(CUSTOMER_VIEW, (
            {'a', "Previous page", CUSTOMER_VIEW_PREV_PAGE},
            {'d', "Next page", CUSTOMER_VIEW_NEXT_PAGE},
            OPT_MAIN_MENU(STAFF_MENU),
        ))
            if (cache.all_customers.size() > 0) {
                cout << "Customers (Page " << cache.view_page + 1 << " of " << (cache.all_customers.size() + CUSTOMER_ENTRIES_PER_PAGE - 1) / CUSTOMER_ENTRIES_PER_PAGE << ')' << endl;
                line();
                
                const auto offset = cache.view_page * CUSTOMER_ENTRIES_PER_PAGE;
                
                auto view = cache.customer_view;
                
                for (auto i = 0; i < CUSTOMER_ENTRIES_PER_PAGE && view != cache.all_customers.end(); i++, view++) {
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
            if (cache.view_page + 1 < (cache.all_customers.size() + CUSTOMER_ENTRIES_PER_PAGE - 1) / CUSTOMER_ENTRIES_PER_PAGE) {
                cache.view_page++;
                
                for (auto i = 0; i < CUSTOMER_ENTRIES_PER_PAGE; i++)
                    cache.customer_view++;
            }
            redirect(CUSTOMER_VIEW)
            
            
        opts(SALES_REPORT, (
            {'c', "Customer summary", SALES_REPORT_CUSTOMER_SUMMARY},
            {'e', "Expert summary", SALES_REPORT_EXPERT_SUMMARY},
            {'f', "Finance summary", SALES_REPORT_FINANCE_SUMMARY},
        ))
            cout << "Sales report" << endl;
            break;
        
        
        opts(SALES_REPORT_CUSTOMER_SUMMARY, (OPT_MAIN_MENU(STAFF_MENU)))
            cout << "Sales Report: Customer Summary" << endl;
            line();
            
            if (cache.all_customers.size() > 0) {
                const Customer *most_appointments = nullptr;
                const Customer *most_paid = nullptr;
                
                for (const auto &p : cache.all_customers) {
                    const auto &customer = p.second;
                    
                    if (most_appointments == nullptr || most_appointments->booking_history.count < customer.booking_history.count)
                        most_appointments = &customer;
                    
                    if (most_paid == nullptr || most_paid->total_payment < customer.total_payment)
                        most_paid = &customer;
                }
                
                {
                    const auto &customer = *most_appointments;
                    const auto treatment_count = customer.booking_history.count_if([](const Booking &a) { return a.service_type == TREATMENT; });
                    const auto consultation_count = customer.booking_history.count - treatment_count;
                    
                    cout << "Customer who booked the highest count of appointments: " << customer.name << endl;
                    cout << "\tGender: " << (customer.gender == MALE ? "Male" : "Female") << endl;
                    cout << "\tAppointment count: " << customer.booking_history.count << endl;
                    cout << "\tBooked consultation count: " << consultation_count << endl;
                    cout << "\tBooked treatment count: " << treatment_count << endl;
                    cout << "\tTotal Payment: " << customer.total_payment << endl;
                }
                
                line();
                
                {
                    const auto &customer = *most_paid;
                    const auto treatment_count = customer.booking_history.count_if([](const Booking &a) { return a.service_type == TREATMENT; });
                    const auto consultation_count = customer.booking_history.count - treatment_count;
                    
                    cout << "Customer who booked the highest count of appointments: " << customer.name << endl;
                    cout << "\tGender: " << (customer.gender == MALE ? "Male" : "Female") << endl;
                    cout << "\tAppointment count: " << customer.booking_history.count << endl;
                    cout << "\tBooked consultation count: " << consultation_count << endl;
                    cout << "\tBooked treatment count: " << treatment_count << endl;
                    cout << "\tTotal Payment: " << customer.total_payment << endl;
                }
            }
            else
                cout << "Not enough data" << endl;
            
            break;
            
        
        
        opts(SALES_REPORT_EXPERT_SUMMARY, (OPT_MAIN_MENU(STAFF_MENU)))
        {
            constexpr static_str HOURS_WORKED = "Hours Worked", TOTAL_SALES = "Total Sales";
            constexpr auto MAX_RECORD_NAME_LEN = max(HOURS_WORKED.len, TOTAL_SALES.len);
            
            coutfill(' ', MAX_RECORD_NAME_LEN);
            
            break;
        }
        
        
        opts(SALES_REPORT_FINANCE_SUMMARY, (OPT_MAIN_MENU(STAFF_MENU)))
            break;
            
            
        opts(LOGOUT, (
            {'l', "Log in", LOGIN},
            OPT_MAIN_MENU(MENU),
        ))
            cache.login_user = nullptr;
            cout << "Successfully logged out!" << endl;
            break;
        
        
        opts(INVALID_USER, (OPT_MAIN_MENU(STAFF_MENU)))
            cout << "Unavailable to this user" << endl;
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
    Cache cache {
        .schedule = {},
        .all_bookings = {},
        .all_customers = {},
        .treatment_schedule = {},
        .consultation_schedule = {},
        .login_user = nullptr,
        .booking = { .service = -1, .expert = -1 },
        .selected_hour = -1,
        .new_booking_id = 1000,
        .view_page = 0,
    };
    State state = ui(MENU, validation, cache);
    
    while (state != EXIT) {
        state = ui(state, validation, cache);
    }
    
    ui(state, validation, cache);
    
    return 0;
}
